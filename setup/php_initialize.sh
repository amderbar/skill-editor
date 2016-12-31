#! /bin/bash

directives=(\
    'expose_php',\
    'error_reporting',\
    'display_errors',\
    'display_startup_errors',\
    'log_errors',\
    'error_log',\
    'extension_dir',\
    'default_charset',\
    'date.timezone',\
    'mbstring.language',\
    'mbstring.detect_order'
)

values_dev=(\
    'off',\
    'E_ALL',\
    'on',\
    'on',\
    'on',\
    '/var/log/php/error.log',\
    '/php/ext',\
    'UTF-8',\
    'Asia/Tokyo',\
    'Japanese',\
    'auto'
)
values_pro=(\
    'off',\
    'E_ALL & ~E_DEPRECATED & ~E_STRICT',\
    'off',\
    'off',\
    'on',\
    '/var/log/php/error.log',\
    '/php/ext',\
    'UTF-8',\
    'Asia/Tokyo',\
    'Japanese',\
    'auto'
)

extensionts=(
    'php_mbstring.dll',\
    'php_pdo.dll',\
    'php_pdo_sqlite.dll',\
    'php_sqlite3.dll'
)

