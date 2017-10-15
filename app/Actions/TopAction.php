<?php

namespace Amderbar\App\Actions;

use Amderbar\Lib\Action;
use Amderbar\App\Requests\TopRequest;

/**
 *
 * @author amderbar
 *
 */
class TopAction extends Action
{
    /**
     *
     * @param IndexRequest $req
     * @return string
     */
    public function index(TopRequest $req) :string
    {
        return $this->foward('top_page.phtml', [$req->get('pid')]);
    }
}
