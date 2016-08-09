<?php
require_once(full_path('models/db_manager.php'));

/**
 * 
 */
class DBEditor {
    private $con = null;

    function __construct($db_file=null) {
        $this->con = array();
        $this->con[ROOT_DB] = new SQLiteHandler(ROOT_DB);
        if ($db_file) {
           $this->con[$db_file] = new SQLiteHandler($db_file);
        }
    }

    /**
    * 
    */
    public function open($db_file) {
        $this->con[$db_file] = new SQLiteHandler($db_file);
    }

    /**
    * 
    */
    public function close($db_file) {
        $this->con[$db_file] = null;
        unset($this->con[$db_file]);
    }

    /**
    * 
    */
    public function listDB() {
        try {
            return $this->con[ROOT_DB]->fetchAll('projects');
        } catch (PDOException $e){
            die('PDOException throwen:'. $e->getMessage());
        }
    }

    /**
    * 
    */
    private function setupRootDB() {
        $sql = <<< _SQL_
CREATE TABLE IF NOT EXISTS projects (
    id integer PRIMARY KEY AUTOINCREMENT,
    file_name text NOT NULL
);
_SQL_;
        $this->con[ROOT_DB]->execSQL($sql);
        $sql = <<< _SQL_
CREATE TABLE IF NOT EXISTS templates (
    id integer PRIMARY KEY AUTOINCREMENT,
    proj_id integer REFERENCES projects(id) on DELETE SET NULL,
    file_name text NOT NULL,
    UNIQUE(proj_id, file_name)
);
_SQL_;
        $this->con[ROOT_DB]->execSQL($sql);
    }

}
?>