<?php
/**
 *
 *
 * @author amderbar
 *
 */

class SQLiteHandler extends PDOHandlar {

    /** */
    static private $DATA_TYPES = array(
            'text' => 'text'
            , 'serial' => 'integer autoincrement'
            , 'integer' => 'integer'
            , 'real' => 'real'
            , 'datetime' => 'datetime'
            , 'boolean' => 'boolean'
            , 'numeric' => 'numeric'
            , 'blob' => 'blob'
    );



    /**
     * {@inheritDoc}
     * @see PDOHandlar::newPDO()
     */
    protected function newPDO( string $db_name ) :PDO {

        return new PDO('sqlite:' . $db_name);
    }



    /**
     * {@inheritDoc}
     * @see PDOHandlar::setupPDO()
     */
    protected function setupPDO( PDO $data_obj ) :PDO {

        $data_obj->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
        $data_obj->setAttribute(PDO::ATTR_DEFAULT_FETCH_MODE, PDO::FETCH_ASSOC);
        $data_obj->exec('PRAGMA foreign_keys = true;');

        return $data_obj;
    }



    /**
     * {@inheritDoc}
     * @see PDOHandlar::getColumnList()
     */
    protected function getColumnList( string $table, string $identifier = null ) :array {
        static $cache;

        if ( !isset( $cache ) ) {
            $cache = array();
        }
        $identifier = $identifier ?? $this->getLastFocus();

        if ( !isset( $cache[ $identifier ] ) ) {
            $cache[ $identifier ] = array();
        }

        if ( !isset( $cache[ $identifier ][ $table ] ) ) {
            $stmt = $this->query( 'PRAGMA table_info('.$table.')', [], $identifier );

            // 一度検索したテーブルの列名リストはキャッシュに保存
            $cache[ $identifier ][ $table ] = array_column( $stmt->fetchAll(), 'name' );
        }

        if ( count( $cache[ $identifier ] ) > 5 ) {
            // キャッシュサイズの調整 一定以上の長さであれば、先頭を削除
            array_shift( $cache[ $identifier ] );
        }
        return $cache[ $identifier ][ $table ];
    }



    /**
     * {@inheritDoc}
     * @see PDOHandlar::getPrimaryColumnName()
     */
    protected function getPrimaryColumnName( string $table, string $identifier = null ) :string {
        return 'id';
    }



    /**
     * {@inheritDoc}
     * @see DataBaseHandlar::getDataTypeDictionary()
     */
    public function getDataTypeDictionary() :array {
        return self::$DATA_TYPES;
    }



    /**
     * @param array $join_tbls  [ tbl_name, tbl_name => [ [tbl_name => col_name, tbl_name => col_name], ... ], ... ]
     * @param array $condition  [ tbl_name => [col_name => value], ... ]
     * @param array $target_col [ tbl_name => [col_name, ... ], ... ]
     * @param array $order_by   [ tbl_name.col_name, tbl_name.col_name, ... ]
     * @return array
     */
    public function findByKeyWithJoin(
        array $join_tbls, array $condition, ?array $target_col = [], ?array $order_by = [], string $identifier = null ) :array {

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

                    $key_cols[] = $tbl_name.'.'.$col_name.'= :'.$tbl_name.'_'.$col_name;
                    $cond_vals[':'.$tbl_name.'_'.$col_name ] = $value;
                }
            }

            $sql = 'SELECT '.implode(','     , $target)
            .' FROM ' .implode(' LEFT JOIN ', $table)
            .' WHERE '.implode(' AND ' , $key_cols);
            if ( !empty( $order_by ) ) {
                $sql .= ' ORDER BY '.implode(', ', $order_by);
            }

            $identifier = $identifier ?? $this->getLastFocus();
            $stmt = $this->query( $sql.';', $cond_vals, $identifier );
            return $stmt->fetchAll();
    }

    public static function getDataType() :array {
        return self::$DATA_TYPES;
    }

}



/**
 * @author amderbar
 *
 */
abstract class PDOHandlar {

    /** */
    static public $ON_DELETE = array(
        'null' => 'set null',
        'cascade' => 'cascade'
    );

    /**
     * @var array
     */
    private $con = array();

    /**
     * @var unknown
     */
    private $focus_histry;

    /**
     * @return string
     */
    public function getLastFocus() :string {
        return $this->focus_histry->last();
    }

    /**
     * constructer
     *
     * @param string $db_name
     * @param string $identifier
     */
    public final function __construct( string $db_name, string $identifier = null ) {

        /**
         * @var PDOHandlar $focus_histry
         */
        $this->focus_histry  = new class {
            /**
             * @var integer
             */
            private $buffer_size = 5;
            /**
             * @var array
             */
            private $histry = array();
            /**
             * @param string $log
             * @return string
             */
            public function rec( string $log ) :string {
                if ( array_push( $this->histry, $log ) > $this->buffer_size ) {
                    array_shift( $this->histry );
                }
                return $log;
            }
            /**
             * @return string
             */
            public function last() :string {
                return end( $this->histry );
            }
            /**
             * @param string $target
             * @return int
             */
            public function clear( string $target = null ) :int {
                $this->histry = array_filter( $this->histry, function ( $log ) use ( $target ) {
                    return $log != ( $target ?? true );
                });
                    return count( $this->histry );
            }
        };

        $this->connect( $db_name, $identifier );
    }


    /**
     * create new connection to db
     *
     * @param string $db_name
     * @param string $identifier
     * @throws PDOException
     * @return string
     */
    public final function connect( string $db_name, string $identifier = null ) :string {
        try {
            $identifier = $identifier ?? uniqid('pdo');
            $this->con[ $identifier ] = $this->setupPDO( $this->newPDO( $db_name ) );

            return ( $this->focus_histry->rec( $identifier ) );

        } catch (PDOException $e){

            error_log('Connection failed:'. $e->getMessage());
            throw $e;
        }
    }


    /**
     * @param string $db_name
     * @return PDO
     */
    abstract protected function newPDO( string $db_name ) :PDO;


    /**
     * @param PDO $data_obj
     * @return PDO
     */
    abstract protected function setupPDO( PDO $data_obj ) :PDO;



    /**
     * disconnect db
     *
     * @param string $identifier
     */
    public function disconnect( string $identifier = null ) :void {

        $identifier = $identifier ?? $this->getLastFocus();
        unset( $this->con[ $identifier ] );
        $this->focus_histry->clear( $identifier );
    }



    /**
     * disconnect all db
     */
    public function disconnectAll() :void {

        $this->con = array();
    }


    abstract protected function getColumnList( string $table, string $identifier = null ) :array ;


    abstract protected function getPrimaryColumnName( string $table, string $identifier = null ) :string ;


    /**
     * @param string $table
     * @param int $id
     * @param array $target_col
     * @return array
     */
    public function load( string $table, $id, ?array $target_col = [], string $identifier = null ) :array {

        // FIXME: $tableと$columnのエスケープ処理が必要
        if ( empty( $target_col ) ) {
            $target_col = $this->getColumnList( $table );

        }
        $identifier = $identifier ?? $this->getLastFocus();
        $id_name = $this->getPrimaryColumnName( $table, $identifier );
        $sql = 'select '.implode( ',', $target_col ).' from '.$table.' where '.$id_name.' = :id;';

        $stmt = $this->prepareExecute( $identifier, $sql, [ ':id' => $id] );
        return $stmt->fetch();
    }



    /**
     * @param string $table
     * @param array $condition
     * @return bool
     */
    public function isExists( string $table, array $condition, string $identifier = null ) :bool {

        // FIXME: $tableと$columnのエスケープ処理が必要
        $key_cols = array();
        $key_vals = array();
        foreach ( $condition as $col => $val ) {

            $key_cols[] = $col.'= :'.$col;
            $key_vals[ ':'.$col_name ] = $val;
        }
        $sql = 'select exists( select id from '.$table.'where '.implode(' and ', $key_cols).') as judge;';

        $identifier = $identifier ?? $this->getLastFocus();
        $stmt = $this->prepareExecute( $identifier, $sql, $key_vals );
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
    public function fetchAll( string $table, ?array $target_col = [], ?array $order_by = [], string $identifier = null ) :array {

        // FIXME: $tableと$columnのエスケープ処理が必要
        if ( empty( $target_col ) ) {
            $target_col = $this->getColumnList( $table );
        }

        $sql = 'select '.implode(',', $target_col).' from '.$table;
        if ( !empty( $order_by ) ) {
            $sql .= ' order by '.implode(', ', $order_by);
        }

        $identifier = $identifier ?? $this->getLastFocus();
        $stmt = $this->prepareExecute( $identifier, $sql.';' );
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
    public function findByKey( string $table, array $condition, ?array $target_col = [], array $order_by = [], string $identifier = null ) :array {

        // FIXME: $tableと$columnのエスケープ処理が必要
        if ( empty( $target_col ) ) {
            $target_col = $this->getColumnList( $table );
        }

        $key_cols = array();
        $key_vals = array();
        foreach ($condition as $col => $val) {

            $key_cols[] = $col.'= :'.$col;
            $key_vals[ ':'.$col ] = $val;
        }

        $sql = 'select '.implode(',', $target_col).' from '.$table.' where '.implode(' and ', $key_cols);
        if ( !empty( $order_by ) ) {
            $sql .= ' order by '.implode(', ', $order_by);
        }

        $identifier = $identifier ?? $this->getLastFocus();
        $stmt = $this->prepareExecute( $identifier, $sql.';', $key_vals );
        return $stmt->fetchAll();
    }



    /**
     * @param string $table
     * @param array $parms
     * @param bool $if_redundant
     * @param string $identifier
     * @throws PDOException
     * @return int
     */
    public function insert( string $table, array $parms, bool $if_redundant = false, string $identifier = null ) :int {
        try {
            $key_cols = array_keys( $parms );
            $place_holders = array();
            $key_vals = array();
            foreach ( $parms as $col_name => $value ) {

                $place_holders[] = ':'.$col_name;
                $key_vals[':'.$col_name ] = $value;
            }

            $sql = 'insert into '.$table;
            $sql .= '('.implode(',', $key_cols).') values ('.implode(',', $place_holders).');';

            $identifier = $identifier ?? $this->getLastFocus();
            $stmt = $this->prepareExecute( $identifier, $sql, $key_vals );

            return $this->con[ $identifier ]->lastInsertId();

        } catch ( PDOException $e) {

            if ( $if_redundant && ( $e->getCode() == 23000 ) ) { // 19 UNIQUE constraint failed
                $err_arr = explode( ' ', $e->getMessage() );
                $colname = explode( '.', array_pop( $err_arr ) );
                $colname = array_pop( $colname );
                $row = $this->findByKey( $table, [ $colname => $parms[ $colname ] ]);
                return $row[0]['id'];
            }

            throw $e;
        }
    }



    /**
     * @param string $table
     * @param array $parms
     * @param array $conditions
     * @param string $identifier
     * @return int
     */
    public function update( string $table, array $parms, array $conditions = [], string $identifier = null ) :int {
        $key_cols = array();
        $key_vals = array();
        foreach ( $parms as $col_name => $value ) {

            $key_cols[] = $col_name.' = :'.$col_name;
            $key_vals[ ':'.$col_name ] = $value;
        }

        $cond_cols = array();
        foreach ( $conditions as $col_name => $value ) {

            $cond_cols[] = $col_name.' = :'.$col_name;
            $key_vals[ ':'.$col_name ] = $value;
        }

        $sql = 'update '.$dto->getTableName();
        $sql .= ' set '.implode( ',', $key_cols );

        if ( !empty( $cond_cols ) ) {
            $sql .= ' where '.implode(' and ', $cond_cols);
        }
        $identifier = $identifier ?? $this->getLastFocus();
        $stmt = $this->prepareExecute( $identifier, $sql.';', $key_vals );

        return $stmt->rowCount();
    }



    /**
     * @param string $table
     * @param array $condition
     * @param string $identifier
     * @return int
     */
    public function delete( string $table, array $condition = [], string $identifier = null ) :int {
        $key_cols = array();
        $key_vals = array();
        foreach ($condition as $col => $val) {
            $key_cols[] = $col.'= :'.$col;
            $key_vals[ ':'.$col ] = $val;
        }
        $sql = 'delete from '.$table.' where '.implode(' and ', $key_cols).';';

        $identifier = $identifier ?? $this->getLastFocus();
        $stmt = $this->prepareExecute( $identifier, $sql, $key_vals );

        return $stmt->rowCount();
    }



    /**
     * @param TableDefineObject $tdo
     * @param bool $if_not_exist
     * @throws DBManageExeption
     */
    public function createTable( TableDefineObject $tdo, bool $if_not_exist = false, string $identifier = null ) :void {

        $data_type_dictionary = $this->getDataTypeDictionary();
        $defs = array();
        while ( $col = $tdo->fetchColumn() ) {

            $dataType = $data_type_dictionary[ $col['type'] ];
            if( isset( $dataType ) ) {
                $sql = $col['name'].' '.$dataType;

            } else {
                throw new DBManageExeption( "Inviled data type specified :".$col['type'], 1 );
            }
            // TODO:Default値のSQLエスケープはどうすれば？
            if (isset($col['default'])) {
                $sql .= ' default ';

                if ($col['default'] === false) {
                    $sql .= "'false'";

                } else {
                    $sql .= "'{$col['default']}'";
                }
            }
            $sql .= ($col['primery'])   ? ' primary key'   : '';
            $sql .= ($col['uniq'])      ? ' unique'        : '';
            $sql .= ($col['not_null'])  ? ' not null'      : '';

            if ( $col['foreign'] ) {
                $sql .= ' references '.$col['foreign']['ref'];

                if ( isset( self::$ON_DELETE[ mb_strtolower( $col['foreign']['del'] ) ] ) ) {
                    $sql .= ' on delete '.self::$ON_DELETE[ mb_strtolower( $col['foreign']['del'] ) ];

                } else {
                    throw new DBManageExeption( "Inviled SQL keyword :".$col['foreign']['del'], 1 );
                }
            }
            $defs[] = $sql;
        }
        while ( $con = $tdo->fetchConstraint() ) {

            $sql = '';
            switch ( mb_strtolower( $con['type'] ) ) {

                case 'uniq':
                    $sql .= ' unique('.implode(',', $con['targets']).')';
                    break;

                case 'foreign':
                    $sql .= ' foreign key '.implode(',', $con['targets']);
                    $sql .= ' references '.$con['references']['tbl'].'('
                        .implode(',', $con['references'][cols]).')';
                        break;
            }
            $defs[] = $sql;
        }
        $sql = 'create table ';
        $sql .= ($if_not_exist) ? 'if not exists ' : '';
        $sql .= $tdo->tablename().' (';
        $sql .= implode(',', $defs);
        $sql .= ');';

        $identifier = $identifier ?? $this->getLastFocus();
        $stmt = $this->prepareExecute( $identifier, $sql );
        return;
    }



    /**
     * @return array
     */
    abstract public function getDataTypeDictionary() :array ;



    /**
     * @param string $sql
     * @param array $parms
     * @return array
     */
    public function query( string $sql, array $parms = [], string $identifier = null ) :PDOStatement {

        $identifier = $identifier ?? $this->getLastFocus();
        return $this->prepareExecute( $identifier, $sql, $parms);
    }



    /**
     * execute prepared statement
     *
     * @param string $sql
     * @param array $parms
     * @throws DBManageExeption
     * @throws Ambigous <PDOException, DBManageExeption>
     * @return array
     */
    private final function prepareExecute( string $identifier, string $sql, array $parms = null ) :PDOStatement {

        $pdo = $this->con[ $this->focus_histry->rec( $identifier ) ];
        try {
            $stmt = $pdo->prepare( $sql );

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


