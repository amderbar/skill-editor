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
        if (isset($_GET['id']) && $_GET['id']!='') {
            $current_proj_id = intval($_GET['id']);
            $proj_name = $proj_list[$current_proj_id];
            self::$db_editor->open(sprintf('proj%03d.db',$current_proj_id), $proj_name);
            $table_list = self::$db_editor->listTables($proj_name);
            // リクエストスコープ相当の配列にデータを格納
            $REQ_SCOPE['proj_list'] = $proj_list;
            $REQ_SCOPE['current_proj_tbl_list'] = $table_list;
        }
        return self::foward('view/side_pain.php', $REQ_SCOPE);
    }

    /**
    * 
    */
    public static function doPost($req='') {
        self::doGet($req);
        return;
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