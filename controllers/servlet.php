<?php
require_once(full_path('models/db_editor.php'));

/**
 * 
 */
class Servlet {
    
    /**
    * 
    */
    public static function doGet($req='') {
        $db_editor = new DBEditor();
        $proj_list = $db_editor->listDB();
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
        return '';
    }
}

?>