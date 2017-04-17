<?php

/**
 * the super class for servlets.
 */
abstract class Servlet {

    /**
    * initialize method for servlet.
    */
    public function __construct() {}

    /**
     *
     * @param string $req
     */
    abstract public function doGet($req='');

    /**
     *
     * @param string $req
     */
    abstract public function doPost($req='');

    /**
     *
     * @param unknown $dest
     * @param unknown $REQ_SCOPE
     * @return unknown
     */
    protected function foward($dest, $REQ_SCOPE = null) {
        $URL = getMyTopURL();
        if (isset($REQ_SCOPE)) {
            extract($REQ_SCOPE);
            unset($REQ_SCOPE);
        }
        return include(full_path($dest));
    }

    /**
     *
     * @param unknown $dist
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