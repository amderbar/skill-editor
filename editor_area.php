<?php
session_start();

/**
* 
*/
require_once('./common.inc');
require_once(full_path('controllers/editor_servlet.inc'));

EditorServlet::setup();
if($_SERVER["REQUEST_METHOD"] === 'GET'){
    EditorServlet::doGet();
} else if($_SERVER["REQUEST_METHOD"] === 'POST'){
    EditorServlet::doPost();
}

?>