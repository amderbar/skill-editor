<?php
/**
* 
*/
define('ROOT_DB', 'project_manager.db');
require_once(full_path('controllers/servlet.php'));
session_start();
if($_SERVER["REQUEST_METHOD"] === 'GET'){
    Servlet::doGet();
} else if($_SERVER["REQUEST_METHOD"] === 'POST'){
    Servlet::doPost();
}

    /**
    * return file full path of argument string
    * if null arg, return the full path of directory.
    */
    function full_path($path='',$newfile=false) {
        if ($path and strpos($path,'/') != 0) {
            $path = '/' . $path;
        }
        $path = str_replace('/', DIRECTORY_SEPARATOR, __DIR__ . $path);
        if ((! $newfile) || (realpath($path))) {
            $path = realpath($path);
        }
        return str_replace(DIRECTORY_SEPARATOR, '/', $path);
    }

    /**
    * 
    */
    function pre_dump($var) {
        echo '<pre>';
        var_dump($var);
        echo '</pre>';
    }
?>