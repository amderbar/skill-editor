<?php

namespace Amderbar\Lib;

use Amderbar\Lib\Validater;

/**
 *
 * @author amderbar
 *
 */
abstract class Request
{
    protected $user;
    protected $query_param;
    protected $form_param;
    protected $cookie_param;
    protected $uri_param;
    protected $file;
    protected $errors;

    /**
     *
     */
    public function __construct(array $uri_params)
    {
        $this->user = ['name' => $_SERVER['PHP_AUTH_USER'], 'password' => $_SERVER['PHP_AUTH_PW'] ?? null];
        $this->uri_param = $uri_params;
        $this->query_param = $_GET;
        $this->form_param = $_POST;
        $this->coolie_param = $_COOKIE;
        $this->errors = Validater::bulkValidate($this->getAll(), $this->rules());
    }

    /**
     * リクエストに適用するバリデーションルールを取得
     *
     * @return array
     */
    public abstract function rules():array;

    /**
     *
     * @return array
     */
    public function getAll() :array
    {
        return ($this->form_param + $this->query_param + $this->coolie_param + $this->uri_param);
    }

    /**
     *
     * @param string $key
     * @return mixed
     */
    public function get(string $key)
    {
        return $this->getAll()[$key] ?? null;
    }

    /**
     *
     * @param string $key
     * @return mixed
     */
    public function query(string $key)
    {
        return $this->query_param[$key] ?? null;
    }

    /**
     *
     * @param string $key
     * @return mixed
     */
    public function input(string $key)
    {
        return $this->form_param[$key] ?? null;
    }

    /**
     *
     * @param string $key
     * @return mixed
     */
    public function cookie(string $key)
    {
        return $this->cookie[$key] ?? null;
    }

    /**
     *
     * @param string $key
     * @return mixed
     */
    public function uriParam(string $key)
    {
        return $this->uri_param[$key] ?? null;
    }

    /**
     *
     * @return array|string
     */
    public function user(string $key = null)
    {
        if (isset($key)) {
            return $this->user[$key] ?? null;
        }
        return $this->user ?? null;
    }

    /**
     *
     * @param string $key
     * @return boolean
     */
    public function has(string $key) :bool
    {
        return array_key_exists($key, $this->getAll());
    }

    /**
     *
     * @param string $key
     * @return mixed
     */
    public function errors() :array
    {
        return $this->errors ?? [];
    }
}
