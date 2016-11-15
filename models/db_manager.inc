<?php
/**
* 
*/
require_once($_SERVER['DOCUMENT_ROOT'].'/skill_editor/common.inc');

/**
* 
*/
class SQLiteHandler {
    /** */
    private $pdo = null;
    /** */
    static public $DATA_TYPES = array(
        'TEXT' => '文字列',
        'INTEGER' => '整数値',
        'REAL' => '実数値',
        'NONE' => 'その他なんでも'
    );
    /** */
    static public $ON_DELETE = array(
        'NULL' => 'SET NULL',
        'CASCADE' => 'CASCADE'
    );

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
        $dsn = 'sqlite:' . $db_name;
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
    public function fetchAll($table, $column='') {
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
    public function findByKey($table, $key_col, $value, $column = '') {
        // $tableと$columnのエスケープ処理が必要
        if (!$column) {
            $stmt = $this->pdo->query('PRAGMA table_info('.$table.')');
            $names = array();
            while ($row = $stmt->fetch()) {
                $names[] = $row['name'];
            }
            $column = implode(',', $names);
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
            $stmt->execute($parms);
            return $this->pdo->lastInsertId();
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
    public function delete($table, $key_col, $value) {
        try {
            $stmt = $this->pdo->prepare(
                'DELETE FROM '.$table.' WHERE '.$key_col.'= ?;');
            return $stmt->execute(array($value));
        } catch (PDOException $e){
            throw $e;
        }
    }

    /**
    * 
    */
    public function createTable($tdo, $if_not_exist = false) {
        try {
            $sql = 'CREATE TABLE ';
            $sql .= ($if_not_exist) ? 'IF NOT EXISTS ' : '';
            $sql .= $tdo->tableName().' (';
            $sql .= 'id integer PRIMARY KEY AUTOINCREMENT';
            while ($col = $tdo->fetchColumn()) {
                if(!isset(self::$DATA_TYPES[strtoupper($col['type'])])) {
                    throw new PDOException("Inviled data type specified :".$col['type'], 1);
                }
                $sql .= ', '.$col['name'].' '.$col['type'];
                if ($col['default'] !== null) {
                    $sql .= ' DEFAULT ';
                    if ($col['default'] === false) {
                        $sql .= 'false';
                    } else {
                        $sql .= $col['default'];
                    }
                }
                if ($col['uniq']) {
                    $sql .= ' UNIQUE';
                }
                if ($col['not_null']) {
                    $sql .= ' NOT NULL';
                }
                if ($col['foreign']) {
                    $sql .= ' REFERENCES '.$col['foreign']['ref'];
                    if (isset(self::$ON_DELETE[strtoupper($col['foreign']['del'])])) {
                        $sql .= ' on DELETE '.self::$ON_DELETE[strtoupper($col['foreign']['del'])];
                    } else {
                       throw new PDOException("Inviled SQL keyword :".$col['foreign']['del'], 1);
                    }
                }
            }
            while ($con = $tdo->fetchConstraint()) {
                $sql .= ', ';
                switch (strtoupper($con['type'])) {
                    case 'UNIQ':
                        $sql .= ' UNIQUE('.implode(',', $con['targets']).')';
                        break;
                    case 'FOREIGN':
                        $sql .= ' FOREIGN KEY '.implode(',', $con['targets']);
                        $sql .= ' REFERENCES '.$con['references']['tbl'].'('
                                    .implode(',', $con['references'][cols]).')';
                        break;
                }
            }
            $sql .= ');';
            $stmt = $this->pdo->prepare($sql);
            $result = $stmt->execute();
            return $result;
        } catch (PDOException $e){
            error_log($e->getMessage() . "\n");
            throw $e;
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
    public function setParm($parm, $value) {
        $this->parms[$parm] = $value;
    }
}

/**
 * 
 */
class TableDefineObject {
    /**  */
    private $tbl_name = "";
    /**  */
    private $colmuns = array();
    /** */
    private $constraints = array();

    /**
    * 
    */
    function __construct($tbl_name, $colmuns = null) {
        $this->tbl_name = $tbl_name;
    }

    /**
    * 
    */
    public function tableName() {
        return $this->tbl_name;
    }

    /**
    * 
    */
    public function appendColumn($col_name, $type, $constraints = array()) {
        $col_hash = array(
            'name' => $col_name,
            'type' => $type,
            'default' => (isset($constraints['default'])) ? $constraints['default'] : null,
            'uniq' => (isset($constraints['uniq'])) ? $constraints['uniq'] : false,
            'not_null'=> (isset($constraints['not_null'])) ? $constraints['not_null'] : false,
            'foreign' => (isset($constraints['foreign'])) ? $constraints['foreign'] : null // reference and on delete
        );
        $this->colmuns[] = $col_hash;
    }

    /**
    * 
    */
    public function fetchColumn() {
        return array_shift($this->colmuns);
    }

    /**
    * 
    */
    public function appendConstraint($con_name, $targets, $references = null) {
        $con_hash = array(
            'type' => $con_name,
            'targets' => $targets, // constraint target columins (itselfs)
            'references' => $references // foreign key constraint references columins
        );
        $this->constraints[] = $con_hash;
    }

    /**
    * 
    */
    public function fetchConstraint() {
        return array_shift($this->constraints);
    }
}

?>