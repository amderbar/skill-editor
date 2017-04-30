<?php
/**
 *
 * @author amderbar
 *
 */
abstract class Request
{
    protected $queryParam;
    protected $formParam;
    protected $file;
    protected $errors;

    /**
     *
     */
    public function __construct()
    {
        $this->queryParam = $_GET;
        $this->formParam = $_POST;
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
        return ($this->formParam + $this->queryParam);
    }

    /**
     *
     * @param string $key
     * @return mixed
     */
    public function get(string $key)
    {
        return $this->formParam[$key] ?? ($this->queryParam[$key] ?? null);
    }

    /**
     *
     * @param string $key
     * @return mixed
     */
    public function query(string $key)
    {
        return $this->queryParam[$key] ?? null;
    }

    /**
     *
     * @param string $key
     * @return mixed
     */
    public function input(string $key)
    {
        return $this->formParam[$key] ?? null;
    }

    /**
     *
     * @param string $key
     * @return boolean
     */
    public function has(string $key) :bool
    {
        return array_key_exists($key, $this->formParam + $this->queryParam);
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
