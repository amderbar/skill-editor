<?php

require_once($_SERVER['DOCUMENT_ROOT'].'/skill_editor/common.inc');
require_once($_SERVER['DOCUMENT_ROOT'].'/skill_editor/models/data_transfer_objects.inc');

/**
 * @author amderbar
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
    public function load(string $table, int $id, array $target_col = array()) :?array {
        try {
            // $tableと$columnのエスケープ処理が必要
            if (empty($target_col)) {
                $stmt = $this->pdo->query('PRAGMA table_info('.$table.')');
                $target_col = array_column($stmt->fetchAll(), 'name');
            }
            $sql = 'SELECT '.implode(',', $target_col).' FROM '.$table.' WHERE id = ?;';

            $stmt = $this->pdo->prepare($sql);
            if ( $stmt->execute( [$id] ) ) {
                $result = $stmt->fetchAll();
                return array_shift($result);
            }
            return null;
        } catch (PDOException $e) {
            if (isset($sql)) {
                $e->queryString = $sql;
            }
            throw $e;
        }
    }

    public function isExists(string $table, array $condition) :bool {
        try {
            // $tableと$columnのエスケープ処理が必要
            $key_cols = array();
            $key_vals = array();
            foreach ($condition as $col => $val) {
                $key_cols[] = $col.'= ?';
                $key_vals[] = $val;
            }
            $sql = 'SELECT EXISTS( SELECT id FROM '.$table.'WHERE '.implode(' AND ', $key_cols).') AS judge;';

            $stmt = $this->pdo->prepare($sql);
            if ( $stmt->execute( $key_vals ) ) {
                $result = $stmt->fetch();
                return boolval($result['judge']);
            } else {
                throw new DBManageExeption( 'Execute Prepared statement Failed.' );
            }
        } catch (PDOException | DBManageExeption $e) {
            if (isset($sql)) {
                $e->queryString = $sql;
            }
            throw $e;
        }
    }

    /**
    *
    */
    public function fetchAll(string $table, $target_col='') {
        try {
            // $tableと$columnのエスケープ処理が必要
            if (! $target_col) {
            	$stmt = $this->pdo->query('PRAGMA table_info('.$table.')');
            	$target_col = array_column($stmt->fetchAll(), 'name');
            	$target_col = implode(',', $target_col);
            }
            $stmt = $this->pdo->prepare('SELECT '.$target_col.' FROM '.$table.';');
            if ($stmt->execute()) {
                return $stmt->fetchAll();
            }
            return false;
        } catch (PDOException $e){
            if (isset($sql)) {
                $e->queryString = $sql;
            }
            throw $e;
        }
    }

    /**
     * @param string $table
     * @param array $condition [colmun_name => value] format array
     * @param array $target_col
     * @return array
     */
    public function findByKey(string $table, array $condition, $target_col = array()) :?array {
        // $tableと$columnのエスケープ処理が必要
        if (empty($target_col)) {
            $stmt = $this->pdo->query('PRAGMA table_info('.$table.')');
            $target_col = array_column($stmt->fetchAll(), 'name');
        }
        $key_cols = array();
        foreach (array_keys($condition) as $col) {
        	$key_cols[] = $col.'= ?';
        }
        $stmt = $this->pdo->prepare(
            'SELECT '.implode(',', $target_col).' FROM '.$table.' WHERE '.implode(' AND ', $key_cols).';'
        );
        if ($stmt->execute(array_values($condition))) {
            return $stmt->fetchAll();
        }
        return null;
    }

    /**
     * @param array $join_tbls  [ tbl_name, tbl_name => [ [tbl_name => col_name, tbl_name => col_name], ... ], ... ]
     * @param array $condition  [ tbl_name => [col_name => value], ... ]
     * @param array $target_col [ tbl_name => [col_name, ... ], ... ]
     * @return array
     */
    public function findByKeyWithJoin(array $join_tbls, array $condition, $target_col = array()) :?array {
        try {
            // $tableと$columnのエスケープ処理が必要

            if (empty($target_col)) {
                $target_col = array();
                foreach ($join_tbls as $tbl_name => $join_ons) {
                    $stmt = $this->pdo->query('PRAGMA table_info('.$tbl_name.')');
                    $target_col[$tbl_name] = array_column($stmt->fetchAll(), 'name');
                }
            }
            $target = array();
            foreach ($target_col as $tbl_name => $col_names) {
                foreach ($col_names as $col_name) {
                    $target[] = $tbl_name.'.'.$col_name;
                }
            }

            $table = array( array_shift($join_tbls) );
            foreach ($join_tbls as $tbl_name => $join_condisions) {
                $join_on = array();
                foreach ($join_condisions as $eq_pair) {
                    $on_equation = '';
                    var_export_log($eq_pair);
                    foreach ($eq_pair as $on_tbl => $on_col) {
                        $on_equation .= ($on_equation) ? ' = ' : '';
                        $on_equation .= $on_tbl.'.'.$on_col;
                    }
                    $join_on[] = $on_equation;
                }
                $table[] = $tbl_name.' on '.implode(' AND ', $join_on);
            }

            $key_cols = array();
            $cond_vals = array();
            foreach ($condition as $tbl_name => $col_val) {
                foreach ($col_val as $col_name => $value) {
                    $key_cols[] = $tbl_name.'.'.$col_name.'= ?';
                    $cond_vals[] = $value;
                }
            }

            $stmt = $this->pdo->prepare(
                    'SELECT '.implode(','     , $target)
                    .' FROM ' .implode(' join ', $table)
                    .' WHERE '.implode(' AND ' , $key_cols).';'
                    );
            if ($stmt->execute($cond_vals)) {
                return $stmt->fetchAll();
            }
            return null;
        } catch (PDOException $e){
            if (isset($sql)) {
                $e->queryString = $sql;
            }
            throw $e;
        }
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
                        $dto->getTableName(), [ $colname => $parms[':'.$colname] ]);
                    return $row[0]['id'];
                } else {
                    return false;
                }
            } else {
                if (isset($sql)) {
                    $e->queryString = $sql;
                }
                throw $e;
            }
        }
    }

    /**
    *
    */
    public function update($dto, $conditions = null) {
        try {
            $place_holders = array_keys($dto->getParms());
            $settings = array();
            foreach ($place_holders as $colname) {
                $settings[] = mb_ereg_replace('^:', '', $colname).'='.$colname;
            }
            $parms = $dto->getParms();
            $sql = 'UPDATE '.$dto->getTableName();
            $sql .= ' SET '.implode(',', $settings);
            if (!empty($conditions)) {
                $sql .= ' WHERE ';
                $place_holders = array();
                foreach ($conditions as $colname => $value) {
                    $place_holders[] = $colname.'=:condition_'.$colname;
                    $parms[':condition_'.$colname] = $value;
                }
                $sql .= implode(' AND ', $place_holders);
            }
            $stmt = $this->pdo->prepare($sql.';');
            return $stmt->execute($parms);
        } catch (PDOException $e){
            if (isset($sql)) {
                $e->queryString = $sql;
            }
            throw $e;
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
            if (isset($sql)) {
                $e->queryString = $sql;
            }
            throw $e;
        }
    }

    /**
    *
    */
    public function createTable(TableDefineObject $tdo, bool $if_not_exist = false) :bool {
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
            if (isset($sql)) {
                $e->queryString = $sql;
            } else {
                $e->queryString = '';
            }
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
            if (isset($sql)) {
                $e->queryString = $sql;
            }
            throw $e;
        }
    }
}

