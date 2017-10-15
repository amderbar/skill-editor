<?php

namespace Amderbar\App\Requests;

/**
 *
 * @author amderbar
 *
 */
class CreateProjRequest extends MyRequest
{
    /**
     *
     * {@inheritDoc}
     * @see Request::rules()
     */
    public function rules() :array
    {
        return [
            'proj_name' => ['string', 'max(100)', 'min(1)']
        ];
    }
}
