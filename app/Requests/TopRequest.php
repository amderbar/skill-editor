<?php

namespace Amderbar\App\Requests;

/**
 *
 * @author amderbar
 *
 */
class TopRequest extends MyRequest
{
    /**
     *
     * {@inheritDoc}
     * @see Request::rules()
     */
    public function rules() :array
    {
        return [
            'pid' => ['integer']
        ];
    }
}
