<?php
/**
* 
*/
class SQLiteHandler {
    private $pdo = null;

    /**
    * 
    */
    function __construct($db_name,$newfile=false) {
        $this->connect($db_name,$newfile);
    }

    /**
    * 
    */
    public function connect($db_name,$newfile=false) {
        $dsn = 'resources/' . $db_name;
        $dsn = 'sqlite:' . full_path($dsn,$newfile);
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
        if ($stmt->execute()) {
            return $stmt->fetchAll();
        }
        return false;
    }

    /**
    * 
    */
    public function findByKey($table,$key_col,$value,$column='') {
        // $tableと$columnのエスケープ処理が必要
        if (! $column) {
            $stmt = $this->pdo->query('PRAGMA table_info('.$table.')');
            $names = array();
            while ($row = $stmt->fetch()) {
                $names[] = $row['name'];
            }
            $column = implode(',',$names);
        }
        $stmt = $this->pdo->prepare(
            'SELECT '.$column.' FROM '.$table.' WHERE '.$key_col.'= ?;');
        if ($stmt->execute(array($value))) {
            return $stmt->fetchAll();
        }
        return false;
    }

    /**
    * 
    */
    public function insert($dto) {
        try {
            $stmt = $this->pdo->prepare($dto->getInsertSQL());
            $parms = $dto->getParms();
            $result = $stmt->execute($parms);
            return $result;
        } catch (PDOException $e){
            if (strpos($e->getMessage(),'19 UNIQUE constraint failed')>0) {
                error_log($e->getMessage() . "\n");
                return false;
            } else {
                throw $e;
            }
        }
    }

    /**
    * 
    */
    public function execSQL($sql, $parms = array()) {
        try {
            $stmt = $this->pdo->prepare($sql);
            foreach ($parms as $key => $value) {
                $stmt->bindParam($key, $value);
            }
            if ($stmt->execute()) {
                return $stmt->fetchAll();
            }
            return false;
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
    private $parms = array();

    /**
    * 
    */
    abstract public function getInsertSQL();

    /**
    * 
    */
    public function getParms() {
        return $this->parms;
    }

    /**
    * 
    */
    public function setParm($parm,$value) {
        $this->parms[$parm] = $value;
    }
}

?>