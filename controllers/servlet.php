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
            $proj_id = intval($_GET['id']);
            $proj_name = $proj_list[$proj_id];
            self::$db_editor->open(sprintf('proj%03d.db',$proj_id),$proj_name);
            $data_list = self::$db_editor->listData($proj_name,'skills_view');
            $proj_template = self::$db_editor->getTemplates($proj_id);
        }
        return include(full_path('view/index_page.php'));
    }

    /**
    * 
    */
    public static function doPost($req='') {
        if (isset($_POST['proj_name'])&&($_POST['proj_name']!='')) {
            $proj_id = self::$db_editor->registerDB($_POST['proj_name']);
        }
        $redirect_uri = empty($_SERVER["HTTPS"]) ? "http://" : "https://";
        $redirect_uri .= $_SERVER["HTTP_HOST"] . $_SERVER["REQUEST_URI"];
        header('Location: ' . $redirect_uri);
    }

    /**
    * 
    */
    public static function setup($arg='') {
        if (self::$db_editor == null) {
            self::$db_editor = new DBEditor();
        }
    }
}

?>