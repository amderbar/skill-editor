<?php
require_once(full_path('models/db_editor.php'));

/**
 * 
 */
class Servlet {
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
        if (isset($_GET['id'])&&$_GET['id']!='') {
            $current_proj_id = intval($_GET['id']);
            $proj_name = $proj_list[$current_proj_id];
            self::$db_editor->open(sprintf('proj%03d.db',$current_proj_id),$proj_name);
            $table_list = self::$db_editor->listTables($proj_name);
            $data_list = array();
            foreach ($table_list as $table) {
                $data_list[$table] = self::$db_editor->listData($proj_name,$table);
            }
            $tmpl_list = self::$db_editor->getTemplates($current_proj_id);
            // 先頭要素のキーを取得
            $selected_tmpl = key($tmpl_list);
            // リクエストスコープ相当の配列にデータを格納
            $REQ_SCOPE['proj_list'] = $proj_list;
            $REQ_SCOPE['current_proj_data_list'] = $data_list;
            $REQ_SCOPE['current_proj_tbl_list'] = $table_list;
            $REQ_SCOPE['tmpl_list'] = $tmpl_list;
            $REQ_SCOPE['selected_tmpl'] = array('skills_view' => $selected_tmpl);
            // 肝心のデータはセッションスコープにも入れておく
            $_SESSION['proj'.$current_proj_id] = $data_list;
        }
        return foward(full_path('view/index_page.php'),$REQ_SCOPE);
    }

    /**
    * 
    */
    public static function doPost($req='') {
        if (isset($_POST['proj-name']) && ($_POST['proj-name'] != '')) {
            $proj_id = self::$db_editor->registerDB($_POST['proj-name']);
        } elseif (isset($_FILES['tmpl-file'])) {
            $proj_id = intval($_GET['id']);
            $tmpl_name = self::upLoadFile($proj_id);
            self::$db_editor->registerTemplate($proj_id,$tmpl_name);
        }
        $redirect_uri = empty($_SERVER["HTTPS"]) ? "http://" : "https://";
        $redirect_uri .= $_SERVER["HTTP_HOST"] . $_SERVER["REQUEST_URI"];
        header('Location: ' . $redirect_uri);
        // return self::doGet();
    }

    /**
    * 
    */
    public static function setup($arg='') {
        if (self::$db_editor == null) {
            self::$db_editor = new DBEditor();
            // self::$db_editor->dropRoot();
        }
    }

    /**
    * 
    */
    private static function upLoadFile($proj_id) {
        // ファイル名についてのその他のバリデーションが必要
        switch ($_FILES['tmpl-file']['error']) {
            case UPLOAD_ERR_OK:
                $uploaddir = sprintf('view/templates/proj%03d',$proj_id);
                $file_name = basename($_FILES['tmpl-file']['name']);
                $uploadfile = full_path($uploaddir) .'/'. $file_name;
                if (move_uploaded_file($_FILES['tmpl-file']['tmp_name'], $uploadfile)) {
                    return $file_name;
                } else {
                    die("Possible file upload attack!\n");
                }
                break;
            default:
                echo 'File Upload Failed'.PHP_EOL;
                break;
        }
    }
}

?>