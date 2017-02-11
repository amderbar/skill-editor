<?php

require_once($_SERVER['DOCUMENT_ROOT'].'/skill_editor/common.inc');
require_once($_SERVER['DOCUMENT_ROOT'].'/skill_editor/models/data_transfer_objects.inc');


/**
 *
 * TODO: DBMS変更の際も修正を最小限にするような設計
 *
 * @author amderbar
 *
 */
class SQLiteHandler {


    /** */
    private $pdo = null;


    /** */
    static public $DATA_TYPES = array(
        'TEXT' => '文字列',
        'INTEGER' => '整数値',
        'REAL' => '実数値',
        'DATETIME' => '日付時刻',
        'BOOLEAN' => '真偽値',
        'NUMERIC' => '数値',
        'BLOB' => 'その他なんでも'
    );


    /** */
    static public $ON_DELETE = array(
        'NULL' => 'SET NULL',
        'CASCADE' => 'CASCADE'
    );



    /**
    *
    */
    public function __construct( string $db_name ) {
        $this->connect( $db_name );
    }



    /**
    *
    */
    public function connect( string $db_name ) {
        $dsn = 'sqlite:' . $db_name;
        try {
            $this->pdo = new PDO($dsn);
            $this->pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
            $this->pdo->setAttribute(PDO::ATTR_DEFAULT_FETCH_MODE, PDO::FETCH_ASSOC);
            $this->pdo->exec('PRAGMA foreign_keys = true;');
        } catch (PDOException $e){
            die('Connection failed:'. $e->getMessage());
        }
    }



    /**
     * @param string $table
     * @param int $id
     * @param array $target_col
     * @return array
     */
    public function load( string $table, int $id, ?array $target_col = [] ) :array {
        // FIXME: $tableと$columnのエスケープ処理が必要
        if ( empty($target_col) ) {
            $target_col = $this->getColumnList( $table );
        }
        $sql = 'SELECT '.implode(',', $target_col).' FROM '.$table.' WHERE id = ?;';
        $stmt = $this->prepareExecute( $sql, [$id] );
        return $stmt->fetch();
    }



    /**
     * @param string $table
     * @param array $condition
     * @return bool
     */
    public function isExists(string $table, array $condition) :bool {
        // FIXME: $tableと$columnのエスケープ処理が必要
        $key_cols = array();
        $key_vals = array();
        foreach ($condition as $col => $val) {
            $key_cols[] = $col.'= ?';
            $key_vals[] = $val;
        }
        $sql = 'SELECT EXISTS( SELECT id FROM '.$table.'WHERE '.implode(' AND ', $key_cols).') AS judge;';

        $stmt = $this->prepareExecute( $sql, $key_vals );
        $result = $stmt->fetch();
        return boolval($result['judge']);
    }



    /**
     *
     * @param string $table
     * @param array $target_col
     * @param array $order_by
     * @return array
     */
    public function fetchAll( string $table, ?array $target_col = [], ?array $order_by = [] ) :array {
        // FIXME: $tableと$columnのエスケープ処理が必要
        if ( empty( $target_col ) ) {
            $target_col = $this->getColumnList( $table );
        }
        $sql = 'SELECT '.implode(',', $target_col).' FROM '.$table;
        if ( !empty( $order_by ) ) {
            $sql .= ' ORDER BY '.implode(', ', $order_by);
        }

        $stmt = $this->prepareExecute( $sql.';' );
        return $stmt->fetchAll();
    }



    /**
     *
     * @param string $table
     * @param array $condition [colmun_name => value] format array
     * @param array $target_col
     * @param array $order_by
     * @return array
     */
    public function findByKey( string $table, array $condition, ?array $target_col = [], ?array $order_by = [] ) :array {
        // FIXME: $tableと$columnのエスケープ処理が必要
        if ( empty( $target_col ) ) {
            $target_col = $this->getColumnList( $table );
        }
        $key_cols = array();
        $key_vals = array();
        foreach ($condition as $col => $val) {
            $key_cols[] = $col.'= ?';
            $key_vals[] = $val;
        }
        $sql = 'SELECT '.implode(',', $target_col).' FROM '.$table.' WHERE '.implode(' AND ', $key_cols);
        if ( !empty( $order_by ) ) {
            $sql .= ' ORDER BY '.implode(', ', $order_by);
        }

        $stmt = $this->prepareExecute( $sql.';', $key_vals );
        return $stmt->fetchAll();
    }



    /**
     *
     * @param array $join_tbls  [ tbl_name, tbl_name => [ [tbl_name => col_name, tbl_name => col_name], ... ], ... ]
     * @param array $condition  [ tbl_name => [col_name => value], ... ]
     * @param array $target_col [ tbl_name => [col_name, ... ], ... ]
     * @param array $order_by   [ tbl_name.col_name, tbl_name.col_name, ... ]
     * @return array
     */
    public function findByKeyWithJoin( array $join_tbls, array $condition, ?array $target_col = [], ?array $order_by = []  ) :array {
        // FIXME: $tableと$columnのエスケープ処理が必要
        $table = array_shift( $join_tbls );

        if ( empty( $target_col ) ) {
            $target_col = array();
            foreach ( array_keys( $join_tbls ) as $tbl_name ) {
                $target_col[ $tbl_name ] = $this->getColumnList( $tbl_name );
            }
            $target_col[ $table ] = $this->getColumnList( $table );
        }
        $target = implodeAssoc( '.', $target_col );

        $table = array( $table );
        foreach ( $join_tbls as $tbl_name => $join_condisions ) {
            $join_on = array();
            foreach ( $join_condisions as $eq_pair ) {
                $on_equation = '';
                foreach ( $eq_pair as $on_tbl => $on_col ) {
                    $on_equation .= ( $on_equation ) ? ' = ' : '';
                    $on_equation .= $on_tbl.'.'.$on_col;
                }
                $join_on[] = $on_equation;
            }
            $table[] = $tbl_name.' ON '.implode(' AND ', $join_on);
        }

        $key_cols = array();
        $cond_vals = array();
        foreach ( $condition as $tbl_name => $col_val ) {
            foreach ( $col_val as $col_name => $value ) {
                $key_cols[] = $tbl_name.'.'.$col_name.'= ?';
                $cond_vals[] = $value;
            }
        }

        $sql = 'SELECT '.implode(','     , $target)
                .' FROM ' .implode(' LEFT JOIN ', $table)
                .' WHERE '.implode(' AND ' , $key_cols);
        if ( !empty( $order_by ) ) {
            $sql .= ' ORDER BY '.implode(', ', $order_by);
        }

        $stmt = $this->prepareExecute( $sql.';', $cond_vals );
        return $stmt->fetchAll();
    }



    /**
     * @param DataTransfarObject $dto
     * @param bool $if_redundant
     * @throws Ambigous <PDOException, DBManageExeption>
     * @return int
     */
    public function insert( DataTransfarObject $dto, ?bool $if_redundant = false) :int {
        try {
            $key_cols = array();
            $place_holders = array();
            $key_vals = array();
            foreach ( $dto->getParms() as $col_name => $value ) {
                $key_cols[] = $col_name;
                $place_holders[] = '?';
                $key_vals[] = $value;
            }
            $sql = 'INSERT INTO '.$dto->getTableName();
            $sql .= '('.implode(',', $key_cols).') VALUES ('.implode(',', $place_holders).');';

            $stmt = $this->prepareExecute( $sql, $key_vals );
            return $this->pdo->lastInsertId();

        } catch ( PDOException $e) {
            if ( $if_redundant && ( $e->getCode() == 23000 ) ) { // 19 UNIQUE constraint failed
                $err_arr = explode( ' ', $e->getMessage() );
                $colname = explode( '.', array_pop( $err_arr ) );
                $colname = array_pop( $colname );
                $parms = $dto->getParms();
                $row = $this->findByKey(
                    $dto->getTableName(), [ $colname => $parms[ $colname ] ]);
                return $row[0]['id'];
            }
            throw $e;
        }
    }



    /**
     * @param DataTransfarObject $dto
     * @param array $conditions
     */
    public function update( DataTransfarObject $dto, ?array $conditions = [] ) :void {
        $key_cols = array();
        $key_vals = array();
        foreach ( $dto->getParms() as $col_name => $value ) {
            $key_cols[] = $col_name.' = ?';
            $key_vals[] = $value;
        }

        $cond_cols = array();
        foreach ( $conditions as $col_name => $value ) {
            $cond_cols[] = $col_name.' = ?';
            $key_vals[] = $value;
        }

        $sql = 'UPDATE '.$dto->getTableName();
        $sql .= ' SET '.implode( ',', $key_cols );

        if ( !empty( $cond_cols ) ) {
            $sql .= ' WHERE '.implode(' AND ', $cond_cols);
        }
        $stmt = $this->prepareExecute( $sql.';', $key_vals );
        return;
    }



    /**
     * @param string $table
     * @param array $condition
     */
    public function delete( string $table, ?array $condition = [] ) :void {
        $key_cols = array();
        $key_vals = array();
        foreach ($condition as $col => $val) {
            $key_cols[] = $col.'= ?';
            $key_vals[] = $val;
        }
        $sql = 'DELETE FROM '.$table.' WHERE '.implode(' AND ', $key_cols).';';

        $stmt = $this->prepareExecute( $sql, $key_vals );
        return;
    }



    /**
     * @param TableDefineObject $tdo
     * @param bool $if_not_exist
     * @throws DBManageExeption
     */
    public function createTable(TableDefineObject $tdo, ?bool $if_not_exist = false) :void {
        $defs = array();
        while ( $col = $tdo->fetchColumn() ) {
            if( !isset( self::$DATA_TYPES[ strtoupper( $col['type'] ) ] ) ) {
                throw new DBManageExeption( "Inviled data type specified :".$col['type'], 1 );
            }
            $sql = $col['name'].' '.$col['type'];
            // TODO:Default値のSQLエスケープはどうすれば？
            if (isset($col['default'])) {
                $sql .= ' DEFAULT ';
                if ($col['default'] === false) {
                    $sql .= '"false"';
                } else {
                    $sql .= '"'.$col['default'].'"';
                }
            }
            $sql .= ($col['primery'])   ? ' PRIMARY KEY'   : '';
            $sql .= ($col['uniq'])      ? ' UNIQUE'        : '';
            $sql .= ($col['not_null'])  ? ' NOT NULL'      : '';
            $sql .= ($col['increment']) ? ' AUTOINCREMENT' : '';
            if ( $col['foreign'] ) {
                $sql .= ' REFERENCES '.$col['foreign']['ref'];
                if ( isset( self::$ON_DELETE[ strtoupper( $col['foreign']['del'] ) ] ) ) {
                    $sql .= ' on DELETE '.self::$ON_DELETE[ strtoupper( $col['foreign']['del'] ) ];
                } else {
                   throw new DBManageExeption( "Inviled SQL keyword :".$col['foreign']['del'], 1 );
                }
            }
            $defs[] = $sql;
        }
        while ( $con = $tdo->fetchConstraint() ) {
            $sql = '';
            switch ( strtoupper( $con['type'] ) ) {
                case 'UNIQ':
                    $sql .= ' UNIQUE('.implode(',', $con['targets']).')';
                    break;
                case 'FOREIGN':
                    $sql .= ' FOREIGN KEY '.implode(',', $con['targets']);
                    $sql .= ' REFERENCES '.$con['references']['tbl'].'('
                                .implode(',', $con['references'][cols]).')';
                    break;
            }
            $defs[] = $sql;
        }
        $sql = 'CREATE TABLE ';
        $sql .= ($if_not_exist) ? 'IF NOT EXISTS ' : '';
        $sql .= $tdo->tableName().' (';
        $sql .= implode(',', $defs);
        $sql .= ');';

        $stmt = $this->prepareExecute( $sql );
        return;
    }



    /**
     * @param string $sql
     * @param array $parms
     * @return array
     */
    public function execSQL( string $sql, ?array $parms = [] ) :array {
        $stmt = $this->prepareExecute( $sql, $parms);
        return $stmt->fetchAll();
    }



    /**
     * @param string $table
     * @return array
     */
    private function getColumnList( string $table ) :array {
        static $cache;
        if ( !isset( $cache ) ) {
            $cache = array();
        }
        if ( !isset( $cache[ $table ] ) ) {
            // 一度検索したテーブルの列名リストはキャッシュに保存
            $stmt = $this->prepareExecute( 'PRAGMA table_info('.$table.')' );
            $cache[ $table ] = array_column($stmt->fetchAll(), 'name');
        }
        if ( count( $cache ) > 5 ) {
            // キャッシュサイズの調整
            // 一定以上の長さであれば、先頭を削除
            array_shift( $cache );
        }
        return $cache[ $table ];
    }



    /**
     * @param string $sql
     * @param array $parms
     * @throws DBManageExeption
     * @throws Ambigous <PDOException, DBManageExeption>
     * @return array
     */
    private function prepareExecute( string $sql, ?array $parms = null ) :PDOStatement {
        try {
            $stmt = $this->pdo->prepare( $sql );
            if ( $stmt->execute( $parms ) ) {
                return $stmt;
            } else {
                throw new DBManageExeption( 'Execute Prepared statement Failed.' );
            }
        } catch ( PDOException | DBManageExeption $e ) {
            $e->queryString = $sql ?? null;
            $e->bindedParam = $parms ?? null;
            throw $e;
        }
    }



}
