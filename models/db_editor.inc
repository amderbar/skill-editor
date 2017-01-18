<?php
/**
 * session check.
 */
require_once ($_SERVER['DOCUMENT_ROOT'] . '/skill_editor/common.inc');

require_once (full_path( 'models/db_manager.inc' ));
define( 'ROOT_DB_ID', 0 );
define( 'ROOT_DB', 'resources/system_admin.db' );
define( 'SYSTEM_TBL', 's_admin_tbl' );
define( 'SYSTEM_COL', 's_admin_col' );
define( 'NUM_SETTINGS', 's_num_settings' );
define( 'FORM_TO_DATA', [
        'color' => 'TEXT',
        'text' => 'TEXT',
        'textarea' => 'TEXT',
        'tel' => 'TEXT',
        'url' => 'TEXT',
        'email' => 'TEXT',
        'password' => 'TEXT',
        'datetime' => 'TEXT',
        'date' => 'DATETIME',
        'month' => 'DATETIME',
        'week' => 'DATETIME',
        'time' => 'DATETIME',
        'datetime-local' => 'DATETIME',
        'listext' => 'NUMERIC',
        'number' => 'NUMERIC',
        'numlist' => 'NUMERIC',
        'range' => 'NUMERIC',
        'select' => 'NUMERIC',
        'multicheck' => 'NUMERIC',
        'radio' => 'NUMERIC',
        'checkbox' => 'BOOLEAN',
        'file' => 'BLOB',
        'image' => 'BLOB'
] );

/**
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
        $this->open( ROOT_DB_ID );
        // $this->setupSystemDB();
        foreach ($this->con[ROOT_DB_ID]->fetchAll( 'form_master' ) as $row) {
            $this->form_id[ $row['type'] ] = $row['id'];
        }
    }

    /**
     * @param int $proj_id
     * @param bool $is_new
     * @return SQLiteHandler
     */
    public function open(int $proj_id, bool $is_new = false): ?SQLiteHandler {
        $db_file = ($proj_id) ? sprintf( 'resources/proj%03d.db', $proj_id ) : ROOT_DB;
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
        $row = $this->con[ROOT_DB_ID]->findByKey( 'projects',
                [ 'id' => $proj_id ],
                [ 'proj_name']
          );
        return $row[0]['proj_name'] ?? null;
    }

    /**
     * @param int $proj_id
     * @param int $tbl_id
     * @param bool $is_actual
     * @return string
     */
    public function tblName(int $proj_id, int $tbl_id, bool $is_actual = false): ?string {
        $target = ($is_actual) ? 'actual_name' : 'tbl_name';
        $row = $this->con[$proj_id]->findByKey( SYSTEM_TBL, [
                'id' => $tbl_id
        ], [
                $target
        ] );
        return $row[0][$target] ?? null;
    }

    /**
     * @param int $proj_id
     * @param int $tbl_id
     * @param int $col_id
     * @param bool $is_actual
     * @return string
     */
    public function colName(int $proj_id, int $tbl_id, int $col_id, bool $is_actual = false): ?string {
        $target = ($is_actual) ? 'actual_name' : 'col_name';
        $row = $this->con[$proj_id]->findByKey( SYSTEM_COL, [
                'id' => $tbl_id
        ], [
                $target
        ] );
        return $row[0][$target] ?? null;
    }

    /**
     * @return array
     */
    public function listDB(): array {
        try {
            $proj_list = array ();
            foreach ( $this->con[ROOT_DB_ID]->fetchAll( 'projects' ) as $row ) {
                $proj_list[$row['id']] = $row['proj_name'];
            }
            return $proj_list;
        } catch ( PDOException $e ) {
            die( 'PDOException throwen:' . $e->getMessage() );
        }
    }

    /**
     */
    public function registerDB($proj_name) {
        try {
            $proj_id = $this->insertData( ROOT_DB_ID, 'projects', [ 'proj_name' => $proj_name ], true );
            $tmpl_dir = full_path( sprintf( 'resources/templates/proj%03d', $proj_id ), true );
            if (! file_exists( $tmpl_dir )) {
                mkdir( $tmpl_dir, 0666 );
            }
            $db_con = $this->open( $proj_id, true );
            // system_admin tables
            $tdo = new TableDefineObject( SYSTEM_TBL );
            $tdo->appendColumn( 'tbl_name', 'text', [ 'uniq' => true, 'not_null' => true ] );
            $tdo->appendColumn( 'actual_name', 'text', [ 'uniq' => true, 'not_null' => true ] );
            $db_con->createTable( $tdo, true );

            $tdo = new TableDefineObject( SYSTEM_COL );
            $tdo->appendColumn( 'tbl_id', 'integer', [
                    'foreign' => [ 'ref' => SYSTEM_TBL . '(id)', 'del' => 'cascade' ]
              ] );
            $tdo->appendColumn( 'col_name', 'text', [ 'not_null' => true ] );
            $tdo->appendColumn( 'actual_name', 'text', [ 'not_null' => true ] );
            $tdo->appendColumn( 'default_val', 'blob');
            $tdo->appendColumn( 'ref_col', 'integer', [
                    'foreign' => [ 'ref' => SYSTEM_COL . '(id)', 'del' => 'null' ]
              ] );
            $tdo->appendColumn( 'form_type', 'integer', [ 'not_null' => true ] );
            $tdo->appendColumn( 'multiple', 'boolean', [ 'not_null' => true, 'default' => false ] );
            $tdo->appendConstraint( 'uniq', [ 'tbl_id', 'actual_name' ] );
            $db_con->createTable( $tdo, true );

            $tdo = new TableDefineObject( NUM_SETTINGS );
            $tdo->appendColumn( 'col_id', 'integer', [
                    'foreign' => [ 'ref' => SYSTEM_COL . '(id)', 'del' => 'cascade' ]
              ] );
            $tdo->appendColumn( 'step', 'real' );
            $tdo->appendColumn( 'max', 'real' );
            $tdo->appendColumn( 'min', 'real' );

            $db_con->createTable( $tdo, true );
            return $proj_id;
        } catch ( PDOException $e ) {
            error_log(
                    $e->getMessage() . PHP_EOL
                    . $e->queryString . PHP_EOL
                    . $e->getTraceAsString()
                );
            die();
        }
    }

    /**
     */
    public function deleteDB($proj_id) {
        // トランザクション処理必要
        if (isset( $this->con[$proj_id] )) {
            $this->close( $proj_id );
        }
        $this->con[ROOT_DB_ID]->delete( 'projects', 'id', $proj_id );
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
    public function registerTemplate($proj_id, $tmpl_name) {
        try {
            $dto = new class( $proj_id, $tmpl_name ) extends DTO {
                function __construct($proj_id, $tmpl_name) {
                    $this->setParm( ':proj_id', $proj_id );
                    $this->setParm( ':tmpl_name', $tmpl_name );
                }
                public function getInsertSQL() {
                    return 'INSERT INTO templates(proj_id, tmpl_name) VALUES (:proj_id, :tmpl_name);';
                }
            };
            if ($this->con[ROOT_DB_ID]->insert( $dto )) {
                $tmpl_id = $this->con[ROOT_DB_ID]->execSQL( 'select last_insert_rowid() AS id' );
                $tmpl_id = $tmpl_id[0]['id'];
                return $tmpl_id;
            }
            return false;
        } catch ( PDOException $e ) {
            die( 'PDOException throwen:' . $e->getMessage() );
        }
    }

    /**
     */
    public function getAllTemplates() {
        try {
            return $this->con[ROOT_DB_ID]->fetchAll( 'templates' );
        } catch ( PDOException $e ) {
            die( 'PDOException throwen:' . $e->getMessage() );
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
        } catch ( PDOException $e ) {
            die( 'PDOException throwen:' . $e->getMessage() );
        }
    }

    /**
     */
    public function listUsrTables($proj_id) {
        try {
            $table_list = array ();
            foreach ( $this->con[$proj_id]->fetchAll( SYSTEM_TBL, 'id, tbl_name' ) as $row ) {
                $table_list[$row['id']] = $row['tbl_name'];
            }
            return $table_list;
        } catch ( PDOException $e ) {
            die( 'PDOException throwen:' . $e->getMessage() );
        }
    }

    /**
     */
    public function listColumns($proj_id, $tbl_id) {
        try {
            return $this->con[$proj_id]->findByKey( SYSTEM_COL, [ 'tbl_id' => $tbl_id ] );
        } catch ( PDOException $e ) {
            die( 'PDOException throwen:' . $e->getMessage() );
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
    public function addUserTable(int $proj_id, string $tbl_name, array $cols_hash, array $constraints_hash = array()): int {
        try {
            // トランザクション処理必要
            // システムテーブルに表名を登録
            $tbl_id = $this->insertData( $proj_id, SYSTEM_TBL, [
                    'tbl_name' => $tbl_name,
                    'actual_name' => 'u_' . fnv132( $tbl_name )
            ] );
            // システムテーブルに列情報を登録
            $this->registerUserColumn( $proj_id, $tbl_id, $cols_hash, $constraints_hash );

            // SQL前駆体の作成
            $form_type = array_flip($this->form_id);
            $tdo = new TableDefineObject( $this->tblName( $proj_id, $tbl_id, true ) );
            foreach ($this->listColumns($proj_id, $tbl_id) as $col) {
                if ($col['actual_name'] == 'id') {
                    continue;
                }
                if ( isset( $col['ref_col'] ) ) {
                    $ref_tbl_id = $this->con[$proj_id]->load(SYSTEM_COL, $col['ref_col'], ['tbl_id']);
                    $ref_tbl_id = array_shift($ref_tbl_id);
                    $tbl_col = $this->con[$proj_id]->findByKeyWithJoin(
                            [ SYSTEM_TBL, SYSTEM_COL => [ [SYSTEM_TBL => 'id', SYSTEM_COL => 'tbl_id'] ] ],
                            [ SYSTEM_TBL => ['id' => $ref_tbl_id ], SYSTEM_COL => ['id' => $col['ref_col']] ],
                            [ SYSTEM_TBL => ['actual_name as tbl_name'], SYSTEM_COL => ['actual_name as col_name'] ]
                    );
                    $tbl_col = array_shift($tbl_col);
                    $col_constraints['foreign'] = [
                            'ref' => $tbl_col['tbl_name'].'('.$tbl_col['col_name'].')',
                            'del' => 'null'
                    ];
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
            return $tbl_id;
        } catch ( PDOException $e ) {
            $err_str = $e->getMessage() . PHP_EOL;
            $err_str .= $e->queryString . PHP_EOL;
            $err_str .= $e->getTraceAsString();
            error_log($err_str);
            die();
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
     * @return string
     */
    private function registerUserColumn(int $proj_id, int $tbl_id, array $cols_hash, array $constraints_hash = array()): void {

        $col_id_arr = array();
        $col_id_arr[] = $this->insertData( $proj_id, SYSTEM_COL, [
                'tbl_id' => $tbl_id,
                'col_name' => 'rid',
                'actual_name' => 'id',
                'form_type' => '',
                'multiple' => false
       ] );

        // TODO: multiple == true のときの扱い

        foreach ( $cols_hash as $def_hash ) {

            $actual_name = 'c_'.fnv132( $def_hash['col_name'] );
            $col_id = $this->insertData( $proj_id, SYSTEM_COL, [
                'tbl_id' => $tbl_id,
                'col_name' => $def_hash['col_name'],
                'actual_name' => $actual_name,
                'form_type' => $this->form_id[$def_hash['form_type']],
                'multiple' => ( isset( $def_hash['multiple'] ) ) ? ( bool ) $def_hash['multiple'] : false
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

        // TODO: 参照先がマスタ表などの場合の外部参照先の指定
        // 選択肢で表示したいのはデータ列だけど参照はID列にしたい(Form上での選択と実際の指定が異なる)
        // しかし指定した通りの列を参照先としたい場合もある
        // 参照先の表の列数に応じて切り替える？

        foreach ( $cols_hash as $i => $def_hash ) {

            switch ($def_hash['form_type']) {
                case 'listext':
                case 'numlist':
                case 'select':
                case 'radio':
                case 'multicheck':
                    if (isset( $def_hash['ref_dist'] )) { // 自表または既存表の列を参照する場合
                        if ($def_hash['ref_dist'] < 1) {
                            $def_hash['ref_dist'] = $col_id_arr[ -$def_hash['ref_dist'] ];
                        }
                        $this->updateData( $proj_id, SYSTEM_COL,
                                ['ref_col' => $def_hash['ref_dist'] ],
                                ['id' => $col_id_arr[$i] ]
                        );
                    } else { // 追加で新しく表を作ってそれを参照する場合
                        // TODO: 新しく表を作る
                    }
                    break;
            }

        }
    }

    /**
     * @param int $proj_id
     * @param int $tbl_id
     * @return array
     */
    public function listData(int $proj_id, int $tbl_id): array {
        try {
            $tmp_table = $this->con[$proj_id]->fetchAll( $this->tblName( $proj_id, $tbl_id, true ) );
            $list_data = array ();
            $col_names = array ();
            foreach ( $tmp_table as $row ) {
                $tmp_row = array ();
                foreach ( $row as $col => $value ) {
                    if (! isset( $col_names[$col] )) {
                        $tmp = $this->con[$proj_id]->findByKey( SYSTEM_COL, [
                                'actual_name' => $col
                        ], [
                                'col_name'
                        ] );
                        $col_names[$col] = $tmp[0]['col_name'];
                    }
                    $tmp_row[$col_names[$col]] = $value;
                }
                $list_data[] = $tmp_row;
            }
            $tmp_row = array ();
            foreach ( $this->listColumns( $proj_id, $tbl_id ) as $col ) {
                $tmp_row[$col['col_name']] = null;
            }
            $list_data[] = $tmp_row;
            return $list_data;
        } catch ( PDOException $e ) {
            die( 'PDOException throwen:' . $e->getMessage() );
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
        $dto = new class( $table, $parms ) extends DTO {
            private $table = '';
            function __construct($table, $parms) {
                $this->table = $table;
                foreach ( $parms as $colname => $value ) {
                    $this->setParm( ':' . $colname, $value );
                }
            }
            public function getTableName() {
                return $this->table;
            }
        };
        try {
            // トランザクション処理必要
            return $this->con[$proj_id]->insert( $dto, $if_redundant );
        } catch ( PDOException $e ) {
            error_log( $e->getMessage() );
            die();
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
        $dto = new class( $table, $parms ) extends DTO {
            private $table = '';
            function __construct($table, $parms) {
                $this->table = $table;
                foreach ( $parms as $colname => $value ) {
                    $this->setParm( ':' . $colname, $value );
                }
            }
            public function getTableName() {
                return $this->table;
            }
        };
        try {
            // トランザクション処理必要
            return $this->con[$proj_id]->update( $dto, $conditions );
        } catch ( PDOException $e ) {
            error_log( $e->getMessage() );
            die();
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
                $dto = new class( $key, $colname, $parms[$key] ) extends DTO {
                    private $table = '';
                    private $colname = '';
                    function __construct($key, $colname, $parm) {
                        $this->table = $key . 's';
                        $this->colname = $colname;
                        $this->setParm( ':' . $colname, $parm );
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
            $dto = new class( $parms ) extends DTO {
                private $table = 'skills';
                private $colname = '';
                function __construct($parm) {
                    $this->colname = array_keys( $parm );
                    foreach ( $parm as $key => $value ) {
                        $this->setParm( ':' . $key, $value );
                    }
                }
                public function getInsertSQL() {
                    $columns = implode( ',', $this->colname );
                    $places = array ();
                    foreach ( $this->colname as $column ) {
                        $places[] = ':' . $column;
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
                    $dto = new class( $condition ) extends DTO {
                        function __construct($parm) {
                            $this->setParm( ':condition', $parm );
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
                    $dto = new class( $pair ) extends DTO {
                        function __construct($parm) {
                            $this->setParm( ':skill_id', $parm[0] );
                            $this->setParm( ':condition_id', $parm[1] );
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
    private function setupSystemDB() {
        try {
            // project table
            $tdo = new TableDefineObject( 'projects' );
            $tdo->appendColumn( 'proj_name', 'text', [
                    'uniq' => true,
                    'not_null' => true
            ] );
            $this->con[ROOT_DB_ID]->createTable( $tdo, true );
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
            $this->con[ROOT_DB_ID]->createTable( $tdo, true );
            // users table
            $tdo = new TableDefineObject( 'users' );
            $tdo->appendColumn( 'name', 'text', [
                    'not_null' => true
            ] );
            $tdo->appendColumn( 'passwd', 'text', [
                    'not_null' => true
            ] );
            $this->con[ROOT_DB_ID]->createTable( $tdo, true );
            // form_type master table
            $tdo = new TableDefineObject( 'form_master' );
            $tdo->appendColumn( 'type', 'text', [
                    'uniq' => true,
                    'not_null' => true
            ] );
            $this->con[ROOT_DB_ID]->createTable( $tdo, true );
            foreach ( HTMLHandler::$FROM_TYPES as $type => $txt ) {
                $this->insertData( ROOT_DB_ID, 'form_master', [
                        'type' => $type
                ], true );
            }
        } catch ( PDOException $e ) {
            die( 'PDOException throwen:' . $e->getMessage() );
        }
    }

    /**
     */
    public static function getDataTypeList() {
        return SQLiteHandler::$DATA_TYPES;
    }
}

?>