<?php

namespace Amderbar\Lib\Core;

use Amderbar\Lib\Utils\FunctionalUtil as Func;

class App
{
    /**
     * Undocumented function
     *
     * @param [type] $action
     * @param array $uri_params
     * @return string
     */
    public static function run($action, array $uri_params) :string
    {
        // 指定されたactionの引数のクラス名を配列化
        $action_arg_class_names = array_map(function (\ReflectionParameter $reflection_param) :string
            {
                return $reflection_param->getClass()->getName() ?? null;
            }, Func::getParameters($action));

        // 指定されたactionの引数をインスタンス化
        $action_args = array_map(function(string $class_name) use ($uri_params)
            {
                return new $class_name($uri_params);
            }, $action_arg_class_names);
    
        // 指定されたactionを実行
        return call_user_func_array(Func::getInstanceMethod($action), $action_args) ?? '';
    }    
}
