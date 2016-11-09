<?php
session_start();

/**
* 
*/
require_once('./common.inc');
require_once(full_path('controllers/side_servlet.inc'));

SideServlet::setup();
if($_SERVER["REQUEST_METHOD"] === 'GET'){
    SideServlet::doGet();
} else if($_SERVER["REQUEST_METHOD"] === 'POST'){
    SideServlet::doPost();
}

?>