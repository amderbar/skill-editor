<?php
session_start();

/**
 * Definition of application constants.
 */
define('APP_ROOT', '/skill_editor');
define('APP_NAME', 'DB Editor on Browser');
define('VERSION', 'ver.0.0.0');
define('ENCODE', 'UTF-8');

define('VIEW_ROOT', 'app/view');
define('RESOURCE_ROOT', 'app/resources');
define('ROOT_DB_ID', 0 );
define('ROOT_DB', RESOURCE_ROOT . '/system_admin.db' );
define('SYSTEM_TBL', 's_admin_tbl' );
define('SYSTEM_COL', 's_admin_col' );
define('NUM_SETTINGS', 's_num_settings' );
define('FORM_TO_DATA', [
        'color' => 'text',
        'text' => 'text',
        'textarea' => 'text',
        'tel' => 'text',
        'url' => 'text',
        'email' => 'text',
        'password' => 'text',
        'datetime' => 'text',
        'date' => 'datetime',
        'month' => 'datetime',
        'week' => 'datetime',
        'time' => 'datetime',
        'datetime-local' => 'datetime',
        'listext' => 'numeric',
        'number' => 'numeric',
        'numlist' => 'numeric',
        'range' => 'numeric',
        'select' => 'numeric',
        'multicheck' => 'numeric',
        'radio' => 'numeric',
        'checkbox' => 'boolean',
        'file' => 'blob',
        'image' => 'blob',
        'hidden' => 'blob'
] );
// mb_internal_encoding(ENCODE);
// mb_regex_encoding();

/**
 *
 */
require_once(full_path('base/common.inc'));

// なんか前処理

/////////////////////////////////////////////////////////////////////////
// Route Settings
/////////////////////////////////////////////////////////////////////////
$routing_obj = new Router(APP_ROOT);

$routing_obj->whenGet('/index.php', ['TopServlet', 'index']);
$routing_obj->whenGet('/', 'TopServlet::index');
$routing_obj->whenPost('/create', 'TopServlet::createProject');
$routing_obj->whenPost('/delete', 'TopServlet::deleteProject');

$routing_obj->whenGet('/main', 'MainServlet::index');

$routing_obj->whenGet('/editor/data', 'DataEditorServlet::index');
$routing_obj->whenGet('/editor/data/{pid}', 'DataEditorServlet::open');
$routing_obj->whenPost('/editor/data/modify', 'DataEditorServlet::modify');

$routing_obj->whenGet('/editor/table/{pid}', 'TableEditorServlet::open');
$routing_obj->whenPost('/editor/table/register', 'TableEditorServlet::register');
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

$responce = call_user_func(function (array $routing) :string
{
    [$action, $uri_params] = $routing;
    if ( !isset($action) ) {
        var_export_log(parse_url($_SERVER['REQUEST_URI'], PHP_URL_PATH));
        throw new \Exeption();
    }
    // 引数の準備
    $param_arr = array();
    foreach (getParameters($action) as $reflection_param) {
        if ($param_class = $reflection_param->getClass()) {
            $param_arr[] = call_user_func(function($class_name){
                return new $class_name();
            }, $param_class->getName());

        } else {
            $param_arr[] = call_user_func(function ($param) use ($uri_params) {
                switch ($param->getType()) {
                    case 'int':
                        $format_param = 'intval';
                        break;

                    default:
                        $format_param = function ($val) { return $val; };
                        break;
                }
                return $format_param($uri_params[$param->getName()] ?? null);
            }, $reflection_param);
        }
    }

    if ( is_array($action) || (is_string($action) && (strpos($action, '::') !== false)) ) {
        [$class, $method] = is_string($action) ? explode('::', $action) : $action;
        $action = [new $class(), $method];
    }
    return call_user_func_array($action, $param_arr) ?? '';

}, $routing_obj->getAction(parse_url($_SERVER['REQUEST_URI'], PHP_URL_PATH), $_SERVER["REQUEST_METHOD"]));

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
