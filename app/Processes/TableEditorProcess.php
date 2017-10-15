<?php
namespace Amderbar\App\Processes;

use Amderbar\Lib\Utils\HtmlUtil as Html;
use Amderbar\Lib\Utils\StrUtil as Str;
use Amderbar\Lib\Utils\DbUtil as Db;
use stdClass;

/**
 *
 * @author amderbar
 *
 */
class TableEditorProcess extends EditorProcess
{
    /**
     * @param int $proj_id
     * @return string
     */
    public function projName(): ?string
    {
        return ($this->open(ROOT_DB_ID)
            ->query('
                select proj_name
                from projects
                where proj_id = :proj_id
            ', [':proj_id' => $this->proj_id])
            ->fetch() ?: new stdClass)
            ->proj_name;
    }

    /**
     * 処理内容メモ
     * ====================
     * 1.
     * EditorServletから引数で情報を受け取る
     * 2. ユーザー指定の表名をシステム上での表名に変換
     * 3. システム表名とユーザー指定表名の組を表名テーブルに登録する(表IDが発行される)
     * 4. 各列定義の情報をパースする
     * 5. CREATE TABLE文の組み立て
     * 6. SQL発行、表の作成
     * 7. 表IDをreturn
     *
     * @param int    $proj_id          対象DBのID
     * @param string $tbl_name         ユーザーが指定したテーブル名
     * @param array  $cols_hash        列定義の情報
     * @param array  $constraints_hash 表制約の情報
     *
     * @return array
     */
    public function createUserTable(string $tbl_name, array $cols_hash): array
    {
        try {
            return $this->open($this->proj_id)
                ->transaction(function (DB $db_con) use ($tbl_name, $cols_hash) {
                    // システムテーブルに表名を登録
                    $actual_tbl_name = $this->generateUniqName('t_' . Str::fnv132($tbl_name), SYSTEM_TBL, 'actual_name');
                    $tbl_id = $db_con->insertGetId(SYSTEM_TBL, [
                        'tbl_name' => $tbl_name,
                        'actual_name' => $actual_tbl_name,
                        'display_order' => $this->getDisplayOrderLast($this->proj_id, SYSTEM_TBL)
                    ]);

                    // システムテーブルに列情報を登録
                    $cols_info = $this->updateForginKeys($this->registerUserColumns($tbl_id, $cols_hash));
                    // SQL前駆体の作成
                    $db_con->query("create table {$actual_tbl_name} ("
                        . implode(',', array_filter(array_map(function ($col) use ($db_con, $tbl_id, $actual_tbl_name) {
                            $sql = "{$col['actual_name']} ";
                            $sql .= ($col['is_primary'] ?? false) ? 'integer primary key' : FORM_TO_DB[$col['form_type'] ?? 'hidden'];
                            $sql .= ($col['default_val'] ?? '') ? " default {$col['default_val']}" : '';
                            $sql .= ($col['not_null'] ?? false) ? " not null" : '';
                            $sql .= ($col['uniq'] ?? false) ? " unique" : '';
                            // 外部参照先の設定がある場合
                            if (isset($col['ref_col'])) {
                                $tbl_col = $this->getColumnInfo($col['ref_col']);

                                if ($col['multiple']) {
                                    // 複数選択列の場合 => 中間表を作成し、実際には列を作らない
                                    return $this->createInternalTbl([
                                        'tbl_id' => $tbl_id,
                                        'tbl_actual' => $actual_tbl_name,
                                        'col_id' => $col['col_id'],
                                        'col_actual' => $col['actual_name']
                                    ], $tbl_col);
                                } else {
                                    // 単一参照の場合 => 実際の列定義では常に参照先表のid列を外部キーに指定
                                    $sql .= " references {$tbl_col->tbl_actual}(id) on delete set null";
                                }
                            }
                            return $sql;
                        }, $cols_info), function ($query) {
                            return boolval($query);
                        })) . ');');

                    return ['tbl_id' => $tbl_id, 'col_id' => array_column($cols_info, 'col_id')];
                });
        } catch (PDOException | DBManageExeption $e) {
            $this->handleException($e);
        }
    }

    /**
     * 処理内容メモ
     * ====================
     * 1. ユーザー指定の列名をシステム上での列名に変換
     * 2. システム列名、ユーザー指定列名、フォーム型(文字列)を列名テーブルに登録(表IDを外部参照 列IDが発行される)
     * 3. 外部参照先が設定されている場合
     *  1. 外部参照先が既存の表、列の場合
     *      その列のIDを登録する
     *  2. 外部参照先が同じ表の列の場合
     *      はじめは参照先NULLで登録し、新表のすべての列の登録が終わったあとでUPDATEする
     *  3. 外部参照先がまだ存在しない表の列の場合
     *      新しい表を作成し、その表のID列のIDを登録する
     *  4. 同時に複数選択であると指定されている場合
     *      この列は実際には列として作らない。
     *      列名と同名の中間表を作成する。Viewの設定もしたほうが良い？
     * 4. フォーム型がnumber, rengeとその仲間の場合
     *    データ刻み幅、最大値、最小値の設定を数値設定テーブルに登録(列IDを参照)
     * 5. フォーム型からデータ型を決定
     * 6. SQLの列定義部分を作成
     *
     * @param int $proj_id
     * @param int $tbl_id
     * @param array $cols_hash
     * @return array
     */
    private function registerUserColumns(int $tbl_id, array $cols_hash): array
    {
        $db_con = $this->open($this->proj_id);
        $disp_order = 0;
        $col_list = [];
        // 主キー列の登録
        $col_info = [
            'tbl_id' => $tbl_id,
            'col_name' => 'No.',
            'actual_name' => 'id',
            'is_primary' => true,
            'multiple' => false,
            'display_order' => $disp_order
        ];
        $col_list[] = array_merge([
            'col_id' => $db_con->insertGetId(SYSTEM_COL, $col_info)
        ], $col_info);
        // 列ごとの登録
        $form_types = array_keys(Html::FROM_TYPES);
        foreach ($cols_hash as $def_hash) {
            $actual_name = $this->generateUniqName('uc_', SYSTEM_COL, 'actual_name');
            $col_info = [
                'tbl_id' => $tbl_id,
                'col_name' => $def_hash['col_name'],
                'actual_name' => $actual_name,
                'form_type' => $def_hash['form_type'],
                'default_val' => $def_hash['default'] ?? '', // 外部参照がある場合はID値への変換が必要
                'multiple' => boolval($def_hash['multiple'] ?? false),
                'display_order' => ++$disp_order
            ];
            $col_id = $db_con->insertGetId(SYSTEM_COL, $col_info);
            if (isset($def_hash['step']) && isset($def_hash['max']) && isset($def_hash['min'])) {
                $option_info = [
                    'col_id' => $col_id,
                    'step' => $def_hash['step'],
                    'max' => $def_hash['max'],
                    'min' => $def_hash['min']
                ];
                $db_con->insertGetId(NUM_SETTINGS, $option_info);
                $col_info += $option_info;
            }
            $col_list[] = array_merge([
                'col_id' => $col_id
            ], $def_hash, $col_info);
        }
        // 最終的な列情報を返す
        return $col_list;
    }

    /**
     * Undocumented function
     *
     * @param array $cols_hash
     * @return array
     */
    private function updateForginKeys(array $cols_hash) :array
    {
        // 外部参照先の設定
        $col_list = $cols_hash;
        foreach (array_filter($cols_hash, function ($def_hash) {
            return isset($def_hash['form_type'])
                && in_array($def_hash['form_type'], [
                    'listext'
                    , 'numlist'
                    , 'select'
                    , 'radio'
                    , 'multicheck'
                ]);
        }) as $i => $def_hash) {
            if (!isset($def_hash['ref_dist'])) {
                // 追加で新しく表を作ってそれを参照する場合
                $tbl_name = '[マスタ]'.$def_hash['col_name'];
                $def_cols = [1 => [
                    'col_name' => $def_hash['col_name'],
                    'form_type' => 'text',
                    'uniq' => true,
                    'not_null' => true
                ]];
                $mstr_tbl = $this->createUserTable($tbl_name, $def_cols);
                $def_hash['ref_dist'] = $mstr_tbl['col_id'][1];
                // TODO:default値が設定されていればそのデータをインサート
            } else if ($def_hash['ref_dist'] < 1) {
                // 自表の列を参照する場合
                $def_hash['ref_dist'] = $col_list[-$def_hash['ref_dist']]['col_id'];
                // TODO:default値が設定されていればそのデータをインサート
            }
            $col_list[$i]['ref_col'] = $def_hash['ref_dist'];
            $this->open($this->proj_id)->query('
                update ' . SYSTEM_COL . '
                set ref_col = :ref_col
                where col_id = :col_id
            ', [
                ':ref_col' => $col_list[$i]['ref_col'],
                ':col_id' => $col_list[$i]['col_id']
            ])
            ->rowCount();
        }
        return $col_list;
    }

    /**
     * @param int   $proj_id
     * @param array $ref_src   ['tbl_actual' => tbl_name, 'col_actual' => col_name]
     * @param array $ref_dest  ['tbl_actual' => tbl_name, 'col_actual' => col_name]
     */
    private function createInternalTbl(array $ref_src, stdClass $ref_dest) :void
    {
        $tbl_name = $this->generateUniqName(
            'it_' . Str::fnv132($ref_src['tbl_actual'] . $ref_dest->tbl_name),
            INTERNAL_TBLS,
            'tbl_name'
        );
        $db_con = $this->open($this->proj_id);
        $db_con->insertGetId(INTERNAL_TBLS, [
            'tbl_name' => $tbl_name
            , 'ref_src_col_id' => $ref_src['col_id']
            , 'ref_dest_col_id' => $ref_dest->col_id
        ]);
        $db_con->query("create table {$tbl_name} (" . implode(',', [
            'it_id integer primary key'
            , "src_id integer not null references {$ref_src['col_name']}(col_id) on delete cascade"
            , "dest_id integer not null references {$ref_dest->col_actual}(col_id) on delete cascade"
        ]) . ');');
    }

    /**
     * ハッシュ化したユーザー入力列名にprefixと表内で一意なsuffixを付けて、重複しない実名を返す
     *
     * @param string $tbl_name
     * @return string
     */
    private function generateUniqName(string $prefix, string $target_tbl, string $target_col) :string
    {
        $actual_name = uniqid($prefix);
        $stmt = $this->open($this->proj_id)
            ->query("
                select exists(
                    select *
                    from {$target_tbl}
                    where {$target_col} = ?
                ) as judge
            ", [$actual_name]);
        while (boolval($stmt->fetch()->judge)) {
            $actual_name = uniqid($prefix, true);
            $stmt->execute([$actual_name]);
        }
        return $actual_name;
    }
}