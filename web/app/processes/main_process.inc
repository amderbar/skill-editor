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
}
