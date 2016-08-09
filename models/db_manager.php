<?php
/**
* 
*/
class SQLiteHandler {
    private $pdo = null;

    /**
    * 
    */
    function __construct($db_name = 'project_manager.db') {
        $this->connect($db_name);
    }

    /**
    * 
    */
    public function connect($db_name) {
        $dsn = '/../resources/' . $db_name;
        $dsn = 'sqlite:' . str_replace(DIRECTORY_SEPARATOR, '/', __DIR__) . $dsn;
        try {
            $this->pdo = new PDO($dsn);
            $this->pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
        } catch (PDOException $e){
            die('Connection failed:'. $e->getMessage());
        }
        try {
            $this->pdo->exec('PRAGMA foreign_keys = true;');
            if ($db_name === 'project_manager.db') {
                $this->setupManagerDB($db_name);
            }
        } catch (PDOException $e){
            die('PDOException throwen:'. $e->getMessage());
        }
    }

    /**
    * 
    */
    public function load($id) {
        # code...
    }

    /**
    * 
    */
    public function ls_db() {
        $stmt = $this->pdo->prepare(
            "SELECT file_name FROM projects;");
        $stmt->execute();
        return $stmt->fetchAll();
    }

    /**
    * 
    */
    private function setupManagerDB($db_name) {
        $sql = <<< _SQL_
CREATE TABLE IF NOT EXISTS projects (
    id integer PRIMARY KEY AUTOINCREMENT,
    file_name text NOT NULL
);
_SQL_;
        $this->pdo->exec($sql);
        $sql = <<< _SQL_
CREATE TABLE IF NOT EXISTS templates (
    id integer PRIMARY KEY AUTOINCREMENT,
    proj_id integer REFERENCES projects(id) on DELETE SET NULL,
    file_name text NOT NULL,
    UNIQUE(proj_id, file_name)
);
_SQL_;
        $this->pdo->exec($sql);
    }
}
?>