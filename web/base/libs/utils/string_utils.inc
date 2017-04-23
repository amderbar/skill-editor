<?php
class Str {

    public static function camel(string $str):string
    {
        return lcfirst(strtr(ucwords(strtr($str, ['_' => ' '])), [' ' => '']));
    }

    public static function snake(string $str):string
    {
        return ltrim(strtolower(preg_replace('/[A-Z]/', '_\0', $str)), '_');
    }

    public static function kabab(string $str):string
    {
        return ltrim(strtolower(preg_replace('/[A-Z]/', '-\0', $str)), '-');
    }

}