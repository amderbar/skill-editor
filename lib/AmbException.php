<?php

namespace Amderbar\Lib;

use RuntimeException;

/**
 *
 * @author amderbar
 *
 */
class AmbException extends RuntimeException
{
    public function __construct(string $message)
    {
        parent::__construct($message);
    }
}
