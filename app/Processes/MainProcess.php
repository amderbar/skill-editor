<?php

namespace Amderbar\App\Processes;

use PDOException;
use stdClass;
use Generator;
use Amderbar\AmbExeption;
use Amderbar\Lib\Utils\DbUtil as Db;
use Amderbar\Lib\Utils\FileUtil as File;

/**
 * @author amderbar
 *
 */
class MainProcess extends Process
{
    /**
     *
     * @return array
     */
    public function listDB(stdClass $user) :array
    {
        try {
            return $this->open(ROOT_DB_ID)
                ->query('
                    select proj_id, proj_name
                    from projects
                    where owner = :owner
                    order by display_order, update_at
                ', [':owner' => $user->user_id])
                ->fetchAll() ?: [];
        } catch (PDOException | DBManageExeption $e) {
            $this->handleException($e);
        }
    }

    /**
     * TODO: 列の表示並び順を保存できるようにする
     *
     * @param string $proj_name
     * @param int $user_id
     * @return int
     */
    public function registerDB(string $proj_name, int $user_id) :int
    {
        try {
            return $this->open(ROOT_DB_ID)
                ->transaction(function (DB $db) use ($proj_name, $user_id) {
                    $proj_id = $this->createSystemTable($db->insertGetId('projects', [
                        'proj_name' => $proj_name,
                        'owner' => $user_id,
                        'display_order' => $this->getDisplayOrderLast(ROOT_DB_ID, 'projects')
                    ]));
                    $tmpl_dir = File::fullPath(sprintf(RESOURCE_ROOT . '/templates/proj%03d', $proj_id), true);
                    if (!file_exists($tmpl_dir)) {
                        mkdir($tmpl_dir, 0666);
                    }
                    return $proj_id;
                });
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
        $sqls[] = 'create table templates (' . implode(',', [
            'tmpl_id integer primary key'
            , 'tmpl_name text not null'
            , 'display_order integer not null'
            , 'update_at datetime not null default CURRENT_TIMESTAMP'
        ]) . ');'; 
        // system_admin tables
        $sqls[] = 'create table ' . SYSTEM_TBL . ' (' . implode(',', [
            'tbl_id integer primary key'
            , 'tbl_name text not null'
            , 'actual_name text unique not null'
            , 'tmpl_id integer references templates(tmpl_id) on delete set null'
            , 'display_order integer not null'
            , 'update_at datetime not null default CURRENT_TIMESTAMP'
        ]) . ');'; 

        $sqls[] = 'create table ' . SYSTEM_COL . ' (' . implode(',', [
            'col_id integer primary key'
            , 'tbl_id integer not null references ' . SYSTEM_TBL . '(tbl_id) on delete cascade'
            , 'col_name text not null'
            , 'actual_name text not null'
            , 'default_val blob'
            , 'ref_col integer references ' . SYSTEM_COL . '(col_id) on delete set null'
            , 'form_type text'
            , 'is_primary boolean not null default false'
            , 'multiple boolean not null default false'
            , 'display_order integer not null'
            , 'update_at datetime not null default CURRENT_TIMESTAMP'
            , 'unique(tbl_id, actual_name)'
        ]) . ');'; 

        $sqls[] = 'create table ' . INTERNAL_TBLS . ' (' . implode(',', [
            'tbl_id integer primary key'
            , 'tbl_name text unique not null'
            , 'ref_src_col_id integer references ' . SYSTEM_COL . '(col_id) on delete cascade'
            , 'ref_dest_col_id integer references ' . SYSTEM_COL . '(col_id) on delete set null'
        ]) . ');'; 

        $sqls[] = 'create table ' . NUM_SETTINGS . ' (' . implode(',', [
            'num_setting_id integer primary key'
            , 'col_id integer not null references ' . SYSTEM_COL . '(col_id) on delete cascade'
            , 'step real'
            , 'max real'
            , 'min real'
        ]) . ');'; 

        $this->open($proj_id, true)
            ->transaction(function (DB $db) use ($sqls) {
                foreach ($sqls as $sql) {
                    $db->query($sql);
                }
                return $db;
            });
        return $proj_id;
    }

    /**
     *
     * @param int $proj_id
     * @param int $user_id
     */
    public function deleteDB(int $proj_id, int $user_id)
    {
        try {
            $this->close($proj_id);
            $this->open(ROOT_DB_ID)
            ->transaction(function (DB $db) use ($proj_id, $user_id) {
                $count = $db->query('
                        delete from projects
                        where proj_id = :proj_id and owner = :owner
                    ', [
                        ':proj_id' => $proj_id,
                        ':owner' => $user_id
                    ])
                    ->rowCount();
                if ($count > 0) {
                    if ($db_file = File::fullPath(sprintf(RESOURCE_ROOT . '/proj%03d.db', $proj_id))) {
                        unlink( $db_file );
                    }
                    if ($tmpl_dir = File::fullPath(sprintf(RESOURCE_ROOT . '/templates/proj%03d', $proj_id))) {
                        if ($handle = opendir($tmpl_dir)) {
                            while (false !== ($tmpl = readdir($handle))) {
                                if ($tmpl != "." && $tmpl != "..") {
                                    unlink($tmpl);
                                }
                            }
                            rmdir($tmpl_dir);
                        }
                        closedir($handle);
                    }
                }
            });
        } catch (PDOException | DBManageExeption $e) {
            $this->handleException($e);
        }
    }
}
