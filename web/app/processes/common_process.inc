<?php
/**
 *
 * @author amderbar
 *
 */
class Process {
    /** */
    private $con = array ();

    /**
     *
     */
    public function __construct()
    {
        $db_con = $this->open( ROOT_DB_ID ) ?? $this->setupSystemDB();
    }

    /**
     * @param int $proj_id
     * @param bool $is_new
     * @return SQLiteHandler
     */
    public function open(int $proj_id, bool $is_new = false): ?SQLiteHandler
    {
        $db_file = ($proj_id) ? sprintf( RESOURCE_ROOT . '/proj%03d.db', $proj_id ) : ROOT_DB;
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
    public function close(int $proj_id): void
    {
        $this->con[$proj_id] = null;
        unset( $this->con[$proj_id] );
    }

    /**
     * @param int $proj_id
     * @return string
     */
    public function projName(int $proj_id): ?string
    {
        $row = $this->con[ROOT_DB_ID]->load( 'projects', $proj_id , [ 'proj_name'] );
        return $row['proj_name'] ?? null;
    }

    /**
     * @param int $proj_id
     * @param int $tbl_id
     * @param bool $is_actual
     * @return string
     */
    public function tblName(int $proj_id, int $tbl_id, bool $is_actual = false): ?string
    {
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
    public function colName(int $proj_id, int $col_id, ?bool $is_actual = false): ?string
    {
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
    public function getTblColNameFromColid( int $proj_id, int $col_id, ?bool $is_actual = false ) :?array
    {
        $result = $this->con[ $proj_id ]->findByKeyWithJoin(
            [ SYSTEM_TBL, SYSTEM_COL => [ [SYSTEM_TBL => 'id', SYSTEM_COL => 'tbl_id'] ] ],
            [ SYSTEM_COL => ['id' => $col_id ] ],
            [ SYSTEM_TBL => ['actual_name as tbl_name'], SYSTEM_COL => ['actual_name as col_name'] ]
            );
        return array_shift( $result );
    }


    /**
     *
     */
    private function setupSystemDB() :SQLiteHandler
    {
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
    protected function handleException( RuntimeException $e ) {
        $err_str = get_class( $e ).' throwen:' . $e->getMessage() . PHP_EOL;
        $err_str .= ( $e->queryString ?? '' ) . PHP_EOL;
        $err_str .= var_export( ( $e->bindedParam ?? '' ), true ) . PHP_EOL;
        $err_str .= $e->getTraceAsString();
        error_log( $err_str );
        throw $e;
    }
}