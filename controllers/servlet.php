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
        $current_proj = null;
        if (isset($_SESSION['current_proj'])) {
            $current_proj = $_SESSION['current_proj'];
        }
        return include(full_path('view/index_page.php'));
    }

    /**
    * 
    */
    public static function doPost($req='') {
        if (isset($_POST['proj_name'])&&($_POST['proj_name']!='')) {
            self::$db_editor->registerDB($_POST['proj_name']);
        }
        return self::doGet($req);
    }

    /**
    * 
    */
    public static function setup($arg='') {
        if (! self::$db_editor) {
            self::$db_editor = new DBEditor();
        }
    }
}

?>