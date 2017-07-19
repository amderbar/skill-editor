<?php
// FIXME: ORDER BY 対応
// リスト表示をする部分の並び順を一定にするように見直し

// FIXME: CSRF対策、二度押し対策の整備

/**
 * load library and common functions
 * TODO:ここももうちょっと賢くやりたい
 */
require_once(full_path('base/gatekeeper.inc'));
require_once(full_path('base/router.inc'));
foreach ( glob(full_path('base/controllers') . '/*.inc') as $file ) {
    if( is_file($file) ){
        require_once($file);
    }
}
foreach ( glob(full_path('base/libs') . '/*.inc') as $file ) {
    if( is_file($file) ){
        require_once($file);
    }
}
foreach ( glob(full_path('base/libs/utils') . '/*.inc') as $file ) {
    if( is_file($file) ){
        require_once($file);
    }
}
foreach ( glob(full_path('base/libs/database') . '/*.inc') as $file ) {
    if( is_file($file) ){
        require_once($file);
    }
}
foreach ( glob(full_path('app/controllers') . '/*.inc') as $file ) {
    if( is_file($file) ){
        require_once($file);
    }
}
foreach ( glob(full_path('app/controllers/requests') . '/*.inc') as $file ) {
    if( is_file($file) ){
        require_once($file);
    }
}
foreach ( glob(full_path('app/processes') . '/*.inc') as $file ) {
    if( is_file($file) ){
        require_once($file);
    }
}

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
 * Returns a callable which formatted from instance method format.
 *
 * @param string|array $callable
 * @param callable $formatter
 * @return mixed
 */
function getCallableMethod($callable, callable $formatter)
{
    if (is_array($callable) || (is_string($callable) && strpos($callable, '::') !== false)) {
        [$class, $method] = is_string($callable) ? explode('::', $callable) : $callable;
        return $formatter($class, $method);
    }
    return null;
}

/**
 * Returns arguments accepted by a callable.
 *
 * @param string|array $callable
 * @return array
 */
function getParameters($callable) :array
{
    return (getCallableMethod($callable, function (string $class, string $method) {
        return (new ReflectionClass($class))->getMethod($method);
    }) ?? new ReflectionFunction($callable))
    ->getParameters();
}

/**
 * Returns callable array for call_user_func(_array).
 *
 * @param string|array $callable
 * @return callable
 */
function getInstanceMethod($callable) :callable
{
    return getCallableMethod($callable, function (string $class, string $method) {
        return [new $class(), $method];
    }) ?? $callable;
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
