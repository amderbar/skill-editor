<?php

namespace Amderbar\App\Requests;

/**
 *
 * @author amderbar
 *
 */
class MainRequest extends MyRequest
{
    /**
     *
     * {@inheritDoc}
     * @see Request::rules()
     */
    public function rules() :array
    {
        return [
                'pid' => ['integer'],
                'tbl_id' => ['integer'],
        ];
    }
}
