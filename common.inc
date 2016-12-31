<?php
/**
* session check.
*/
require_once(full_path('gatekeeper.inc'));
/**
* Definition of constants and global valiables.
*/
define('VERSION', 'ver.0.0.0');
define('ENCODE', 'UTF-8');
// mb_internal_encoding(ENCODE);
// mb_regex_encoding();

/**
* load library and common functions
*/
require_once(full_path('models/html_handler.inc'));

/**
* return file full path of argument string
* if null arg, return the full path of directory.
*/
function full_path($path='', $newfile=false) {
    if ($path && strpos($path,'/') !== 0) {
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
    return $var;
}

/**
* 
*/
function fnv132($str) {
    return hash('fnv132', $str);
}

/**
* https://www.softel.co.jp/blogs/tech/archives/58 からコピペした
* 多次元配列の次元数を調べる関数
*/
function array_depth($a, $c = 0) {
    return (is_array($a) && count($a))
            ? max(array_map("array_depth", $a, array_fill(0, count($a), ++$c)))
            : $c;
}

/**
* http://doop-web.com/blog/archives/1182 からコピペし、改造した
* ファイルの更新日時をリクエストパラメータ風にファイル名に付け加える関数
*/
function addFilemtime($filename) {
    if (file_exists($filename)) {
        return $filename . '?date='. date('YmdHis', filemtime($filename));
    } else {
        return $filename;
    }
}


/**
* http://10000-hours.jp/wordpress/2015/09/2015091101/ からコピペした
* 多次元配列の差分を求める関数
*/
function array_diff_assoc_recursive( ) {
    $args = func_get_args();
    //エラーチェック
    if( empty($args) ) {
        return false;
    }
    foreach($args as $array_one) {
        if( !is_array( $array_one ) ) {
            return false;
        }
    }
    //2つ以上配列の指定がある場合
    $difference = array();
    if( count($args) > 2 ) {
        $difference = array_shift( $args );
        foreach($args as $array_one) {
            $difference = array_diff_assoc_recursive( $difference , $array_one );
        }
    } else {
        foreach($args[0] as $key => $value) {
            if( is_array($value) ) {
                if( !isset($args[1][$key]) || !is_array($args[1][$key]) ) {
                    $difference[$key] = $value;
                } else {
                    $new_diff = array_diff_assoc_recursive($value, $args[1][$key]);
                    if( !empty($new_diff) ) {
                        $difference[$key] = $new_diff;
                    }
                }
            } else if( !array_key_exists($key,$args[1]) || $args[1][$key] !== $value ) {
                $difference[$key] = $value;
            }
        }
    }
    return $difference;
}

?>