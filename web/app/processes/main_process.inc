<?php
/**
 *
 * @author amderbar
 *
 */
class MainProcess extends Process
{
    /**
     *
     * @return array
     */
    public function listDB(): array
    {
        try {
            return $this->open(ROOT_DB_ID)->fetchAll( 'projects', ['id' ,'proj_name'], ['id'] );

        } catch ( PDOException | DBManageExeption $e ) {
            $this->handleException( $e );
        }
    }

    /**
     *
     * @param int $proj_id
     * @return array
     */
    public function listUsrTables(int $proj_id) :array
    {
        try {
            return $this->open($proj_id)->fetchAll( SYSTEM_TBL, ['id', 'tbl_name'], ['id'] );

        } catch ( PDOException | DBManageExeption $e ) {
            $this->handleException( $e );
        }
    }
}