<?php

namespace Amderbar\App\Requests;

use Amderbar\Lib\Request;
use Amderbar\App\Processes\Process;
use stdClass;

/**
 *
 * @author amderbar
 *
 */
abstract class MyRequest extends Request
{
    /**
     * @var Process
     */
     private $proc;
     
    /**
     *
     */
    public function __construct(array $uri_params)
    {
        parent::__construct($uri_params);
        $this->proc = new Process();
    }

    /**
     *
     * @return stdClass
     */
    public function user(string $key = null) :stdClass
    {
        ['name' => $name, 'password' => $passwd] = parent::user();
        return $this->proc->getUser(['name' => $name])
            ?? $this->proc->registerUser($name, $passwd ?? '');
    }
}