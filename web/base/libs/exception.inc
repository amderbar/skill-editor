<?php

/**
 *
 * @author amderbar
 *
 */
class DBManageExeption extends RuntimeException
{
    function __construct( string $message )
    {
        parent::__construct( $message );
    }
}