<?php
session_start();

/**
* 
*/
require_once('./common.php');
require_once(full_path('controllers/editor_servlet.php'));

EditorServlet::setup();
if($_SERVER["REQUEST_METHOD"] === 'GET'){
    EditorServlet::doGet();
} else if($_SERVER["REQUEST_METHOD"] === 'POST'){
    EditorServlet::doPost();
}

?>