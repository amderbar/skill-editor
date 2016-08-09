<?php
/**
 * 
 */
require_once(full_path('models/db_manager.php'));

class Servlet {
    
    /**
    * 
    */
    public static function doGet($req='') {
        $root_db = new SQLiteHandler();
        $proj_list = $root_db->ls_db();
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