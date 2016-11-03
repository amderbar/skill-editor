<?php
/**
* session check.
*/
require_once($_SERVER['DOCUMENT_ROOT'].'/skill_editor/gatekeeper.php');

/**
 * the super class for servlets.
 */
abstract class Servlet {

    /**
    * initialize method for servlet.
    */
    abstract public static function setup($arg='');

    /**
    * 
    */
    abstract public static function doGet($req='');

    /**
    * 
    */
    abstract public static function doPost($req='');

    /**
    * 
    */
    protected static function foward($dist, $REQ_SCOPE = null) {
        global $URL;
        if (isset($REQ_SCOPE)) {
            extract($REQ_SCOPE);
            unset($REQ_SCOPE);
        }
        return include(full_path($dist));
    }

}

?>