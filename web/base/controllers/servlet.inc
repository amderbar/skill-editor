<?php

/**
 * the super class for servlets.
 */
abstract class Servlet {

    /**
     *
     * @param string $dest
     * @param array $REQ_SCOPE
     * @return string
     */
    protected function foward(string $dest, array $REQ_SCOPE = null) :string
    {
        $URL = getMyTopURL();
        if (isset($REQ_SCOPE)) {
            extract($REQ_SCOPE);
            unset($REQ_SCOPE);
        }

        $dest = (strpos($dest,'/') !== 0) ? '/' . $dest : $dest;

        ini_set('url_rewriter.tags','form=');
        output_add_rewrite_var('_token', uniqid());

        ob_start();
        include(full_path(VIEW_ROOT . $dest));
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
        return $this->redirect(parse_url($_SERVER['HTTP_REFERER'] ?? null, PHP_URL_PATH));
    }
}

?>