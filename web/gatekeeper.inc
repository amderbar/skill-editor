<?php
/**
* session check.
*/
switch (session_status()) {
    case PHP_SESSION_DISABLED:
    case PHP_SESSION_NONE:
        header('Location: '.$_SERVER['DOCUMENT_ROOT'].'/skill_editor/');
        break;
    case PHP_SESSION_ACTIVE:
        break;
}

?>