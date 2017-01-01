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
        'DATETIME' => '日付時刻',
        'BOOLEAN' => '真偽値',
        'NUMERIC' => '数値',
        'BLOB' => 'その他なんでも'
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
            $column = implode(',', $names);
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
    public function findByKey($table, $key_col, $value, $column = null) {
        // $tableと$columnのエスケープ処理が必要
        if (empty($column)) {
            $stmt = $this->pdo->query('PRAGMA table_info('.$table.')');
            $column = array();
            while ($row = $stmt->fetch()) {
                $column[] = $row['name'];
            }
        }
        $stmt = $this->pdo->prepare(
            'SELECT '.implode(',', $column).' FROM '.$table.' WHERE '.$key_col.'= ?;');
        if ($stmt->execute(array($value))) {
            return $stmt->fetchAll();
        }
        return false;
    }

    /**
    * 
    */
    public function insert($dto, $if_redundant = false) {
        try {
            $place_holders = array_keys($dto->getParms());
            $colnames = array();
            foreach ($place_holders as $colname) {
                $colnames[] = mb_ereg_replace('^:', '', $colname); 
            }
            $sql = 'INSERT INTO '.$dto->getTableName();
            $sql .= '('.implode(',', $colnames).') VALUES ('.implode(',', $place_holders).');';
            $stmt = $this->pdo->prepare($sql);
            $stmt->execute($dto->getParms());
            return $this->pdo->lastInsertId();
        } catch (PDOException $e){
            if ($e->getCode() == 23000) { // 19 UNIQUE constraint failed
                if ($if_redundant) {
                    $err_arr = explode(' ', $e->getMessage());
                    $colname = explode('.', array_pop($err_arr));
                    $colname = array_pop($colname);
                    $parms = $dto->getParms();
                    $row = $this->findByKey(
                        $dto->getTableName(), $colname, $parms[':'.$colname]);
                    return $row[0]['id'];
                } else {
                    return false;
                }
            } else {
                throw $e;
            }
        }
    }

    /**
    * 
    */
    public function update($dto, $conditions = null) {
        $place_holders = array_keys($dto->getParms());
        $settings = array();
        foreach ($place_holders as $colname) {
            $settings[] = mb_ereg_replace('^:', '', $colname).'='.$colname; 
        }
        $parms = $dto->getParms();
        $sql = 'UPDATE '.$dto->getTableName();
        $sql .= ' SET '.implode(',', $settings);
        if (!empty($condition)) {
            $sql .= ' WHERE ';
            $place_holders = array();
            foreach ($condition as $colname => $value) {
                $place_holders[] = $colname.'=:condition_'.$colname;
                $parms[':condition_'.$colname] = $value;
            }
            $sql .= implode(' AND ', $place_holders);
        }
        $stmt = $this->pdo->prepare($sql.';');
        return $stmt->execute($parms);
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
            $defs = array();
            while ($col = $tdo->fetchColumn()) {
                if(!isset(self::$DATA_TYPES[strtoupper($col['type'])])) {
                    // ここの例外クラスは自作必要？
                    throw new RuntimeException("Inviled data type specified :".$col['type'], 1);
                }
                $sql = $col['name'].' '.$col['type'];
                // TODO:Default値のSQLエスケープはどうすれば？
                if (isset($col['default'])) {
                    $sql .= ' DEFAULT ';
                    if ($col['default'] === false) {
                        $sql .= '"false"';
                    } else {
                        $sql .= '"'.$col['default'].'"';
                    }
                }
                if ($col['primery']) {
                    $sql .= ' PRIMARY KEY';
                }
                if ($col['uniq']) {
                    $sql .= ' UNIQUE';
                }
                if ($col['not_null']) {
                    $sql .= ' NOT NULL';
                }
                if ($col['increment']) {
                    $sql .= ' AUTOINCREMENT';
                }
                if ($col['foreign']) {
                    $sql .= ' REFERENCES '.$col['foreign']['ref'];
                    if (isset(self::$ON_DELETE[strtoupper($col['foreign']['del'])])) {
                        $sql .= ' on DELETE '.self::$ON_DELETE[strtoupper($col['foreign']['del'])];
                    } else {
                        // ここの例外クラスは自作必要？
                       throw new RuntimeException("Inviled SQL keyword :".$col['foreign']['del'], 1);
                    }
                }
                $defs[] = $sql;
            }
            while ($con = $tdo->fetchConstraint()) {
                $sql = '';
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
                $defs[] = $sql;
            }
            $sql = 'CREATE TABLE ';
            $sql .= ($if_not_exist) ? 'IF NOT EXISTS ' : '';
            $sql .= $tdo->tableName().' (';
            $sql .= implode(',', $defs);
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
    abstract function getTableName();

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
    function __construct($tbl_name, $colmuns = null, $auto_primery = true) {
        $this->tbl_name = $tbl_name;
        if ($auto_primery) {
            $this->appendColumn('id', 'integer', ['primery' => true]);
        }
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
            'uniq' => (isset($constraints['uniq'])) ? !!$constraints['uniq'] : false,
            'not_null'=> (isset($constraints['not_null'])) ? !!$constraints['not_null'] : false,
            'primery' => (isset($constraints['primery'])) ? !!$constraints['primery'] : false,
            'increment' => (isset($constraints['increment'])) ? !!$constraints['increment'] : false,
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