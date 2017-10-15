<?php

namespace Amderbar\Lib\Utils;

class DebugUtil
{
    /**
     * Undocumented function
     *
     * @param [type] $var
     * @return void
     */
    public static function pre_dump($var) :void
    {
        echo '<pre>';
        var_dump($var);
        echo '</pre>';
    }

    /**
     * Undocumented function
     *
     * @param [type] $var
     * @return void
     */
    public static function var_export_log($var) :void
    {
        error_log(var_export($var, true));
    }

}
