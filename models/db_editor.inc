<?php
/**
* session check.
*/
require_once($_SERVER['DOCUMENT_ROOT'].'/skill_editor/common.inc');

require_once(full_path('models/db_manager.inc'));
define('ROOT_DB', 'resources/system_admin.db');
define('SYSTEM_TBL', 's_admin_tbl');
define('SYSTEM_COL', 's_admin_col');
define('NUM_SETTINGS', 's_num_settings');

/**
 * 
 */
class DBEditor {
    private $con = array();
    private $form_id = array();

    function __construct() {
        $this->open(0);
        $this->setupSystemDB();
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
        $row = $this->con[0]->findByKey('projects', 'id', $proj_id, ['proj_name']);
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
            $proj_id = $this->insertData(0, 'projects', ['proj_name' => $proj_name], true);
            $tmpl_dir = full_path(sprintf('resources/templates/proj%03d', $proj_id), true);
            if(!file_exists($tmpl_dir)){
                mkdir($tmpl_dir, 0666);
            }
            $db_con = $this->open($proj_id, true);
            // system_admin tables
            $tdo = new TableDefineObject(SYSTEM_TBL);
            $tdo->appendColumn('tbl_name', 'text', ['uniq' => true, 'not_null' => true]);
            $tdo->appendColumn('actual_name', 'text', ['uniq' => true, 'not_null' => true]);
            $db_con->createTable($tdo, true);
            $tdo = new TableDefineObject(SYSTEM_COL);
            $tdo->appendColumn('tbl_id', 'integer', ['foreign' => ['ref' => SYSTEM_TBL.'(id)', 'del' => 'cascade']]);
            $tdo->appendColumn('col_name', 'text', ['not_null' => true]);
            $tdo->appendColumn('actual_name', 'text', ['not_null' => true]);
            $tdo->appendColumn('ref_col', 'integer', ['foreign' => ['ref' => SYSTEM_COL.'(id)', 'del' => 'null']]);
            $tdo->appendColumn('form_type', 'integer', ['not_null' => true]);
            $tdo->appendColumn('multiple', 'boolean', ['not_null' => true, 'default' => false]);
            $db_con->createTable($tdo, true);
            $tdo = new TableDefineObject(NUM_SETTINGS);
            $tdo->appendColumn('col_id', 'integer', ['foreign' => ['ref' => SYSTEM_COL.'(id)', 'del' => 'cascade']]);
            $tdo->appendColumn('step', 'real');
            $tdo->appendColumn('max', 'real');
            $tdo->appendColumn('min', 'real');
            $db_con->createTable($tdo, true);
            return $proj_id;
        } catch (PDOException $e){
            error_log($e->getMessage());
            die();
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
                    if ($tmpl != "." && $tmpl != "..") {
                        unlink($tmpl);
                    }
                }
                rmdir($tmpl_dir);
            }
            closedir($handle);
        }
        if ($db_file = full_path(sprintf('resources/proj%03d.db', $proj_id))) {
            unlink($db_file);
        }
    }

    /**
    * 
    */
    public function registerTemplate($proj_id, $tmpl_name) {
        try {
            $dto = new class($proj_id, $tmpl_name) extends DTO {
                function __construct($proj_id, $tmpl_name) {
                    $this->setParm(':proj_id', $proj_id);
                    $this->setParm(':tmpl_name', $tmpl_name);
                }
                public function getInsertSQL() {
                    return 'INSERT INTO templates(proj_id, tmpl_name) VALUES (:proj_id, :tmpl_name);';
                }
            };
            if ($this->con[0]->insert($dto)) {
                $tmpl_id = $this->con[0]->execSQL('select last_insert_rowid() AS id');
                $tmpl_id = $tmpl_id[0]['id'];
                return $tmpl_id;
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
    public function listUsrTables($proj_id) {
        try {
            $table_list = array();
            foreach ($this->con[$proj_id]->fetchAll(SYSTEM_TBL, 'id, tbl_name') as $row) {
                $table_list[$row['id']] = $row['tbl_name'];
            }
            return $table_list;
        } catch (PDOException $e){
            die('PDOException throwen:'. $e->getMessage());
        }
    }

    /**
    * 
    */
    public function listColumns($proj_id, $tbl_id) {
        try {
            return $this->con[$proj_id]->findByKey(SYSTEM_COL, 'tbl_id', $tbl_id, ['id', 'col_name']);
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
            // システムテーブルに表名を登録
            $actual_tbl_name = 'u_'.fnv132($tbl_name);
            $tbl_id = $this->insertData($proj_id, SYSTEM_TBL,
                ['tbl_name' => $tbl_name, 'actual_name' => $actual_tbl_name]);
            // システムテーブルに列名を登録
            $actual_col_name = array();
            $i = 0;
            foreach ($cols_hash as $def_hash) {
                $i++;
                $actual_name = 'c_'.fnv132($def_hash['col_name']);
                $form_settings = array(
                    'tbl_id' => $tbl_id,
                    'col_name' => $def_hash['col_name'],
                    'actual_name' => $actual_name,
                    'form_type' => $this->form_id[$def_hash['form_type']],
                    'multiple' => isset($def_hash['multiple'])
                );
                $col_id = $this->insertData($proj_id, SYSTEM_COL, $form_settings);
                if (isset($def_hash['step']) && isset($def_hash['max']) && isset($def_hash['min'])) {
                    $this->insertData($proj_id, NUM_SETTINGS, [
                        'col_id' => $col_id,
                        'step' => $def_hash['step'],
                        'max' => $def_hash['max'],
                        'min' => $def_hash['min']
                    ]);
                }
                $actual_col_name[$i] = ['col_id' => $col_id, 'actural' => $actual_name];
            }
            $i = 0;
            foreach ($cols_hash as $def_hash) {
                $i++;
                if (isset($def_hash['ref_dist'])) {
                    if ($def_hash['ref_dist'] < 0) {
                        $def_hash['ref_dist'] = $actual_col_name[-$def_hash['ref_dist']]['col_id'];
                    }
                    $this->updateData($proj_id, SYSTEM_COL,
                        ['ref_col' => $def_hash['ref_dist']],
                        ['id' => $actual_col_name[$i]['col_id']]
                    );
                }
            }
            // テーブル作成
            $tdo = new TableDefineObject($actual_tbl_name);
            $i = 0;
            foreach ($cols_hash as $def_hash) {
                $i++;
                switch ($def_hash['form_type']) {
                    case 'color':
                    case 'text':
                    case 'textarea':
                    case 'tel':
                    case 'url':
                    case 'email':
                    case 'password':
                    case 'datetime':
                        $data_type = 'TEXT';
                        break;

                    case 'date':
                    case 'month':
                    case 'week':
                    case 'time':
                    case 'datetime-local':
                        $data_type = 'DATETIME';
                        break;

                    case 'listext':
                    case 'number':
                    case 'numlist':
                    case 'range':
                    case 'select':
                    case 'multicheck':
                    case 'radio':
                        $data_type = 'NUMERIC';
                        break;

                    case 'checkbox':
                        $data_type = 'BOOLEAN';
                        break;

                    case 'file':
                    case 'image':
                        $data_type = 'BLOB';
                        break;
                }
                $col_constraints = array();
                $col_constraints['default'] = ($def_hash['default']) ? $def_hash['default'] : null;
                $col_constraints['not_null'] = isset($def_hash['not_null']);
                $col_constraints['uniq'] = isset($def_hash['uniq']);
                $tdo->appendColumn($actual_col_name[$i]['actural'], $data_type, $col_constraints);
            }
            // foreach ($constraints_hash as $def_hash) {
            //     $targets = $def_hash['targets'];
            //     foreach ($targets as $key => $value) {
            //         $targets[$key] = fnv132($value);
            //     }
            //     $references = array();
            //     foreach ($def_hash['references'] as $tbl => $cols) {
            //         $SYSTEM_COLs = array();
            //         foreach ($cols as $col) {
            //             $SYSTEM_COLs[] = fnv132($col);
            //         }
            //         $references[fnv132($tbl)] = $SYSTEM_COLs;
            //     }
            //     $tdo->appendConstraint($con_name, $targets, $references);
            // }
            $this->con[$proj_id]->createTable($tdo);
            return $tbl_id;
        } catch (PDOException $e){
            die('PDOException throwen:'. $e->getMessage());
        }
    }

    /**
    * 
    */
    public function listData($proj_id, $tbl_id) {
        try {
            $tmp_table = $this->con[$proj_id]->findByKey(SYSTEM_TBL, 'id', $tbl_id, ['actual_name']);
            $tmp_table = $this->con[$proj_id]->fetchAll($tmp_table[0]['actual_name']);
            $list_data = array();
            $col_names = array();
            foreach ($tmp_table as $row) {
                $tmp_row = array();
                foreach ($row as $col => $value) {
                    if ($col == 'id') {
                        $col_names[$col] = 'rid';
                    } elseif (!isset($col_names[$col])) {
                        $tmp = $this->con[$proj_id]->findByKey(SYSTEM_COL, 'actual_name', $col, ['col_name']);
                        error_log(var_export($col, true));
                        error_log(var_export($tmp, true));
                        $col_names[$col] = $tmp[0]['col_name'];
                    }
                    $tmp_row[$col_names[$col]] = $value;
                }
                $list_data[] = $tmp_row;
            }
            $tmp_row = array();
            foreach ($this->listColumns($proj_id, $tbl_id) as $col) {
                $tmp_row[$col['col_name']] = null;
            };
            $list_data[] = $tmp_row;
            return $list_data;
        } catch (PDOException $e){
            die('PDOException throwen:'. $e->getMessage());
        }
    }

    /**
    * 
    */
    public function insertData($proj_id, $table, $parms, $if_redundant = false) {
        $dto = new class($table, $parms) extends DTO {
            private $table = '';
            function __construct($table, $parms) {
                $this->table = $table;
                foreach ($parms as $colname => $value) {
                    $this->setParm(':'.$colname, $value);
                }
            }
            public function getTableName(){ return $this->table; }
        };
        try {
            //トランザクション処理必要
            return $this->con[$proj_id]->insert($dto, $if_redundant);
        } catch (PDOException $e){
            error_log($e->getMessage());
            die();
        }
    }

    /**
    * 
    */
    public function updateData($proj_id, $table, $parms, $conditions = null) {
        $dto = new class($table, $parms) extends DTO {
            private $table = '';
            function __construct($table, $parms) {
                $this->table = $table;
                foreach ($parms as $colname => $value) {
                    $this->setParm(':'.$colname, $value);
                }
            }
            public function getTableName(){ return $this->table; }
        };
        try {
            //トランザクション処理必要
            return $this->con[$proj_id]->update($dto, $conditions);
        } catch (PDOException $e){
            error_log($e->getMessage());
            die();
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
            // 外部キーの要素を登録し、rowidに変換
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
                    $row_id = $this->con[$proj_id]->execSQL('select last_insert_rowid() AS id');
                    $row_id = $row_id[0]['id'];
                } else {
                    $row = $this->con[$proj_id]->findByKey(
                        $dto->getTableName(), $colname, $parms[$key], ['id']);
                    $row_id = $row[0]['id'];
                }
                $parms[$key] = $row_id;
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
                $row_id = $this->con[$proj_id]->execSQL('select last_insert_rowid() AS id');
                $skill_id = $row_id[0]['id'];
            } else {
                $row = $this->con[$proj_id]->findByKey(
                    'skills', 'name', $parms['name'], ['id']);
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
                        $row_id = $this->con[$proj_id]->execSQL('select last_insert_rowid() AS id');
                        $row_id = $row_id[0]['id'];
                    } else {
                        $row = $this->con[$proj_id]->findByKey(
                            'conditions', 'condition', $condition, ['id']);
                        $row_id = $row[0]['id'];
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
    private function setupSystemDB() {
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
            // users table
            $tdo = new TableDefineObject('users');
            $tdo->appendColumn('name', 'text', ['not_null' => true]);
            $tdo->appendColumn('passwd', 'text', ['not_null' => true]);
            $this->con[0]->createTable($tdo, true);
            // form_type master table
            $tdo = new TableDefineObject('form_master');
            $tdo->appendColumn('type', 'text', ['uniq' => true, 'not_null' => true]);
            $this->con[0]->createTable($tdo, true);
            foreach (HTMLHandler::$FROM_TYPES as $type => $txt) {
                $this->form_id[$type] = $this->insertData(0, 'form_master', ['type' => $type], true);
            }
        } catch (PDOException $e){
            die('PDOException throwen:'. $e->getMessage());
        }
    }

    /**
    * 
    */
    public function convertForm2DTO($cols_hash, $constraints_hash = array()) {
        # code...
    }

    /**
    * 
    */
    public static function getDataTypeList() {
        return SQLiteHandler::$DATA_TYPES;
    }


}

?>