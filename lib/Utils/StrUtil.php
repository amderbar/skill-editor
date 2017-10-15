<?php

namespace Amderbar\Lib\Utils;

class StrUtil
{

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

    /**
    * fnv132アルゴリズムでハッシュを作成
    */
    public static function fnv132(string $str) :string
    {
        return hash('fnv132', $str);
    }
}