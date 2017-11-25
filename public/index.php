<?php
require __DIR__ . "/../vendor/autoload.php";

// なんか前処理
use Amderbar\Lib\Core\App;
use Amderbar\Lib\Core\Router;

// print_r(spl_autoload_functions());
// exit;
session_start();

/**
 * Definition of application constants.
 */
define('APP_ROOT', '/skill_editor');
define('APP_NAME', 'Skill Editor');
define('VERSION', 'ver.0.0.0');
define('ENCODE', 'UTF-8');

define('VIEW_ROOT', 'resources/views');
define('RESOURCE_ROOT', 'storage');
define('ROOT_DB_ID', 0);
define('ROOT_DB', RESOURCE_ROOT . '/system_admin.db');
define('SYSTEM_TBL', 's_admin_tbl');
define('SYSTEM_COL', 's_admin_col');
define('NUM_SETTINGS', 's_num_settings');
define('INTERNAL_TBLS', 's_internal_tbls');
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
]);
// mb_internal_encoding(ENCODE);
// mb_regex_encoding();


/////////////////////////////////////////////////////////////////////////
// Route Settings
/////////////////////////////////////////////////////////////////////////
// $routing_obj->whenGet|Post|Any('URI Path', 'action class::method')
// 'URI Path' can include parameters in the format '{param}'.
// e.g. '/hoge/{param1}/fuga/{param2}'
// These parameters are stored in the Request object and passed to the action.
// TODO: 名前空間の修飾を省略できるようにしたい

[$action, $uir_parm] = Router::setup(function (Router $routing_obj) {

    $routing_obj->whenGet('/index.php', ['Amderbar\App\Actions\TopAction', 'index']);
    $routing_obj->whenGet('/', 'Amderbar\App\Actions\TopAction::index');

    $routing_obj->whenGet('/main', 'Amderbar\App\Actions\MainAction::index');
    $routing_obj->whenPost('/create', 'Amderbar\App\Actions\MainAction::createProject');
    $routing_obj->whenPost('/delete', 'Amderbar\App\Actions\MainAction::deleteProject');

    $routing_obj->whenGet('/editor/data', 'Amderbar\App\Actions\DataEditorAction::index');
    $routing_obj->whenGet('/editor/data/{pid}', 'Amderbar\App\Actions\DataEditorAction::open');
    $routing_obj->whenPost('/editor/data/update', 'Amderbar\App\Actions\DataEditorAction::update');

    $routing_obj->whenGet('/editor/table/{pid}', 'Amderbar\App\Actions\TableEditorAction::open');
    $routing_obj->whenPost('/editor/table/register', 'Amderbar\App\Actions\TableEditorAction::register');

}, APP_ROOT)
->getAction(parse_url($_SERVER['REQUEST_URI'], PHP_URL_PATH), $_SERVER["REQUEST_METHOD"]);

$responce = App::run($action, $uir_parm);

// なんか後処理

// レスポンスを返す

echo $responce;
exit;
