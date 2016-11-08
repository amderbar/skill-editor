<?php
/**
* session check.
*/
require_once($_SERVER['DOCUMENT_ROOT'].'/skill_editor/common.php');

/**
* 
*/
require_once(full_path('controllers/servlet.php'));
require_once(full_path('models/db_editor.php'));

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
        $REQ_SCOPE['tbl_list'] = array();
        // リクエストパラメータidがセットされている時、そのidのプロジェクトが開かれる
        if (isset($_GET['id']) && $_GET['id']!='') {
            $current_proj_id = intval($_GET['id']);
            if(self::$db_editor->open($current_proj_id) === false){
                return self::redirect($_SERVER['PHP_SELF']);
            }
            $REQ_SCOPE['proj_id'] = $current_proj_id;
            $table_list = self::$db_editor->listTables($current_proj_id);
            if ($table_list) {
                $opened_tab = (isset($_GET['tab'])) ? intval($_GET['tab']) : 0;
                $teble_data = self::$db_editor->listData($current_proj_id, $table_list[$opened_tab]);
                $tmpl_list = self::$db_editor->getTemplates($current_proj_id);
                // 先頭要素のキーを取得
                $selected_tmpl = key($tmpl_list);
                // リクエストスコープ相当の配列にデータを格納
                $REQ_SCOPE['tbl_list'] = $table_list;
                $REQ_SCOPE['tbl_data'] = $teble_data;
                $REQ_SCOPE['tbl_tmpl'] = ($table_list[$opened_tab] == 'skills_view') ?
                    full_path(sprintf('resources/templates/proj%03d/', $current_proj_id).$tmpl_list[$selected_tmpl]) :
                    full_path('resources/templates/default_template.php');
                $REQ_SCOPE['opened_tab'] = $opened_tab;
                // 肝心のデータはセッションスコープにも入れておく
                $_SESSION['proj'.$current_proj_id][$opened_tab] = $teble_data;
            } else {
                $REQ_SCOPE['proj_name'] = self::$db_editor->projName($current_proj_id);
            }
        }
        return self::foward('view/editor_pain.php', $REQ_SCOPE);
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