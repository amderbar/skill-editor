<?php
/**
 * session check.
 */
require_once($_SERVER['DOCUMENT_ROOT'].'/skill_editor/common.inc');

/**
 * 
 */
require_once(full_path('controllers/servlet.inc'));

/**
 * 
 */
class EditorServlet extends Servlet {

    /**
     * 
     */
    public function doGet($req='') {
        $REQ_SCOPE = array();
            if($this->db_editor->open($current_proj_id) === false){
                return $this->redirect($_SERVER['PHP_SELF']);
            }
            $REQ_SCOPE['proj_id'] = $current_proj_id;
            $table_list = $this->db_editor->listUsrTables($current_proj_id);
            $REQ_SCOPE['tbl_list'] = $table_list;
            if (isset($_GET['tab'])) {
                $opened_tab = ($_GET['tab'] == 'new') ? 'new' : intval($_GET['tab']);
            } else {
                $opened_tab = 0;
            }
            if (empty($table_list) || ($opened_tab === 'new')) {
            // テーブル新規作成タブを開く
                $REQ_SCOPE['new_tab'] = true;
                $REQ_SCOPE['data_types'] = DBEditor::getDateTypeList();
                $REQ_SCOPE['form_types'] = HTMLHandler::$FROM_TYPES;
                $REQ_SCOPE['opened_tab'] = null;
                $REQ_SCOPE['proj_name'] = $this->db_editor->projName($current_proj_id);
                $tmpl_list = $this->db_editor->getAllTemplates();
                foreach ($table_list as $tbl_num => $tbl_name) {
                    $REQ_SCOPE['col_list'][$tbl_name] = array();
                    $col_list = $this->db_editor->listColumns($current_proj_id, $tbl_num);
                    }
                }
                $REQ_SCOPE['tbl_tmpls'] = ['default' => full_path('resources/templates/default_template.php')];
                foreach ($tmpl_list as $tmpl) {
                    $REQ_SCOPE['tbl_tmpls'][$tmpl['tmpl_name']]
                     = full_path(sprintf('resources/templates/proj%03d/', $tmpl['proj_id']).$tmpl['tmpl_name']);
                }
                $REQ_SCOPE['tbl_data'] = array(
                    ['id' => 1, 'Nameless' => ''],
                    ['id' => 2, 'Nameless' => ''],
                    ['id' => 3, 'Nameless' => '']
                );
            } else {
            // 既存テーブルのタブを開く
                $teble_data = $this->db_editor->listData($current_proj_id, $table_list[$opened_tab]);
                $tmpl_list = $this->db_editor->getTemplates($current_proj_id);
                // 先頭要素のキーを取得
                $selected_tmpl = key($tmpl_list);
                // リクエストスコープ相当の配列にデータを格納
                $REQ_SCOPE['tbl_data'] = $teble_data;
                $REQ_SCOPE['tbl_tmpl'] = ($table_list[$opened_tab] == 'skills_view') ?
                    full_path(sprintf('resources/templates/proj%03d/', $current_proj_id).$tmpl_list[$selected_tmpl]) :
                    full_path('resources/templates/default_template.php');
                $REQ_SCOPE['opened_tab'] = $opened_tab;
                // 肝心のデータはセッションスコープにも入れておく
                $_SESSION['proj'.$current_proj_id][$opened_tab] = $teble_data;
            }
        }
        return $this->foward('view/editor_pain.inc', $REQ_SCOPE);
    }

    /**
     * 
     */
    public function doPost($req='') {
            // 本当はここで入力値チェックをする
            // $_POST
            // self::$db_editor->addUserTable($proj_id, $_POST['tbl_name'], $prop_hash);
        }
    }
            // $this->db_editor->addUserTable($proj_id, $tbl_name, $def_tbl);
        }
        return $this->foward('view/editor_pain.inc', $REQ_SCOPE);
        // return $this->redirect($_SERVER["REQUEST_URI"]);
    }
}

?>