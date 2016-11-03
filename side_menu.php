<?php
session_start();

/**
* 
*/
require_once('./common.php');
require_once(full_path('controllers/side_servlet.php'));

SideServlet::setup();
if($_SERVER["REQUEST_METHOD"] === 'GET'){
    SideServlet::doGet();
} else if($_SERVER["REQUEST_METHOD"] === 'POST'){
    SideServlet::doPost();
}

?>