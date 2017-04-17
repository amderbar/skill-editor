<?php
// FIXME: ORDER BY 対応
// リスト表示をする部分の並び順を一定にするように見直し

// FIXME: CSRF対策、二度押し対策の整備

// FIXME: 入力値チェックの統一的な仕組み

/**
* session check.
*/
require_once(full_path('base/gatekeeper.inc'));

/**
* load library and common functions
*/
require_once(full_path('base/libs/html_handler.inc'));
require_once(full_path('base/libs/exception.inc'));
require_once(full_path('base/controllers/servlet.inc'));

/**
*
*/
function getMyTopURL() {
    return parse_url($_SERVER["REQUEST_URI"], PHP_URL_PATH);
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
function var_export_log($var) {
    error_log(var_export($var, true));
}

/**
*
*/
function fnv132($str) {
    return hash('fnv132', $str);
}

/**
 * Returns an indication of the number of arguments accepted by a callable.
 *
 * @param callable $callable
 * @return int|null
 */
function signature(callable $callable)
{
    if (is_array($callable) || (is_string($callable) && strpos($callable, '::') !== false)) {
        list($class, $method) = is_string($callable) ? explode('::', $callable) : $callable;
        $reflection = (new ReflectionClass($class))->getMethod($method);

    } else {
        $reflection = new ReflectionFunction($callable);

    }
    return $reflection->getParameters();
}


/**
 * @param string $glue
 * @param array $asoc
 * @return array
 */
function implodeAssoc(string $glue, array $asoc) :array {
    $result = array();
    foreach ($asoc as $key => $child_arr) {
        if ( !is_array( $child_arr ) ) {
            $child_arr = array( $child_arr );
        }
        foreach ($child_arr as $val) {
            $result[] = "{$key}{$glue}{$val}";
        }
    }
    return $result;
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
