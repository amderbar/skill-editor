<?php

namespace Amderbar\Lib\Utils;

use \ReflectionClass;
use \ReflectionFunction;

class FunctionalUtil
{
    /**
    * Returns a callable which formatted from instance method format.
    *
    * @param string|array $callable
    * @param callable $formatter
    * @return mixed
    */
    public static function getCallableMethod($callable, callable $formatter)
    {
        if (is_array($callable) || (is_string($callable) && strpos($callable, '::') !== false)) {
            [$class, $method] = is_string($callable) ? explode('::', $callable) : $callable;
            return $formatter($class, $method);
        }
        return null;
    }

    /**
    * Returns arguments accepted by a callable.
    *
    * @param string|array $callable
    * @return array
    */
    public static function getParameters($callable) :array
    {
        return (self::getCallableMethod($callable, function (string $class, string $method) {
            return (new ReflectionClass($class))->getMethod($method);
        }) ?? new ReflectionFunction($callable))
        ->getParameters();
    }

    /**
    * Returns callable array for call_user_func(_array).
    *
    * @param string|array $callable
    * @return callable
    */
    public static function getInstanceMethod($callable) :callable
    {
        return self::getCallableMethod($callable, function (string $class, string $method) {
            $fqdn = (new ReflectionClass($class))->getName();
            return [new $fqdn(), $method];
        }) ?? $callable;
    }
}
