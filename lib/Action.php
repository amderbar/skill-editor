<?php

namespace Amderbar\Lib;

use Amderbar\Lib\Utils\FileUtil as File;

/**
 * the super class for servlets.
 */
abstract class Action
{
    /**
     *
     * @param string $dest
     * @param array $REQ_SCOPE
     * @return string
     */
    protected function foward(string $dest, array $REQ_SCOPE = null) :string
    {
        $URL = parse_url($_SERVER["REQUEST_URI"], PHP_URL_PATH);
        if (isset($REQ_SCOPE)) {
            extract($REQ_SCOPE);
        }

        $dest = (strpos($dest,'/') !== 0) ? '/' . $dest : $dest;

        ini_set('url_rewriter.tags','form=');
        output_add_rewrite_var('_token', uniqid());

        ob_start();
        include(File::fullPath(VIEW_ROOT . $dest));
        return ob_get_clean();
    }

    /**
     *
     * @param string $dist
     */
    protected function redirect(string $dist = null)
    {
        if (empty($dist)) {
            $dist = getMyTopURL();
        }
        $redirect_uri = empty($_SERVER["HTTPS"]) ? "http://" : "https://";
        $redirect_uri .= $_SERVER["HTTP_HOST"] . $dist;
        return header('Location: ' . $redirect_uri);
    }

    /**
     *
     */
    protected function redirectBack()
    {
        return $this->redirect(call_user_func(function (string $url):string
        {
            $url = parse_url($url);
            return ($url['path'] ?? '') . ($url['query'] ?? '' ? '?'.$url['query'] : '');
        }, $_SERVER['HTTP_REFERER'] ?? ''));
    }
}

?>