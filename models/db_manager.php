<?php
/**
* 
*/
class SQLiteHandler {
    private $pdo = null;

    /**
    * 
    */
    function __construct($db_name) {
        $this->connect($db_name);
    }

    /**
    * 
    */
    public function connect($db_name) {
        $dsn = 'resources/' . $db_name;
        $dsn = 'sqlite:' . full_path($dsn);
        try {
            $this->pdo = new PDO($dsn);
            $this->pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
            $this->pdo->exec('PRAGMA foreign_keys = true;');
        } catch (PDOException $e){
            die('Connection failed:'. $e->getMessage());
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
    public function fetchAll($table,$column='') {
        // $tableと$columnのエスケープ処理が必要
        if (! $column) {
            $stmt = $this->pdo->query('PRAGMA table_info('.$table.')');
            $names = array();
            while ($row = $stmt->fetch(PDO::FETCH_ASSOC)) {
                $names[] = $row['name'];
            }
            $column = implode(',',$names);
        }
        $stmt = $this->pdo->prepare('SELECT '.$column.' FROM '.$table.';');
        $stmt->execute();
        return $stmt->fetchAll();
    }

    /**
    * 
    */
    public function execSQL($sql) {
        try {
            $this->pdo->exec($sql);
        } catch (PDOException $e){
            die('PDOException throwen:'. $e->getMessage());
        }
    }
}
?>