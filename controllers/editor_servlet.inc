<?php
/**
 * session check.
 */
require_once($_SERVER['DOCUMENT_ROOT'].'/skill_editor/common.inc');

/**
 * 
 */
require_once(full_path('controllers/servlet.inc'));
require_once(full_path('models/db_editor.inc'));

/**
 * 
 */
class EditorServlet extends Servlet {
    /**  */
    private static $db_editor = null;

    /**
     * 
     */
    public static function doGet($req='') {
        $REQ_SCOPE = array();
        // リクエストパラメータidがセットされている時、そのidのプロジェクトが開かれる
        if (!empty($_GET['id'])) {
            $current_proj_id = intval($_GET['id']);
            if(self::$db_editor->open($current_proj_id) === false){
                return self::redirect($_SERVER['PHP_SELF']);
            }
            $REQ_SCOPE['proj_id'] = $current_proj_id;
            $table_list = self::$db_editor->listTables($current_proj_id);
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
                $REQ_SCOPE['proj_name'] = self::$db_editor->projName($current_proj_id);
                $tmpl_list = self::$db_editor->getAllTemplates();
                $REQ_SCOPE['col_list'] = array('This Table' => ['this.0' => '']);
                // モック生成コード
                foreach ($table_list as $tbl_num => $tbl_name) {
                    $REQ_SCOPE['col_list'][$tbl_name] = array();
                    for ($i=0; $i < 4; $i++) { // 適当に列があることにする
                        $REQ_SCOPE['col_list'][$tbl_name][$tbl_num.','.$i] = 'column'.$i;
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
                $teble_data = self::$db_editor->listData($current_proj_id, $table_list[$opened_tab]);
                $tmpl_list = self::$db_editor->getTemplates($current_proj_id);
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
        return self::foward('view/editor_pain.inc', $REQ_SCOPE);
    }

    /**
     * 
     */
    public static function doPost($req='') {
        $proj_id = isset($_GET['id']) ? intval($_GET['id']) : null;
        if ($proj_id && isset($_POST) && !empty($_POST)) {
            // 本当はここで入力値チェックをする
            // $_POST
            // self::$db_editor->addUserTable($proj_id, $_POST['tbl_name'], $prop_hash);
        }
        return self::redirect($_SERVER["REQUEST_URI"]);
    }

    /**
     * サーブレットの初期設定メソッド
     */
    public static function setup($arg='') {
        if (self::$db_editor == null) {
            self::$db_editor = new DBEditor();
            // self::$db_editor->dropRoot();
        }
    }
}

?>