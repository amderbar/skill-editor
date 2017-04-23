<?php
/**
 *
 * @author amderbar
 *
 */
class TopRequest extends Request
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
            'proj_name' => ['string', 'max(100)', 'min(1)']
        ];
    }
}
