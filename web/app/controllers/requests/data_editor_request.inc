<?php
/**
 *
 * @author amderbar
 *
 */
class DataEditorRequest extends Request
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
