<?php
/**
 *
 * @author amderbar
 *
 */
class TableEditorProcess extends Process
{
    /**
     *
     * @param int $proj_id
     * @param int $tbl_id
     * @return array
     */
    public function listColumns(int $proj_id, int $tbl_id) :array
    {
        try {
            return $this->open($proj_id)->findByKeyWithJoin(
                [ SYSTEM_COL, NUM_SETTINGS => [ [NUM_SETTINGS => 'col_id', SYSTEM_COL => 'id'] ] ],
                [ SYSTEM_COL => ['tbl_id' => $tbl_id ] ],
                null, [ SYSTEM_COL.'.id' ]
            );
        } catch ( PDOException | DBManageExeption $e ) {
            $this->handleException( $e );
        }
    }


    /**
     * @param int $proj_id
     * @param int $col_id
     * @param bool $is_actual
     * @return array
     */
    public function getTblColNameFromColid(int $proj_id, int $col_id, ?bool $is_actual = false) :?array
    {
        $result = $this->open($proj_id)->findByKeyWithJoin(
            [ SYSTEM_TBL, SYSTEM_COL => [ [SYSTEM_TBL => 'id', SYSTEM_COL => 'tbl_id'] ] ],
            [ SYSTEM_COL => ['id' => $col_id ] ],
            [ SYSTEM_TBL => ['actual_name as tbl_name'], SYSTEM_COL => ['actual_name as col_name'] ]
        );
        return array_shift( $result );
    }

    /**
     *
     * @return array
     */
    public function getAllTemplates(int $proj_id) :array
    {
        try {
            return $this->open($proj_id)->fetchAll('templates');
        } catch (PDOException | DBManageExeption $e) {
            $this->handleException($e);
        }
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
     * @param int $proj_id
     *            対象DBのID
     * @param string $tbl_name
     *            ユーザーが指定したテーブル名
     * @param array $cols_hash
     *            列定義の情報
     * @param array $constraints_hash
     *            表制約の情報
     * @return int
     */
    public function createUserTable(int $proj_id, string $tbl_name, array $cols_hash, array $constraints_hash = array()): array
    {
        try { // FIXME: トランザクション処理必要
            $db_con = $this->open($proj_id);

            // システムテーブルに表名を登録
            $actual_tbl_name = $this->generateUniqName($proj_id, 'ut_', SYSTEM_TBL, 'actual_name');

            $tbl_id = $db_con->insert( SYSTEM_TBL, [
                    'tbl_name' => $tbl_name,
                    'actual_name' => $actual_tbl_name
            ] );

            // システムテーブルに列情報を登録
            $col_info = $this->registerUserColumns( $proj_id, $tbl_id, $cols_hash, $constraints_hash );

            // SQL前駆体の作成
            $form_type = Arr::combine(
                $this->open(ROOT_DB_ID)->fetchAll('form_master'),
                'id',
                'type'
            );
            $tdo = new TableDefineObject( $actual_tbl_name );
            foreach ($this->listColumns($proj_id, $tbl_id) as $col) {
                if ($col['actual_name'] == 'id') {
                    continue;
                }
                $col_constraints = array();
                if ( isset( $col['ref_col'] ) ) {
                    $tbl_col = $this->getTblColNameFromColid( $proj_id, $col['ref_col'], true );
                    if ( $col['multiple'] ) {
                        // 複数選択列の場合 => 中間表を作成し、実際には列を作らない
                        $ref_src = array(
                                'tbl_name' => $actual_tbl_name,
                                'col_name' => $col['actual_name']
                        );
                        $this->createInternalTbl($proj_id, $ref_src, $tbl_col);
                        continue;
                    } else {
                        // 単一参照の場合 => 実際の列定義では常に参照先表のid列を外部キーに指定
                        $col_constraints['foreign'] = [
                                'ref' => $tbl_col['tbl_name'].'(id)',
                                'del' => 'null'
                        ];
                    }
                }
                $col_constraints['default'] = ($col['default_val'] === '') ? null : $col['default_val'];
                $col_constraints['not_null'] = $col['not_null'] ?? null;
                $col_constraints['uniq'] = $col['uniq'] ?? null;
                $tdo->appendColumn(
                    $col['actual_name'],
                    FORM_TO_DATA[ $form_type[ $col['form_type'] ] ],
                    $col_constraints
                );
            }

            // テーブル作成
            $db_con->createTable( $tdo );

            // templates table
            $tdo = new TableDefineObject('templates');
            $tdo->appendColumn('proj_id', 'integer', [
                    'foreign' => [
                            'ref' => 'projects(id)',
                            'del' => 'null'
                    ]
            ]);
            $tdo->appendColumn('tmpl_name', 'text', [
                    'not_null' => true
            ]);
            $tdo->appendConstraint('uniq', [
                    'proj_id',
                    'tmpl_name'
            ]);
            $db_con->createTable($tdo, true);

            return [ 'tbl_id' => $tbl_id, 'col_id' => $col_info ];

        } catch ( PDOException | DBManageExeption $e ) {
            $this->handleException( $e );
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
    private function registerUserColumns(int $proj_id, int $tbl_id, array $cols_hash, array $constraints_hash = array()): array
    {
        $form_id = Arr::combine(
            $this->open(ROOT_DB_ID)->fetchAll('form_master'),
            'type',
            'id'
        );

        $db_con = $this->open($proj_id);

        $col_id_arr = array();
        $col_id_arr[] = $db_con->insert( SYSTEM_COL, [
                'tbl_id' => $tbl_id,
                'col_name' => 'No.',
                'actual_name' => 'id',
                'form_type' => $form_id['hidden'],
                'multiple' => false
        ] );

        foreach ( $cols_hash as $def_hash ) {

            $actual_name = $this->generateUniqName($proj_id, 'uc_', SYSTEM_COL, 'actual_name');
            $col_id = $db_con->insert( SYSTEM_COL, [
                    'tbl_id' => $tbl_id,
                    'col_name' => $def_hash['col_name'],
                    'actual_name' => $actual_name,
                    'form_type' => $form_id[ $def_hash['form_type'] ],
                    'multiple' => boolval( $def_hash['multiple'] ?? false )
            ] );

            if ( isset( $def_hash['step'] ) && isset( $def_hash['max'] ) && isset( $def_hash['min'] ) ) {
                $db_con->insert( NUM_SETTINGS, [
                        'col_id' => $col_id,
                        'step' => $def_hash['step'],
                        'max' => $def_hash['max'],
                        'min' => $def_hash['min']
                ] );
            }

            $col_id_arr[] = $col_id;
        }

        foreach ( $cols_hash as $i => $def_hash ) {

            switch ( $def_hash['form_type'] ) {
                case 'listext':
                case 'numlist':
                case 'select':
                case 'radio':
                case 'multicheck':
                    if ( !isset( $def_hash['ref_dist'] ) ) {
                        // 追加で新しく表を作ってそれを参照する場合
                        $tbl_name = '[マスタ]'.$def_hash['col_name'];
                        $def_cols = [1 => [
                            'col_name' => $def_hash['col_name'],
                            'form_type' => 'text',
                            'uniq' => true,
                            'not_null' => true
                     ]];
                        $mstr_tbl = $this->createUserTable($proj_id, $tbl_name, $def_cols);
                        $def_hash['ref_dist'] = $mstr_tbl['col_id'][ 1 ];

                    } else if ($def_hash['ref_dist'] < 1) {
                        // 自表の列を参照する場合
                        $def_hash['ref_dist'] = $col_id_arr[ -$def_hash['ref_dist'] ];
                    }

                    $db_con->update( SYSTEM_COL,
                        ['ref_col' => $def_hash['ref_dist'] ],
                        ['id' => $col_id_arr[$i] ]
                    );
                    break;
            }

        }

        return $col_id_arr;
    }

    /**
     * @param int   $proj_id
     * @param array $ref_src   ['tbl_name' => tbl_name, 'col_name' => col_name]
     * @param array $ref_dest  ['tbl_name' => tbl_name, 'col_name' => col_name]
     */
    private function createInternalTbl( int $proj_id, array $ref_src, array $ref_dest ) :void
    {
        $tdo = new TableDefineObject( $ref_src['col_name'].'_'.$ref_src['tbl_name'] );
        $col_constraints = array();
        $col_constraints['foreign'] = [
                'ref' => $ref_src['tbl_name'].'('.'id'.')',
                'del' => 'cascade'
        ];
        $col_constraints['not_null'] = true;
        $tdo->appendColumn( $ref_src['col_name'].'_id', 'integer', $col_constraints );

        $col_constraints = array();
        $col_constraints['foreign'] = [
                'ref' => $ref_dest['tbl_name'].'('.'id'.')',
                'del' => 'cascade'
        ];
        $col_constraints['not_null'] = true;
        $tdo->appendColumn( $ref_dest['col_name'].'_id', 'integer', $col_constraints );
        $this->open($proj_id)->createTable( $tdo );
    }

    /**
     * ハッシュ化したユーザー入力列名にprefixと表内で一意なsuffixを付けて、重複しない実名を返す
     *
     * @param string $tbl_name
     * @return string
     */
    private function generateUniqName(int $proj_id, string $prefix, string $target_tbl, string $target_col) :string
    {
        $db_con = $this->open($proj_id);
        $actual_name = uniqid($prefix);
        while ($db_con->isExists($target_tbl, [$target_col => $actual_name])) {
            $actual_name = uniqid($prefix, true);
        }
        return $actual_name;
    }
}