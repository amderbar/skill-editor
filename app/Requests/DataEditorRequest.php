<?php

namespace Amderbar\App\Requests;

/**
 *
 * @author amderbar
 *
 */
class DataEditorRequest extends MyRequest
{
    /**
     *
     * {@inheritDoc}
     * @see Request::rules()
     */
    public function rules() :array
    {
        return [
                'tab' => ['integer'],
        ];
    }
}
