<?php
require_once(full_path('models/db_editor.php'));

Servlet::setup();

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
        $proj_list = self::$db_editor->listDB();
        $proj_template = null;
        if (isset($_GET['id'])&&$_GET['id']!='') {
            $current_proj_id = intval($_GET['id']);
            $proj_name = $proj_list[$current_proj_id];
            self::$db_editor->open(sprintf('proj%03d.db',$current_proj_id),$proj_name);
            $table_list = self::$db_editor->listTables($proj_name);
            $data_list = array();
            foreach ($table_list as $table) {
                $data_list[$table] = self::$db_editor->listData($proj_name,$table);
            }
            $proj_template = self::$db_editor->getTemplates($current_proj_id);
            $selected_tmpl = key($proj_template);
        }
        return include(full_path('view/index_page.php'));
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