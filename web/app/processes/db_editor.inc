<?php
/**
 *
 * TODO: システムDBを編集するものとユーザーDBを編集するものとでクラスを分けたい
 *
 * @author amderbar
 *
 */
class DBEditor {
    /** */
    private $con = array ();

    /** */
    private $form_id = array ();

    /**
     *
     */
    public function __construct() {
        $db_con = $this->open( ROOT_DB_ID ) ?? $this->setupSystemDB();
        foreach ($db_con->fetchAll( 'form_master' ) as $row) {
            $this->form_id[ $row['type'] ] = $row['id'];
        }
    }

    /**
     * @param int $proj_id
     * @param bool $is_new
     * @return SQLiteHandler
     */
    public function open(int $proj_id, bool $is_new = false): ?SQLiteHandler {
        $db_file = ($proj_id) ? sprintf( 'app/resources/proj%03d.db', $proj_id ) : ROOT_DB;
        $db_file = full_path( $db_file, $is_new );
        if (! $db_file) {
            return null;
        } else if (! isset( $this->con[$proj_id] )) {
            $this->con[$proj_id] = new SQLiteHandler( $db_file );
        }
        return $this->con[$proj_id];
    }

    /**
     * @param int $proj_id
     */
    public function close(int $proj_id): void {
        $this->con[$proj_id] = null;
        unset( $this->con[$proj_id] );
    }

    /**
     * @param int $proj_id
     * @return string
     */
    public function projName(int $proj_id): ?string {
        $row = $this->con[ROOT_DB_ID]->load( 'projects', $proj_id , [ 'proj_name'] );
        return $row['proj_name'] ?? null;
    }

    /**
     * @param int $proj_id
     * @param int $tbl_id
     * @param bool $is_actual
     * @return string
     */
    public function tblName(int $proj_id, int $tbl_id, bool $is_actual = false): ?string {
        $target = ($is_actual) ? 'actual_name' : 'tbl_name';
        $row = $this->con[$proj_id]->load( SYSTEM_TBL, $tbl_id, [ $target ] );
        return $row[$target] ?? null;
    }

    /**
     * @param int $proj_id
     * @param int $col_id
     * @param bool $is_actual
     * @return string
     */
    public function colName(int $proj_id, int $col_id, ?bool $is_actual = false): ?string {
        $target = ($is_actual) ? 'actual_name' : 'col_name';
        $row = $this->con[$proj_id]->load( SYSTEM_COL, $col_id, [ $target ] );
        return $row[$target] ?? null;
    }

    /**
     * @param int $proj_id
     * @param int $col_id
     * @param bool $is_actual
     * @return array
     */
    public function getTblColNameFromColid( int $proj_id, int $col_id, ?bool $is_actual = false ) :?array {
        $result = $this->con[ $proj_id ]->findByKeyWithJoin(
             [ SYSTEM_TBL, SYSTEM_COL => [ [SYSTEM_TBL => 'id', SYSTEM_COL => 'tbl_id'] ] ],
             [ SYSTEM_COL => ['id' => $col_id ] ],
             [ SYSTEM_TBL => ['actual_name as tbl_name'], SYSTEM_COL => ['actual_name as col_name'] ]
         );
        return array_shift( $result );
    }

    /**
     * @return array
     */
    public function listDB(): array {
        try {
            $proj_list = array ();
            foreach ( $this->con[ROOT_DB_ID]->fetchAll( 'projects', null, ['id'] ) as $row ) {
                $proj_list[$row['id']] = $row['proj_name'];
            }
            return $proj_list;
        } catch ( PDOException | DBManageExeption $e ) {
            $this->handleException( $e );
        }
    }


    /**
     */
    public function deleteDB($proj_id) {
        // FIXME: トランザクション処理必要
        if (isset( $this->con[$proj_id] )) {
            $this->close( $proj_id );
        }
        $this->con[ROOT_DB_ID]->delete( 'projects', [ 'id' => $proj_id ] );
        if ($tmpl_dir = full_path( sprintf( 'resources/templates/proj%03d', $proj_id ) )) {
            if ($handle = opendir( $tmpl_dir )) {
                while ( false !== ($tmpl = readdir( $handle )) ) {
                    if ($tmpl != "." && $tmpl != "..") {
                        unlink( $tmpl );
                    }
                }
                rmdir( $tmpl_dir );
            }
            closedir( $handle );
        }
        if ($db_file = full_path( sprintf( 'resources/proj%03d.db', $proj_id ) )) {
            unlink( $db_file );
        }
    }

    /**
     */
    public function getAllTemplates() {
        try {
            return $this->con[ROOT_DB_ID]->fetchAll( 'templates' );
        } catch ( PDOException | DBManageExeption $e ) {
            $this->handleException( $e );
        }
    }

    /**
     */
    public function getTemplates($proj_id) {
        try {
            $tmpl_list = array ();
            $tmpls = $this->con[ROOT_DB_ID]->findByKey( 'templates', [
                    'proj_id' => $proj_id
            ] );
            foreach ( $tmpls as $row ) {
                $tmpl_list[$row['id']] = $row['tmpl_name'];
            }
            return $tmpl_list;
        } catch ( PDOException | DBManageExeption $e ) {
            $this->handleException( $e );
        }
    }

    /**
     */
    public function listUsrTables($proj_id) {
        try {
            $table_list = array ();
            foreach ( $this->con[$proj_id]->fetchAll( SYSTEM_TBL, ['id', 'tbl_name'], ['id'] ) as $row ) {
                $table_list[$row['id']] = $row['tbl_name'];
            }
            return $table_list;
        } catch ( PDOException | DBManageExeption $e ) {
            $this->handleException( $e );
        }
    }

    /**
     */
    public function listColumns($proj_id, $tbl_id) {
        try {
            return $this->con[ $proj_id ]->findByKeyWithJoin(
                [ SYSTEM_COL, NUM_SETTINGS => [ [NUM_SETTINGS => 'col_id', SYSTEM_COL => 'id'] ] ],
                [ SYSTEM_COL => ['tbl_id' => $tbl_id ] ],
                null, [ SYSTEM_COL.'.id' ]
            );
        } catch ( PDOException | DBManageExeption $e ) {
            $this->handleException( $e );
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
    public function addUserTable(int $proj_id, string $tbl_name, array $cols_hash, array $constraints_hash = array()): array {
        try {
            // FIXME: トランザクション処理必要
            // システムテーブルに表名を登録
            $actual_tbl_name = $this->generateActualTblName(  $proj_id, $tbl_name );
            $tbl_id = $this->insertData( $proj_id, SYSTEM_TBL, [
                    'tbl_name' => $tbl_name,
                    'actual_name' => $actual_tbl_name
            ] );
            // システムテーブルに列情報を登録
            $col_info = $this->registerUserColumns( $proj_id, $tbl_id, $cols_hash, $constraints_hash );

            // SQL前駆体の作成
            $form_type = array_flip($this->form_id);
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
            // TODO: 表制約の設定ロジック
            foreach ( $constraints_hash as $def_hash ) {
                // $tdo->appendConstraint( $def_hash );
            }
            // テーブル作成
            $this->con[$proj_id]->createTable( $tdo );
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
    private function registerUserColumns(int $proj_id, int $tbl_id, array $cols_hash, array $constraints_hash = array()): array {

        $col_id_arr = array();
        $col_id_arr[] = $this->insertData( $proj_id, SYSTEM_COL, [
                'tbl_id' => $tbl_id,
                'col_name' => 'No.',
                'actual_name' => 'id',
                'form_type' => $this->form_id['hidden'],
                'multiple' => false
       ] );

        foreach ( $cols_hash as $def_hash ) {

            $actual_name = $this->generateActualColName( $proj_id, $def_hash['col_name'] );
            $col_id = $this->insertData( $proj_id, SYSTEM_COL, [
                'tbl_id' => $tbl_id,
                'col_name' => $def_hash['col_name'],
                'actual_name' => $actual_name,
                'form_type' => $this->form_id[ $def_hash['form_type'] ],
                'multiple' => boolval( $def_hash['multiple'] ?? false )
            ] );

            if ( isset( $def_hash['step'] ) && isset( $def_hash['max'] ) && isset( $def_hash['min'] ) ) {
                $this->insertData( $proj_id, NUM_SETTINGS, [
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
                        $def_cols = array(
                            1 => array(
                                'col_name' => $def_hash['col_name'],
                                'form_type' => 'text',
                                'uniq' => true,
                                'not_null' => true
                        ));
                        $mstr_tbl = $this->addUserTable($proj_id, $tbl_name, $def_cols);
                        $def_hash['ref_dist'] = $mstr_tbl['col_id'][ 1 ];

                    } else if ($def_hash['ref_dist'] < 1) {
                        // 自表の列を参照する場合
                        $def_hash['ref_dist'] = $col_id_arr[ -$def_hash['ref_dist'] ];
                    }

                    $this->updateData( $proj_id, SYSTEM_COL,
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
    private function createInternalTbl( int $proj_id, array $ref_src, array $ref_dest ) :void {
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
        $this->con[$proj_id]->createTable( $tdo );
    }



    /**
     * ハッシュ化したユーザー入力表名にprefixと一意なsuffixを付けて、重複しない実名を返す
     * FIXME: 現状では一意でない実名が返されることもありうる
     * ⇒ 入力表名をハッシュ化するのではなく、それとは無関係にuniqid()を使用してはどうか。
     *
     * @param string $tbl_name
     * @return string
     */
    private function generateActualTblName(int $proj_id, string $tbl_name) :string {
        $max_id = $this->con[$proj_id]->fetchAll( SYSTEM_TBL, ['max(id) as id'] );
        $max_id = current($max_id);
        return 'ut_' . fnv132( $tbl_name ).'_'.($max_id['id'] + 1);
    }

    /**
     * ハッシュ化したユーザー入力列名にprefixと表内で一意なsuffixを付けて、重複しない実名を返す
     *
     * @param string $tbl_name
     * @return string
     */
    private function generateActualColName(int $proj_id, string $col_name) :string {
        static $max_id;
        $max_id = $max_id ?? 0;
        $max_id++;
        return 'uc_' . fnv132( $col_name ).'_'.$max_id;
    }

    /**
     *
     *
     * @param int $proj_id
     * @param int $tbl_id
     * @return array
     */
    public function listData(int $proj_id, int $tbl_id): array {
        try {
            $form_type = array_flip( $this->form_id );
            $col_names = array();
            $col_forms = array();
            foreach ( $this->listColumns( $proj_id, $tbl_id ) as $col ) {
                $actual_name = $col[ 'actual_name' ];
                $col_names[ $actual_name ] = $col[ 'col_name'];
                $col_forms[ $actual_name ] = array(
                        'type'     => $form_type[ $col[ 'form_type'] ],
                        'default'  => $col[ 'default_val'],
                        'multiple' => boolval( $col[ 'multiple'] )
                );
                if ( isset( $col[ 'step'] ) || isset( $col[ 'max'] ) || isset( $col[ 'min'] ) ) {
                    $col_forms[ $actual_name ]['step'] = $col[ 'step'] ?? 1;
                    $col_forms[ $actual_name ]['max']  = $col[ 'max'] ?? null;
                    $col_forms[ $actual_name ]['min']  = $col[ 'min'] ?? null;
                }
                if ( isset( $col[ 'ref_col'] ) ) {
                    $ref_tbl_col = $this->getTblColNameFromColid( $proj_id, $col[ 'ref_col'], true );
                    $col_forms[ $actual_name ]['ref'] = array();
                    $ref_data = $this->con[$proj_id]->fetchAll( $ref_tbl_col['tbl_name'], ['id', $ref_tbl_col['col_name'] ], ['id'] );
                    foreach ( $ref_data as $row) {
                        $col_forms[ $actual_name ]['ref'][ $row['id'] ] = $row[ $ref_tbl_col['col_name'] ];
                    }
                    if ( empty( $col_forms[ $actual_name ]['ref'] ) ) {
                        $col_forms[ $actual_name ]['ref'] = [];
                    }
                }
            }
            $list_data = $this->con[$proj_id]->fetchAll( $this->tblName( $proj_id, $tbl_id, true ), null, ['id'] );
            // 新規データ用行を追加
            $tmp_row = array ();
            foreach ( array_keys( $col_names ) as $col_key ) {
                $tmp_row[ $col_key ] = null;
            }
            $list_data[] = $tmp_row;
            return ['col_name' => $col_names, 'form' => $col_forms, 'data' => $list_data];
        } catch ( PDOException | DBManageExeption $e ) {
            $this->handleException( $e );
        }
    }

    /**
     * @param unknown $proj_id
     * @param unknown $table
     * @param unknown $parms
     * @param string $if_redundant
     * @return unknown
     */
    public function insertData($proj_id, $table, $parms, $if_redundant = false) {
        try {
            // FIXME: トランザクション処理必要
            return $this->con[$proj_id]->insert( $table, $parms, $if_redundant );
        } catch ( PDOException | DBManageExeption $e ) {
            $this->handleException( $e );
        }
    }

    /**
     * @param unknown $proj_id
     * @param unknown $table
     * @param unknown $parms
     * @param unknown $conditions
     * @return unknown
     */
    public function updateData($proj_id, $table, $parms, $conditions = null) {
        $dto = new class( $table, $parms ) extends DataTransfarObject {
            private $table = '';
            function __construct($table, $parms) {
                $this->table = $table;
                foreach ( $parms as $colname => $value ) {
                    $this->setParm( $colname, $value );
                }
            }
            public function getTableName() {
                return $this->table;
            }
        };
        try {
            // FIXME: トランザクション処理必要
            return $this->con[$proj_id]->update( $dto, $conditions );
        } catch ( PDOException | DBManageExeption $e ) {
            $this->handleException( $e );
        }
    }

    /**
     * 現状ではSNTRPG_Skills専用
     */
    public function insertSNRPGData($proj_id, $parms) {
        if ($parms['preconditions'] == '-') {
            $parms['has_preconditions'] = 0;
        } else {
            $parms['has_preconditions'] = 1;
            $preconditions = $parms['preconditions'];
        }
        unset( $parms['preconditions'] );
        $foreign_keys = array (
                'timing' => false,
                'target' => false,
                'renge' => false,
                'icon' => 'file_name'
        );
        try {
            // 外部キーの要素を登録し、rowidに変換
            foreach ( $foreign_keys as $key => $colname ) {
                $colname = $colname ? $colname : $key;
                $dto = new class( $key, $colname, $parms[$key] ) extends DataTransfarObject {
                    private $table = '';
                    private $colname = '';
                    function __construct($key, $colname, $parm) {
                        $this->table = $key . 's';
                        $this->colname = $colname;
                        $this->setParm( $colname, $parm );
                    }
                    public function getInsertSQL() {
                        return 'INSERT INTO ' . $this->table . '(' . $this->colname . ') VALUES (:' . $this->colname . ');';
                    }
                    public function getTableName() {
                        return $this->table;
                    }
                };
                if ($this->con[$proj_id]->insert( $dto )) {
                    $row_id = $this->con[$proj_id]->execSQL( 'select last_insert_rowid() AS id' );
                    $row_id = $row_id[0]['id'];
                } else {
                    $row = $this->con[$proj_id]->findByKey( $dto->getTableName(), $colname, $parms[$key], [
                            'id'
                    ] );
                    $row_id = $row[0]['id'];
                }
                $parms[$key] = $row_id;
            }
            // スキルデータ本体を登録
            $dto = new class( $parms ) extends DataTransfarObject {
                private $table = 'skills';
                private $colname = '';
                function __construct($parm) {
                    $this->colname = array_keys( $parm );
                    foreach ( $parm as $key => $value ) {
                        $this->setParm( $key, $value );
                    }
                }
                public function getInsertSQL() {
                    $columns = implode( ',', $this->colname );
                    $places = array ();
                    foreach ( $this->colname as $column ) {
                        $places[] = $column;
                    }
                    $places = implode( ',', $places );
                    return 'INSERT INTO ' . $this->table . '(' . $columns . ') VALUES (' . $places . ');';
                }
                public function getTableName() {
                    return $this->table;
                }
            };
            if ($this->con[$proj_id]->insert( $dto )) {
                $row_id = $this->con[$proj_id]->execSQL( 'select last_insert_rowid() AS id' );
                $skill_id = $row_id[0]['id'];
            } else {
                $row = $this->con[$proj_id]->findByKey( 'skills', 'name', $parms['name'], [
                        'id'
                ] );
                $skill_id = $row[0]['id'];
            }
            // 習得前提条件が存在する場合
            if ($parms['has_preconditions']) {
                $conditions = explode( ',', $preconditions );
                $preconditions = array ();
                // 個々の条件を登録
                foreach ( $conditions as $condition ) {
                    $dto = new class( $condition ) extends DataTransfarObject {
                        function __construct($parm) {
                            $this->setParm( 'condition', $parm );
                        }
                        public function getInsertSQL() {
                            return 'INSERT INTO conditions(condition) VALUES (:condition);';
                        }
                    };
                    if ($this->con[$proj_id]->insert( $dto )) {
                        $row_id = $this->con[$proj_id]->execSQL( 'select last_insert_rowid() AS id' );
                        $row_id = $row_id[0]['id'];
                    } else {
                        $row = $this->con[$proj_id]->findByKey( 'conditions', [
                                'condition' => $condition
                        ], [
                                'id'
                        ] );
                        $row_id = $row[0]['id'];
                    }
                    $preconditions[] = array (
                            $skill_id,
                            $id
                    );
                }
                // 各条件をスキルデータと関連付け
                foreach ( $preconditions as $pair ) {
                    $dto = new class( $pair ) extends DataTransfarObject {
                        function __construct($parm) {
                            $this->setParm( 'skill_id', $parm[0] );
                            $this->setParm( 'condition_id', $parm[1] );
                        }
                        public function getInsertSQL() {
                            return 'INSERT INTO preconditions(skill_id,condition_id) VALUES (:skill_id,:condition_id);';
                        }
                    };
                    $this->con[$proj_id]->insert( $dto );
                }
            }
            return;
        } catch ( PDOException $e ) {
            die( 'PDOException throwen:' . $e->getMessage() );
        }
    }

    /**
     *
     */
    private function setupSystemDB() :SQLiteHandler {
        try {
            $db_con = $this->con[ ROOT_DB_ID ] ?? $this->open( ROOT_DB_ID, true );
            // project table
            $tdo = new TableDefineObject( 'projects' );
            $tdo->appendColumn( 'proj_name', 'text', [
                    'not_null' => true
            ] );
            $db_con->createTable( $tdo, true );
            // templates table
            $tdo = new TableDefineObject( 'templates' );
            $tdo->appendColumn( 'proj_id', 'integer', [
                    'foreign' => [
                            'ref' => 'projects(id)',
                            'del' => 'null'
                    ]
            ] );
            $tdo->appendColumn( 'tmpl_name', 'text', [
                    'not_null' => true
            ] );
            $tdo->appendConstraint( 'uniq', [
                    'proj_id',
                    'tmpl_name'
            ] );
            $db_con->createTable( $tdo, true );
            // users table
            $tdo = new TableDefineObject( 'users' );
            $tdo->appendColumn( 'name', 'text', [
                    'not_null' => true
            ] );
            $tdo->appendColumn( 'passwd', 'text', [
                    'not_null' => true
            ] );
            $db_con->createTable( $tdo, true );
            // form_type master table
            $tdo = new TableDefineObject( 'form_master' );
            $tdo->appendColumn( 'type', 'text', [
                    'uniq' => true,
                    'not_null' => true
            ] );
            $db_con->createTable( $tdo, true );
            foreach ( HTMLHandler::$FROM_TYPES as $type => $txt ) {
                $this->insertData( ROOT_DB_ID, 'form_master', [
                        'type' => $type
                ], true );
            }
            return $db_con;
        } catch ( PDOException | DBManageExeption $e ) {
            $this->handleException( $e );
        }
    }

     /**
      * @param RuntimeException $e
      * @throws RuntimeException
      */
     private function handleException( RuntimeException $e ) {
         $err_str = get_class( $e ).' throwen:' . $e->getMessage() . PHP_EOL;
         $err_str .= ( $e->queryString ?? '' ) . PHP_EOL;
         $err_str .= var_export( ( $e->bindedParam ?? '' ), true ) . PHP_EOL;
         $err_str .= $e->getTraceAsString();
         error_log( $err_str );
         throw $e;
    }

    /**
     */
    public static function getDataTypeList() {
        return SQLiteHandler::getDataType();
    }
}

?>