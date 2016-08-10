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
            $this->pdo->setAttribute(PDO::ATTR_DEFAULT_FETCH_MODE, PDO::FETCH_ASSOC);
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
            while ($row = $stmt->fetch()) {
                $names[] = $row['name'];
            }
            $column = implode(',',$names);
        }
        $stmt = $this->pdo->prepare('SELECT '.$column.' FROM '.$table.';');
        $stmt->execute();
        $rows = $stmt->fetchAll();
        return $rows;
    }

    public function insert($dto) {
        $stmt = $this->pdo->prepare($dto->getInsertSQL());
        $data_array = $dto->getDataArray();
        $stmt->execute($data_array);
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

/**
 * 
 */
abstract class DTO {
    /**  */
    protected $data_array = array();

    /**
    * 
    */
    public function getInsertSQL() {}

    /**
    * 
    */
    public function getDataArray() {
        return $this->data_array;
    }
}

?>