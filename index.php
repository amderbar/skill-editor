<?php
session_start();

/**
* 
*/
require_once('./common.inc');
require_once(full_path('controllers/index_servlet.inc'));

IndexServlet::setup();
if($_SERVER["REQUEST_METHOD"] === 'GET'){
    IndexServlet::doGet();
} else if($_SERVER["REQUEST_METHOD"] === 'POST'){
    IndexServlet::doPost();
}

?>