<?php
require_once(full_path('models/db_manager.php'));

/**
 * 
 */
class DBEditor {
    private $con = null;

    function __construct() {
        $this->con = array();
        $this->con[ROOT_DB] = new SQLiteHandler(ROOT_DB,true);
        $this->setupRootDB();
    }

    /**
    * 
    */
    public function open($db_file,$proj_name = null) {
        $proj_name = ($proj_name) ? $proj_name : $db_file ;
        $this->con[$proj_name] = new SQLiteHandler($db_file,true);
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
            $proj_list = array();
            foreach ($this->con[ROOT_DB]->fetchAll('projects') as $row) {
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
            $dto = new class($proj_name) extends DTO {
                private $table = 'projects';
                function __construct($proj_name) {
                    $this->setParm(':proj_name',$proj_name);
                }
                public function getInsertSQL() {
                    return 'INSERT INTO '.$this->table.'(proj_name) VALUES (:proj_name)';
                }
            };
            if ($this->con[ROOT_DB]->insert($dto)) {
                $newid = $this->con[ROOT_DB]->execSQL('select last_insert_rowid() AS id');
                $id = $newid[0]['id'];
            } else {
                $row = $this->con[ROOT_DB]->findByKey(
                    'projects','proj_name',$proj_name);
                $id = $row[0]['id'];
            }
            $tmpl_dir = full_path(sprintf('view/templates/proj%03d',$id),true);
            if(! file_exists($tmpl_dir)){
                mkdir($tmpl_dir, 0666);
            }
            $db_file = sprintf('proj%03d.db',$id);
            $this->open($db_file,$proj_name);
            return $id;
        } catch (PDOException $e){
            die('PDOException throwen:'. $e->getMessage());
        }
    }

    public function registerTemplate($proj_id,$tmpl_name) {
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
            if ($this->con[ROOT_DB]->insert($dto)) {
                $newid = $this->con[ROOT_DB]->execSQL('select last_insert_rowid() AS id');
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
    public function dropRoot() {
        $this->con[ROOT_DB]->execSQL('drop table templates;');
        $this->con[ROOT_DB]->execSQL('drop table projects;');
    }

    /**
    * 
    */
    public function getTemplates($proj_id) {
        try {
            $tmpl_list = array();
            foreach ($this->con[ROOT_DB]->findByKey('templates','proj_id',$proj_id) as $row) {
                $tmpl_list[] = $row['tmpl_name'];
            }
            return $tmpl_list;
        } catch (PDOException $e){
            die('PDOException throwen:'. $e->getMessage());
        }
    }

    /**
    * 
    */
    public function listTables($proj_name) {
        try {
            $table_list = $this->con[$proj_name]->findByKey('sqlite_master','type','table','name');
            $table_list = array_merge($table_list,$this->con[$proj_name]->findByKey('sqlite_master','type','view','name'));
            $table_list = call_user_func_array('array_map',array_merge(array(null),$table_list));
            $table_list = array_diff(call_user_func_array ('array_merge',$table_list), array('sqlite_sequence'));
            return array_values($table_list);
        } catch (PDOException $e){
            die('PDOException throwen:'. $e->getMessage());
        }
    }

    /**
    * 
    */
    public function listData($proj_name,$table) {
        try {
            return $this->con[$proj_name]->fetchAll($table);
        } catch (PDOException $e){
            die('PDOException throwen:'. $e->getMessage());
        }
    }

    /**
    * 現状ではSNTRPG_Skills専用
    */
    public function insertData($proj_name,$parms) {
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
                if ($this->con[$proj_name]->insert($dto)) {
                    $newid = $this->con[$proj_name]->execSQL('select last_insert_rowid() AS id');
                    $id = $newid[0]['id'];
                } else {
                    $row = $this->con[$proj_name]->findByKey(
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
            if ($this->con[$proj_name]->insert($dto)) {
                $newid = $this->con[$proj_name]->execSQL('select last_insert_rowid() AS id');
                $skill_id = $newid[0]['id'];
            } else {
                $row = $this->con[$proj_name]->findByKey(
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
                    if ($this->con[$proj_name]->insert($dto)) {
                        $newid = $this->con[$proj_name]->execSQL('select last_insert_rowid() AS id');
                        $id = $newid[0]['id'];
                    } else {
                        $row = $this->con[$proj_name]->findByKey(
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
                    $this->con[$proj_name]->insert($dto);
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
        $sql = <<< _SQL_
CREATE TABLE IF NOT EXISTS projects (
    id integer PRIMARY KEY AUTOINCREMENT,
    proj_name text NOT NULL UNIQUE
);
_SQL_;
        $this->con[ROOT_DB]->execSQL($sql);
        $sql = <<< _SQL_
CREATE TABLE IF NOT EXISTS templates (
    id integer PRIMARY KEY AUTOINCREMENT,
    proj_id integer REFERENCES projects(id) on DELETE SET NULL,
    tmpl_name text NOT NULL,
    UNIQUE(proj_id, tmpl_name)
);
_SQL_;
        $this->con[ROOT_DB]->execSQL($sql);
    }
}

?>