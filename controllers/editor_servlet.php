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
        $tmpl_list = null;
        $proj_list = self::$db_editor->listDB();
        // リクエストパラメータidがセットされている時、そのidのプロジェクトが開かれる
        if (isset($_GET['id']) && $_GET['id']!='') {
            $current_proj_id = intval($_GET['id']);
            if(self::$db_editor->open($current_proj_id) === false){
                return self::redirect($_SERVER['PHP_SELF']);
            }
            $table_list = self::$db_editor->listTables($current_proj_id);
            if ($table_list) {
                $data_list = array();
                foreach ($table_list as $table) {
                    $data_list[$table] = self::$db_editor->listData($current_proj_id,$table);
                }
                $tmpl_list = self::$db_editor->getTemplates($current_proj_id);
                // 先頭要素のキーを取得
                $selected_tmpl = key($tmpl_list);
                // リクエストスコープ相当の配列にデータを格納
                $REQ_SCOPE['current_proj_data_list'] = $data_list;
                $REQ_SCOPE['current_proj_tbl_list'] = $table_list;
                $REQ_SCOPE['tmpl_list'] = $tmpl_list;
                $REQ_SCOPE['selected_tmpl'] = array('skills_view' => $selected_tmpl);
                // 肝心のデータはセッションスコープにも入れておく
                $_SESSION['proj'.$current_proj_id] = $data_list;
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