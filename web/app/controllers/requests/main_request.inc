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
                'tbl_id' => ['integer'],
        ];
    }
}