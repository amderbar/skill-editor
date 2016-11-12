<?php
/**
* session check.
*/
require_once($_SERVER['DOCUMENT_ROOT'].'/skill_editor/common.inc');

require_once(full_path('models/db_manager.inc'));
define('ROOT_DB', 'resources/system_admin.db');
define('ACTUAL_TBL', 'actual_names_tbl');
define('ACTUAL_COL', 'actual_names_col');

/**
 * 
 */
class DBEditor {
    private $con = array();

    function __construct() {
        $this->open(0);
        $this->setupRootDB();
    }

    /**
    * 
    */
    public function open($proj_id, $is_new = false) {
        $db_file = ($proj_id) ? sprintf('resources/proj%03d.db', $proj_id) : ROOT_DB;
        $db_file = full_path($db_file, $is_new);
        if (!$db_file) {
            return false;
        } else if (!isset($this->con[$proj_id])) {
            $this->con[$proj_id] = new SQLiteHandler($db_file);
        }
        return $this->con[$proj_id];
    }

    /**
    * 
    */
    public function close($proj_id) {
        $this->con[$proj_id] = null;
        unset($this->con[$proj_id]);
    }

    /**
    * 
    */
    public function projName($proj_id) {
        $row = $this->con[0]->findByKey('projects', 'id', $proj_id, 'proj_name');
        return (isset($row[0]['proj_name'])) ? $row[0]['proj_name'] : null;
    }

    /**
    * 
    */
    public function listDB() {
        try {
            $proj_list = array();
            foreach ($this->con[0]->fetchAll('projects') as $row) {
                $proj_list[$row['id']] = $row['proj_name'];
            }
            return $proj_list;
        } catch (PDOException $e){
            die('PDOException throwen:'. $e->getMessage());
        }
    }

    /**
    * 
    */
    public function registerDB($proj_name) {
        try {
            //トランザクション処理必要
            $dto = new class($proj_name) extends DTO {
                private $table = 'projects';
                function __construct($proj_name) {
                    $this->setParm(':proj_name', $proj_name);
                }
                public function getInsertSQL() {
                    return 'INSERT INTO '.$this->table.'(proj_name) VALUES (:proj_name)';
                }
            };
            $id = $this->con[0]->insert($dto);
            if (!$id) {
                $row = $this->con[0]->findByKey('projects','proj_name', $proj_name);
                $id = $row[0]['id'];
            }
            $tmpl_dir = full_path(sprintf('resources/templates/proj%03d', $id), true);
            if(!file_exists($tmpl_dir)){
                mkdir($tmpl_dir, 0666);
            }
            $db_con = $this->open($id, true);
            // system_admin tables
            $tdo = new TableDefineObject(ACTUAL_TBL);
            $tdo->appendColumn('tbl_name', 'text', ['uniq' => true, 'not_null' => true]);
            // $tdo->appendColumn('actual_name', 'text', ['foreign' => ['ref' => 'sqlite_master(name)', 'del' => 'cascade']]);
            $tdo->appendColumn('actual_name', 'text', ['uniq' => true, 'not_null' => true]);
            $db_con->createTable($tdo, true);
            $tdo = new TableDefineObject(ACTUAL_COL);
            $tdo->appendColumn('tbl_id', 'integer', ['foreign' => ['ref' => 'actual_names_tbl(id)', 'del' => 'cascade']]);
            $tdo->appendColumn('col_name', 'text', ['not_null' => true]);
            $tdo->appendColumn('actual_name', 'text', ['not_null' => true]);
            $db_con->createTable($tdo, true);
            return $id;
        } catch (PDOException $e){
            die('PDOException throwen:'. $e->getMessage());
        }
    }

    /**
    *
    */
    public function deleteDB($proj_id) {
        //トランザクション処理必要
        if (isset($this->con[$proj_id])) {
            $this->close($proj_id);
        }
        $this->con[0]->delete('projects', 'id', $proj_id);
        if($tmpl_dir = full_path(sprintf('resources/templates/proj%03d', $proj_id))) {
            if ($handle = opendir($tmpl_dir)) {
                while (false !== ($tmpl = readdir($handle))) {
                    unlink($tmpl);
                }
                rmdir($tmpl_dir);
            }
            closedir($handle);
        }
        if ($db_file = full_path(sprintf('resources/proj%03d.db', $proj_id))) {
            unlink($db_file);
        }
    }

    public function registerTemplate($proj_id, $tmpl_name) {
        try {
            $dto = new class($proj_id,$tmpl_name) extends DTO {
                function __construct($proj_id,$tmpl_name) {
                    $this->setParm(':proj_id',$proj_id);
                    $this->setParm(':tmpl_name',$tmpl_name);
                }
                public function getInsertSQL() {
                    return 'INSERT INTO templates(proj_id,tmpl_name) VALUES (:proj_id,:tmpl_name);';
                }
            };
            if ($this->con[0]->insert($dto)) {
                $newid = $this->con[0]->execSQL('select last_insert_rowid() AS id');
                $id = $newid[0]['id'];
                return $id;
            }
            return false;
        } catch (PDOException $e){
            die('PDOException throwen:'. $e->getMessage());
        }
    }

    /**
    * 
    */
    public function getAllTemplates() {
        try {
            return $this->con[0]->fetchAll('templates');
        } catch (PDOException $e){
            die('PDOException throwen:'. $e->getMessage());
        }
    }

    /**
    * 
    */
    public function getTemplates($proj_id) {
        try {
            $tmpl_list = array();
            $tmpls = $this->con[0]->findByKey('templates', 'proj_id', $proj_id);
            foreach ($tmpls as $row) {
                $tmpl_list[$row['id']] = $row['tmpl_name'];
            }
            return $tmpl_list;
        } catch (PDOException $e){
            die('PDOException throwen:'. $e->getMessage());
        }
    }

    /**
    * 
    */
    public function listTables($proj_id) {
        try {
            $table_list = $this->con[$proj_id]->findByKey('sqlite_master','type','table','name');
            $table_list = array_merge($table_list, $this->con[$proj_id]->findByKey('sqlite_master','type','view','name'));
            if ($table_list) {
                $table_list = call_user_func_array('array_map',array_merge(array(null),$table_list));
                $table_list = array_diff(
                    call_user_func_array('array_merge',$table_list),
                    array('sqlite_sequence', ACTUAL_TBL, ACTUAL_COL)
                );
            }            
            return array_values($table_list);
        } catch (PDOException $e){
            die('PDOException throwen:'. $e->getMessage());
        }
    }

    /**
    * 
    */
    public function addUserTable($proj_id, $tbl_name, $cols_hash, $constraints_hash = array()) {
        try {
            //トランザクション処理必要
            $actual_tbl_name = fnv132($tbl_name);
            $actual_col_name = array();
            $tdo = new TableDefineObject($actual_tbl_name);
            foreach ($cols_hash as $col_name => $settings) {
                $actual_name = fnv132($col_name);
                $tdo->appendColumn($actual_name, $settings['type'], $settings['constraints']);
                $actual_col_name[$col_name] = $actual_name;
            }
            foreach ($constraintss_hash as $con_name => $settings) {
                $targets = $settings['targets'];
                foreach ($targets as $key => $value) {
                    $targets[$key] = fnv132($value);
                }
                $references = array();
                foreach ($settings['references'] as $tbl => $cols) {
                    $actual_cols = array();
                    foreach ($cols as $col) {
                        $actual_cols[] = fnv132($col);
                    }
                    $references[fnv132($tbl)] = $actual_cols;
                }
                $tdo->appendConstraint($con_name, $targets, $references);
            }
            $this->con[$proj_id]->createTable($tdo);
            // システムテーブルに表名と列名を登録
            $tbl_id = $this->insertData($proj_id, ACTUAL_TBL,
                ['tbl_name' => $tbl_name, 'actual_name' => $actual_tbl_name]);
            foreach ($actual_col_name as $col_name => $actual_name) {
                $this->insertData($proj_id, ACTUAL_COL,
                    ['tbl_id' => $tbl_id, 'col_name' => $col_name, 'actual_name' => $actual_name]);
            }
            return;
        } catch (PDOException $e){
            die('PDOException throwen:'. $e->getMessage());
        }
    }

    /**
    * 
    */
    public function listData($proj_id, $table) {
        try {
            return $this->con[$proj_id]->fetchAll($table);
        } catch (PDOException $e){
            die('PDOException throwen:'. $e->getMessage());
        }
    }

    /**
    * 
    */
    public function insertData($proj_id, $table, $parms) {
        $dto = new class($key, $colname, $parms[$key]) extends DTO {
            private $table = '';
            private $colname = '';
            function __construct($key, $colname, $parm) {
                $this->table = $key . 's';
                $this->colname = $colname;
                $this->setParm(':'.$colname, $parm);
            }
            public function getInsertSQL() {
                return 'INSERT INTO '.$this->table.'('.$this->colname.') VALUES (:'.$this->colname.');';
            }
            public function getTableName(){ return $this->table; }
        };
        try {
            //トランザクション処理必要
            if ($this->con[$proj_id]->insert($dto)) {
                $newid = $this->con[$proj_id]->execSQL('select last_insert_rowid() AS id');
                $id = $newid[0]['id'];
            } else {
                $row = $this->con[$proj_id]->findByKey(
                    $dto->getTableName(), $colname, $parms[$key]);
                $id = $row[0]['id'];
            }
        } catch (PDOException $e){
            die('PDOException throwen:'. $e->getMessage());
        }
    }

    /**
    * 現状ではSNTRPG_Skills専用
    */
    public function insertSNRPGData($proj_id, $parms) {
        if ($parms['preconditions'] == '-') {
            $parms['has_preconditions'] = 0;
        } else {
            $parms['has_preconditions'] = 1;
            $preconditions = $parms['preconditions'];
        }
        unset($parms['preconditions']);
        $foreign_keys = array('timing' => false, 'target' => false,
            'renge' => false,'icon' => 'file_name');
        try {
            // 外部キーの要素を登録し、idに変換
            foreach ($foreign_keys as $key => $colname) {
                $colname = $colname ? $colname : $key ;
                $dto = new class($key,$colname,$parms[$key]) extends DTO {
                    private $table = '';
                    private $colname = '';
                    function __construct($key,$colname,$parm) {
                        $this->table = $key . 's';
                        $this->colname = $colname;
                        $this->setParm(':'.$colname,$parm);
                    }
                    public function getInsertSQL() {
                        return 'INSERT INTO '.$this->table.'('.$this->colname.') VALUES (:'.$this->colname.');';
                    }
                    public function getTableName(){return $this->table;}
                };
                if ($this->con[$proj_id]->insert($dto)) {
                    $newid = $this->con[$proj_id]->execSQL('select last_insert_rowid() AS id');
                    $id = $newid[0]['id'];
                } else {
                    $row = $this->con[$proj_id]->findByKey(
                        $dto->getTableName(),$colname,$parms[$key]);
                    $id = $row[0]['id'];
                }
                $parms[$key] = $id;
            }
            // スキルデータ本体を登録
            $dto = new class($parms) extends DTO {
                private $table = 'skills';
                private $colname = '';
                function __construct($parm) {
                    $this->colname = array_keys($parm);
                    foreach ($parm as $key => $value) {
                        $this->setParm(':'.$key,$value);
                    }
                }
                public function getInsertSQL() {
                    $columns = implode(',',$this->colname);
                    $places = array();
                    foreach ($this->colname as $column) {
                        $places[] = ':'.$column;
                    }
                    $places = implode(',',$places);
                    return 'INSERT INTO '.$this->table.'('.$columns.') VALUES ('.$places.');';
                }
                public function getTableName(){return $this->table;}
            };
            if ($this->con[$proj_id]->insert($dto)) {
                $newid = $this->con[$proj_id]->execSQL('select last_insert_rowid() AS id');
                $skill_id = $newid[0]['id'];
            } else {
                $row = $this->con[$proj_id]->findByKey(
                    'skills','name',$parms['name']);
                $skill_id = $row[0]['id'];
            }
            // 習得前提条件が存在する場合
            if ($parms['has_preconditions']) {
                $conditions = explode(',',$preconditions);
                $preconditions = array();
                // 個々の条件を登録
                foreach ($conditions as $condition) {
                    $dto = new class($condition) extends DTO {
                        function __construct($parm) {
                            $this->setParm(':condition',$parm);
                        }
                        public function getInsertSQL() {
                            return 'INSERT INTO conditions(condition) VALUES (:condition);';
                        }
                    };
                    if ($this->con[$proj_id]->insert($dto)) {
                        $newid = $this->con[$proj_id]->execSQL('select last_insert_rowid() AS id');
                        $id = $newid[0]['id'];
                    } else {
                        $row = $this->con[$proj_id]->findByKey(
                            'conditions','condition',$condition);
                        $id = $row[0]['id'];
                    }
                    $preconditions[] = array($skill_id,$id);
                }
                // 各条件をスキルデータと関連付け
                foreach ($preconditions as $pair) {
                    $dto = new class($pair) extends DTO {
                        function __construct($parm) {
                            $this->setParm(':skill_id',$parm[0]);
                            $this->setParm(':condition_id',$parm[1]);
                        }
                        public function getInsertSQL() {
                            return 'INSERT INTO preconditions(skill_id,condition_id) VALUES (:skill_id,:condition_id);';
                        }
                    };
                    $this->con[$proj_id]->insert($dto);
                }
            }
            return;
        } catch (PDOException $e){
            die('PDOException throwen:'. $e->getMessage());
        }
    }

    /**
    * 
    */
    private function setupRootDB() {
        try {
            // project table
            $tdo = new TableDefineObject('projects');
            $tdo->appendColumn('proj_name', 'text', ['uniq' => true, 'not_null' => true]);
            $this->con[0]->createTable($tdo, true);
            // templates table
            $tdo = new TableDefineObject('templates');
            $tdo->appendColumn('proj_id', 'integer', ['foreign' => ['ref' => 'projects(id)', 'del' => 'null']]);
            $tdo->appendColumn('tmpl_name', 'text', ['not_null' => true]);
            $tdo->appendConstraint('uniq', ['proj_id', 'tmpl_name']);
            $this->con[0]->createTable($tdo, true);
        } catch (PDOException $e){
            die('PDOException throwen:'. $e->getMessage());
        }
    }

    /**
    * 
    */
    public static function getDateTypeList() {
        return SQLiteHandler::$DATA_TYPES;
    }


}

?>