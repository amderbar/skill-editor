<?php
/**
 *
 * @author amderbar
 *
 */
class MainRequest extends Request
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
                'tub_id' => ['integer'],
        ];
    }
}
