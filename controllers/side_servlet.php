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
class SideServlet extends Servlet {
    /**  */
    private static $db_editor = null;

    /**
    * 
    */
    public static function doGet($req='') {
        $REQ_SCOPE = array();
        $proj_list = self::$db_editor->listDB();
        $REQ_SCOPE['proj_list'] = $proj_list;
        $tmpl_list = null;
        // リクエストパラメータidがセットされている時、そのidのプロジェクトが開かれる
        if (isset($_GET['id']) && $_GET['id'] != '') {
            $current_proj_id = intval($_GET['id']);
            if(self::$db_editor->open($current_proj_id) === false){
                return self::redirect($_SERVER['PHP_SELF']);
            }
            $table_list = self::$db_editor->listTables($current_proj_id);
            // リクエストスコープ相当の配列にデータを格納
            $REQ_SCOPE['current_proj_tbl_list'] = $table_list;
        }
        return self::foward('view/side_pain.php', $REQ_SCOPE);
    }

    /**
    * 
    */
    public static function doPost($req='') {
        pre_dump($_POST);
        if (isset($_POST['fMode'])) {
            if ($_POST['fMode'] == 'del-prj') {
                // プロジェクト削除時
                // 本当はここで入力値チェックをする
                // CSRF対策のトークンチェックも必要
                $proj_id = isset($_POST['id']) ? intval($_POST['id']) : null;
                if ($proj_id) {
                    self::$db_editor->deleteDB($proj_id);
                }
            }
        }
        return self::redirect($_SERVER["REQUEST_URI"]);
        // return self::doGet($req);
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