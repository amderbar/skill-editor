<?php
/**
 *
 * @author amderbar
 *
 */
class TopProcess extends Process {

    /**
     * TODO: 列の表示並び順を保存できるようにする
     *
     * @param string $proj_name
     * @return int
     */
    public function registerDB(string $proj_name) :int
    {
        try {
            $proj_id = $this->open(ROOT_DB_ID)->insert('projects', [ 'proj_name' => $proj_name ], true);

            $tmpl_dir = full_path( sprintf( RESOURCE_ROOT . '/templates/proj%03d', $proj_id ), true );
            if (! file_exists( $tmpl_dir )) {
                mkdir( $tmpl_dir, 0666 );
            }
            $db_con = $this->open( $proj_id, true );

            // system_admin tables
            $tdo = (new TableDefineObject( SYSTEM_TBL ))
                ->appendColumn( 'tbl_name', 'text', [ 'not_null' => true ] )
                ->appendColumn( 'actual_name', 'text', [ 'uniq' => true, 'not_null' => true ] );
            $db_con->createTable( $tdo, true );

            $tdo = (new TableDefineObject( SYSTEM_COL ))
                ->appendColumn( 'tbl_id', 'integer', [
                    'foreign' => [ 'ref' => SYSTEM_TBL . '(id)', 'del' => 'cascade' ]
                ] )
                ->appendColumn( 'col_name', 'text', [ 'not_null' => true ] )
                ->appendColumn( 'actual_name', 'text', [ 'not_null' => true ] )
                ->appendColumn( 'default_val', 'blob')
                ->appendColumn( 'ref_col', 'integer', [
                    'foreign' => [ 'ref' => SYSTEM_COL . '(id)', 'del' => 'null' ]
                ] )
                ->appendColumn( 'form_type', 'integer', [ 'not_null' => true ] )
                ->appendColumn( 'multiple', 'boolean', [ 'not_null' => true, 'default' => false ] )
                ->appendConstraint( 'uniq', [ 'tbl_id', 'actual_name' ] );
            $db_con->createTable( $tdo, true );

            $tdo = (new TableDefineObject( NUM_SETTINGS ))
                ->appendColumn( 'col_id', 'integer', [
                    'foreign' => [ 'ref' => SYSTEM_COL . '(id)', 'del' => 'cascade' ]
                ] )
                ->appendColumn( 'step', 'real' )
                ->appendColumn( 'max', 'real' )
                ->appendColumn( 'min', 'real' );
            $db_con->createTable( $tdo, true );

            return $proj_id;
        } catch ( PDOException | DBManageExeption $e ) {
            $this->handleException( $e );
        }
    }

    /**
     *
     * @param int $proj_id
     */
    public function deleteDB(int $proj_id)
    {
        // FIXME: トランザクション処理必要
        try {
            $this->close( $proj_id );
            $this->open(ROOT_DB_ID)->delete( 'projects', [ 'id' => $proj_id ] );
            if ($tmpl_dir = full_path( sprintf(  RESOURCE_ROOT . '/templates/proj%03d', $proj_id ) )) {
                if ($handle = opendir( $tmpl_dir )) {
                    while ( false !== ($tmpl = readdir( $handle )) ) {
                        if ($tmpl != "." && $tmpl != "..") {
                            unlink( $tmpl );
                        }
                    }
                    rmdir( $tmpl_dir );
                }
                closedir( $handle );
            }
            if ($db_file = full_path( sprintf(  RESOURCE_ROOT . '/proj%03d.db', $proj_id ) )) {
                unlink( $db_file );
            }
        } catch ( PDOException | DBManageExeption $e ) {
            $this->handleException( $e );
        }
    }

    /**
     *
     * @param int $proj_id
     * @param string $tmpl_name
     * @return int
     */
    public function registerTemplate(int $proj_id, string $tmpl_name):int
    {
        try {
            return $this->open(ROOT_DB_ID)
                ->insert( 'templates', [
                        'proj_id' => $proj_id,
                        'tmpl_name' => $tmpl_name
                ] );
        } catch ( PDOException | DBManageExeption $e ) {
            $this->handleException( $e );
        }
    }

    /**
     *
     * @param int $proj_id
     */
    public function saveData(int $proj_id)
    {
        // POSTデータに対するバリデーションが必要
        $_POST['save'] = null;
        $column_map = array_keys($_POST);
        array_shift($column_map);
        // pre_dump($column_map);
        $new_data = array();
        $new_data['skills_view'] = call_user_func_array('array_map',$_POST);
        // pre_dump($new_data['skills_view']);
        $old_data = $_SESSION['proj'.$proj_id];
        // 既存データの配列を平滑化
        // さらにデータをキー、pidを値とする連想配列化
        foreach ($old_data as $tbl_name => $tbl_data) {
            if ($tbl_name == 'skills') {
                continue;
            }
            $tbl_data = call_user_func_array('array_map',array_merge(array(null),$tbl_data));
            $keys = array_keys($tbl_data);
            if (array_depth($tbl_data) > 1) {
                foreach ($tbl_data[1] as $num => $value) {
                    $tbl_data[$value] = $tbl_data[0][$num];
                }
            } else {
                $tbl_data[$tbl_data[$keys[1]]] = $tbl_data[$keys[0]];
            }
            foreach ($keys as $key) {
                unset($tbl_data[$key]);
            }
            $old_data[$tbl_name] = $tbl_data;
        }
        // pre_dump($old_data['skills']);
        // 親表に関して、新データと既存データの差分を取得
        $table_list = array_keys($old_data);
        foreach ($column_map as $column_name) {
            $tbl_name = $column_name . 's';
            if ($column_name == 'preconditions') {
                $tbl_name = 'conditions';
            }
            if (isset($old_data[$tbl_name])) {
                $new_data[$tbl_name] = array_diff($_POST[$column_name],array_keys($old_data[$tbl_name]));
            }
        }
        // conditions
        foreach ($new_data['conditions'] as $num => $condition) {
            if ($condition == '-') {
                unset($new_data['conditions'][$num]);
            }
        }
        // skills
        $new_data['skills'] = array();
        foreach ($new_data['skills_view'] as $num => $skill) {
            foreach ($skill as $key => $value) {
                $column_name = $column_map[$key];
                if ($column_name == 'preconditions') {
                    $tbl_name = 'conditions';
                    $new_skill['has_preconditions'] =  ($value == '-') ? '0' : '1';
                    unset($new_data['conditions'][$num]);
                    $new_skill['icon'] = '1';
                } else {
                    $tbl_name = $column_name . 's';
                    if (isset($old_data[$tbl_name])) {
                        $new_skill[$column_name] = $old_data[$tbl_name][$value];
                    } else {
                        $new_skill[$column_name] = preg_replace("/\r\n|\r|\n/", "\n", trim($value));
                    }
                }
            }
            $new_skill['list_order'] = strval($num);
            $new_data['skills'][$num] = $new_skill;
        }
        unset($new_data['skills_view']);
        $new_data['skills'] = array_diff_assoc_recursive($new_data['skills'],$old_data['skills']);
        pre_dump($new_data);
    }
}