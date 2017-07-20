<?php
/**
 * TODO: user_idをいちいちuser_nameで検索して取得するのが手間。セッションに保存するなどしたい
 * @author amderbar
 *
 */
class MainProcess extends Process
{
    /**
     *
     * @return array
     */
    public function listDB(array $user): array
    {
        try {
            $con = $this->open(ROOT_DB_ID);
            // get user_id, if not exist, register and generate 
            $user_id = $con->findByKey('users', [
                    'name' => $user['name']
            ], ['id'])[0]['id'] ?? $this->registerUser($user['name'], $user['password'] ?? 'null');

            return $con->findByKey('projects', ['owner' => $user_id], ['id', 'proj_name'], ['id']);

        } catch (PDOException | DBManageExeption $e) {
            $this->handleException($e);
        }
    }

    /**
     *
     * @return array
     */
    private function registerUser(string $name, string $passwd) :int
    {
        return $this->open(ROOT_DB_ID)->insert('users', [
            'name' => $name,
            'passwd'=> $passwd
        ]);
    }

    /**
     * TODO: 列の表示並び順を保存できるようにする
     *
     * @param string $proj_name
     * @param string $user_name
     * @return int
     */
    public function registerDB(string $proj_name, string $user_name) :int
    {
        try {
            $db_con = $this->open(ROOT_DB_ID);
            $proj_id = $this->createSystemTable($db_con->insert('projects', [
                    'proj_name' => $proj_name,
                    'owner' => $db_con->findByKey('users', ['name' => $user_name], ['id'])[0]['id']
                ], true));
            $tmpl_dir = full_path(sprintf(RESOURCE_ROOT . '/templates/proj%03d', $proj_id), true);
            if (!file_exists($tmpl_dir)) {
                mkdir($tmpl_dir, 0666);
            }
            return $proj_id;
        } catch (PDOException | DBManageExeption $e) {
            $this->handleException($e);
        }
    }

    /**
     * TODO: 列の表示並び順を保存できるようにする
     *
     * @param int $proj_id
     * @return int
     */
    private function createSystemTable(int $proj_id) :int
    {
        // templates table
        $tdos[] = TableDefineObject::table('templates')
            ->appendColumn('tmpl_name', 'text', ['not_null' => true]);
        // system_admin tables
        $tdos[] = TableDefineObject::table(SYSTEM_TBL)
            ->appendColumn('tbl_name', 'text', ['not_null' => true])
            ->appendColumn('actual_name', 'text', ['uniq' => true, 'not_null' => true])
            ->appendColumn('display_order', 'integer', ['uniq' => true, 'not_null' => true]);

        $tdos[] = TableDefineObject::table(SYSTEM_COL)
            ->appendColumn('tbl_id', 'integer', [
                'foreign' => ['ref' => SYSTEM_TBL . '(id)', 'del' => 'cascade']
            ])->appendColumn('col_name', 'text', ['not_null' => true])
            ->appendColumn('actual_name', 'text', ['not_null' => true])
            ->appendColumn('default_val', 'blob')
            ->appendColumn('ref_col', 'integer', [
                'foreign' => ['ref' => SYSTEM_COL . '(id)', 'del' => 'null']
            ])->appendColumn('form_type', 'integer', ['not_null' => true])
            ->appendColumn('multiple', 'boolean', ['not_null' => true, 'default' => false])
            ->appendColumn('display_order', 'integer', ['uniq' => true, 'not_null' => true])
            ->appendConstraint('uniq', ['tbl_id', 'actual_name']);

        $tdos[] = TableDefineObject::table(NUM_SETTINGS)
            ->appendColumn('col_id', 'integer', [
                'foreign' => ['ref' => SYSTEM_COL . '(id)', 'del' => 'cascade']
            ])->appendColumn('step', 'real')
            ->appendColumn('max', 'real')
            ->appendColumn('min', 'real');

        $db_con = $this->open($proj_id, true);
        foreach ($tdos as $tdo) {
            $db_con->createTable($tdo, true);
        }

        return $proj_id;
    }

    /**
     *
     * @param int $proj_id
     * @param string $user_name
     */
    public function deleteDB(int $proj_id, string $user_name)
    {
        // FIXME: トランザクション処理必要
        try {
            $this->close($proj_id);
            $db_con = $this->open(ROOT_DB_ID);
            if ($db_con->delete('projects', [
                'id' => $proj_id,
                'owner' => $db_con->findByKey('users', ['name' => $user_name], ['id'])[0]['id']
            ]) > 0) {
                if ($db_file = full_path(sprintf(RESOURCE_ROOT . '/proj%03d.db', $proj_id))) {
                    unlink( $db_file );
                }
                if ($tmpl_dir = full_path(sprintf(RESOURCE_ROOT . '/templates/proj%03d', $proj_id))) {
                    if ($handle = opendir($tmpl_dir)) {
                        while ( false !== ($tmpl = readdir($handle)) ) {
                            if ($tmpl != "." && $tmpl != "..") {
                                unlink($tmpl);
                            }
                        }
                        rmdir($tmpl_dir);
                    }
                    closedir($handle);
                }
            }
        } catch (PDOException | DBManageExeption $e) {
            $this->handleException($e);
        }
    }
}
