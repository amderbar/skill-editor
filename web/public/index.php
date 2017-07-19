<?php
session_start();

/**
 * Definition of application constants.
 */
define('APP_ROOT', '/skill_editor');
define('APP_NAME', 'Skill Editor');
define('VERSION', 'ver.0.0.0');
define('ENCODE', 'UTF-8');

define('VIEW_ROOT', 'app/view');
define('RESOURCE_ROOT', 'app/resources');
define('ROOT_DB_ID', 0 );
define('ROOT_DB', RESOURCE_ROOT . '/system_admin.db' );
define('SYSTEM_TBL', 's_admin_tbl' );
define('SYSTEM_COL', 's_admin_col' );
define('NUM_SETTINGS', 's_num_settings' );
define('FORM_TO_DB', [
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
// $routing_obj->whenGet|Post|Any('URI Path', 'action class::method')
// 'URI Path' can include parameters in the format '{param}'.
// e.g. '/hoge/{param1}/fuga/{param2}'
// These parameters are stored in the Request object and passed to the action.

$routing_obj = new Router(APP_ROOT);

$routing_obj->whenGet('/index.php', ['TopServlet', 'index']);
$routing_obj->whenGet('/', 'TopServlet::index');

$routing_obj->whenGet('/main', 'MainServlet::index');
$routing_obj->whenPost('/create', 'MainServlet::createProject');
$routing_obj->whenPost('/delete', 'MainServlet::deleteProject');

$routing_obj->whenGet('/editor/data', 'DataEditorServlet::index');
$routing_obj->whenGet('/editor/data/{pid}', 'DataEditorServlet::open');
$routing_obj->whenPost('/editor/data/modify', 'DataEditorServlet::modify');

$routing_obj->whenGet('/editor/table/{pid}', 'TableEditorServlet::open');
$routing_obj->whenPost('/editor/table/register', 'TableEditorServlet::register');

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
$responce = call_user_func_array(function ($action, array $uri_params) :string
{
    // 指定されたactionの引数のクラス名を配列化
    // actionは常にinterface ControllerArgを実装しているものとする
    $action_arg_class_names = array_map(function(ReflectionParameter $reflection_param) :string {
            return $reflection_param->getClass()->getName();
        }, getParameters($action));

    // 指定されたactionの引数をインスタンス化
    $action_args = array_map(function(string $class_name) use ($uri_params) {
            return new $class_name($uri_params);
        }, $action_arg_class_names);

    // 指定されたactionを実行
    return call_user_func_array(getInstanceMethod($action), $action_args) ?? '';

}, $routing_obj->getAction(parse_url($_SERVER['REQUEST_URI'], PHP_URL_PATH), $_SERVER["REQUEST_METHOD"]));

// なんか後処理

// レスポンスを返す

echo $responce;
exit;

/////////////////////////////////////////////////////////////////////////
// ライブラリを読み込む際に使う共通関数
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
