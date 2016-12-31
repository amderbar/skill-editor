<?php
session_start();

/**
* 
*/
require_once('./common.inc');
require_once(full_path('controllers/side_servlet.inc'));

$servlet = new SideServlet();
if($_SERVER["REQUEST_METHOD"] === 'GET'){
    $servlet->doGet();
} else if($_SERVER["REQUEST_METHOD"] === 'POST'){
    $servlet->doPost();
}

?>