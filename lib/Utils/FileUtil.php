<?php

namespace Amderbar\Lib\Utils;

class FileUtil
{
    /**
     * Undocumented function
     *
     * @param string $path
     * @param bool $newfile
     * @return string
     */
    public static function fullPath(string $path = '', bool $newfile = false) :string
    {
        static $root_path;
        $root_path = $root_path
            ?? str_replace(DIRECTORY_SEPARATOR, '/', realpath($_SERVER['DOCUMENT_ROOT'] . APP_ROOT . '/..'));
    
        if ($path && strpos($path, '/') !== 0) {
            $path = '/' . $path;
        }
        $path = str_replace('/', DIRECTORY_SEPARATOR, $root_path . $path);
        $path = realpath($path) ?: ($newfile ? $path : '');
        return str_replace(DIRECTORY_SEPARATOR, '/', $path);
    }

    /**
     * http://doop-web.com/blog/archives/1182 からコピペし、改造した
     * ファイルの更新日時をリクエストパラメータ風にファイル名に付け加える関数
     */
    public static function addFilemtime(string $filename) :string
    {
        return file_exists($filename)
            ? $filename . '?date='. date('YmdHis', filemtime($filename))
            : $filename;
    }
}
