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
foreach ( glob(full_path('app/controllers') . '/{*_servlet.inc}', GLOB_BRACE) as $file ) {
    if( is_file($file) ){
        require_once($file);
    }
}

// なんか前処理

// Routing
$routing_obj = new class(APP_ROOT)
{
    private $path_prefix;
    private $get_routing = array();
    private $post_routing = array();

    public function __construct(?string $path_prefix = null)
    {
      $this->path_prefix = $path_prefix;
    }

    public function whenGet(string $path, $action) :void
    {
        $this->get_routing[$this->getPrefix() . $path] = $action;
    }

    public function whenPost(string $path, $action) :void
    {
        $this->get_routing[$this->getPrefix() . $path] = $action;
    }

    public function whenAny(string $path, $action) :void
    {
        $this->get_routing[$this->getPrefix() . $path] = $action;
        $this->post_routing[$this->getPrefix() . $path] = $action;
    }

    public function getPrefix(): string
    {
        return $this->path_prefix ?? '';
    }

    public function getGetAction(string $path)
    {
        return $this->get_routing[$path] ?? null;
    }

    public function getPostAction(string $path)
    {
        return $this->post_routing[$path] ?? null;
    }
};

/////////////////////////////////////////////////////////////////////////
// Route Settings
/////////////////////////////////////////////////////////////////////////

$routing_obj->whenGet('/index.php', ['IndexServlet', 'index']);
$routing_obj->whenGet('/', 'IndexServlet::index');
$routing_obj->whenGet('main', null);

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

$responce = call_user_func(function ($routing_config, $request_path) :string
{
    $path_prefix = $routing_config->getPrefix();
    if ( $_SERVER["REQUEST_METHOD"] === 'GET' ) {
        $action = $routing_config->getGetAction($request_path);

    } else if ( $_SERVER["REQUEST_METHOD"] === 'POST' ) {
        $action = $routing_config->getPostAction($request_path);
    }
    if ( isset($action) ) {
        pre_dump(signature($action));

        if ( is_callable($action) ) {
            return $action();
        }
        if ( is_string($action) ) {
            [$class, $method] = explode('@', $action);
            return $action = (new $class())->$method;
        }
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
