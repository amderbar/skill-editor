<?php
/**
* session check.
*/
require_once($_SERVER['DOCUMENT_ROOT'].'/skill_editor/common.inc');

/**
* 
*/
require_once(full_path('models/db_editor.inc'));

/**
 * the super class for servlets.
 */
abstract class Servlet {
    /**  */
    protected $db_editor = null;

    /**
    * initialize method for servlet.
    */
    public function __construct($arg='') {
        if ($this->db_editor == null) {
            $this->db_editor = new DBEditor();
            // $this->db_editor->dropRoot();
        }
    }
    
    /**
    * 
    */
    abstract public function doGet($req='');

    /**
    * 
    */
    abstract public function doPost($req='');

    /**
    * 
    */
    protected function foward($dist, $REQ_SCOPE = null) {
        $URL = getMyTopURL();
        if (isset($REQ_SCOPE)) {
            extract($REQ_SCOPE);
            unset($REQ_SCOPE);
        }
        return include(full_path($dist));
    }

    /**
    * 
    */
    protected function redirect($dist = null) {
        if (empty($dist)) {
            $dist = getMyTopURL();
        }
        $redirect_uri = empty($_SERVER["HTTPS"]) ? "http://" : "https://";
        $redirect_uri .= $_SERVER["HTTP_HOST"] . $dist;
        return header('Location: ' . $redirect_uri);
    }
}

?>