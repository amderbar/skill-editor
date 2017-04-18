<?php
session_start();

/**
 * Definition of application constants.
 */
define('APP_ROOT', '/skill_editor');
define('APP_NAME', 'DB Editor on Browser');
define('VERSION', 'ver.0.0.0');
define('ENCODE', 'UTF-8');
// mb_internal_encoding(ENCODE);
// mb_regex_encoding();

/**
 *
 */
require_once(full_path('base/common.inc'));
require_once(full_path('base/router.inc'));
foreach ( glob(full_path('app/controllers') . '/{*_servlet.inc}', GLOB_BRACE) as $file ) {
    if( is_file($file) ){
        require_once($file);
    }
}

// なんか前処理

// Routing
$routing_obj = new Router(APP_ROOT);

/////////////////////////////////////////////////////////////////////////
// Route Settings
/////////////////////////////////////////////////////////////////////////
$callback = function (?string $hoge = null) {
    return 'hollo ' . ($hoge ?? 'unspecified');
};
$routing_obj->whenGet('/index.php', ['IndexServlet', 'index']);
$routing_obj->whenGet('/', 'IndexServlet::index');
$routing_obj->whenGet('/main', $callback);
$routing_obj->whenGet('/main/{hoge}', $callback);
$routing_obj->whenGet('/main/{hoge}/moge/piyo', $callback);

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

$responce = call_user_func(function (Router $routing_config, string $request_path) :string
{
    if ( $_SERVER["REQUEST_METHOD"] === 'GET' ) {
        [$action, $uri_params] = $routing_config->getGetAction($request_path);

    } else if ( $_SERVER["REQUEST_METHOD"] === 'POST' ) {
        [$action, $uri_params] = $routing_config->getPostAction($request_path);
    }
    if ( isset($action) ) {
        // 引数の準備
        $param_arr = array();
        foreach (getParameters($action) as $param) {
            if ($param_class = $param->getClass()) {
                $param_arr[] = call_user_func(function($class_name){
                    return new $class_name('hoge');
                }, $param_class->getName());

            } else {
                $param_arr[] = $uri_params[$param->getName()] ?? null;
            }
        }

        if ( is_array($action) || (is_string($action) && (strpos($action, '::') !== false)) ) {
            [$class, $method] = is_string($action) ? explode('::', $action) : $action;
            $action = [new $class(), $method];
        }

        return call_user_func_array($action, $param_arr);
    }
    throw new Exeption();

}, $routing_obj, parse_url($_SERVER['REQUEST_URI'], PHP_URL_PATH));

// なんか後処理

// レスポンスを返す

echo $responce;
exit;

/////////////////////////////////////////////////////////////////////////

/**
 * return file full path of argument string
 * if null arg, return the full path of directory.
 *
 * @param string $path
 * @param bool $newfile
 * @return string
 */
function full_path(string $path = '', bool $newfile = false) :string
{
    static $root_path;
    $root_path = $root_path ?? str_replace(DIRECTORY_SEPARATOR, '/', realpath($_SERVER['DOCUMENT_ROOT'] . APP_ROOT . '/..'));

    if ($path && strpos($path,'/') !== 0) {
        $path = '/' . $path;
    }
    $path = str_replace('/', DIRECTORY_SEPARATOR, $root_path . $path);
    $path = realpath($path) ?: ($newfile ? $path : '');
    return str_replace(DIRECTORY_SEPARATOR, '/', $path);
}
