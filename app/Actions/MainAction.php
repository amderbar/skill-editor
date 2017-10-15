<?php

namespace Amderbar\App\Actions;

use Amderbar\Lib\Action;
use Amderbar\App\Requests\MainRequest;
use Amderbar\App\Requests\CreateProjRequest;
use Amderbar\App\Requests\TopRequest;
use Amderbar\App\Processes\MainProcess;

/**
 *
 */
class MainAction extends Action
{
    /**
     *
     * @param MainRequest $req
     * @param MainProcess $process
     * @return string
     */
    public function index(MainRequest $req, MainProcess $process): string
    {       
        $data = array();
        $data['proj_list'] = $process->listDB($req->user());

        if (($pid = $req->get('pid')) && array_reduce($data['proj_list'], function (?bool $carry, \stdClass $item) use ($pid) {
            return $carry || $item->proj_id == $pid;
        })) {
            $data['focus_pid'] = $pid;
            $data['tbl_list'] = $process->listUsrTables($pid);
            $data['focus_tbl'] = $req->get('tab') ?? (reset($data['tbl_list'])->tbl_id ?? null);
        }

        return $this->foward('main_panel.phtml', $data);
    }

    /**
     * プロジェクトの新規作成
     *
     * @param CreateProjRequest $req
     * @param MainProcess $process
     * @return unknown
     */
    public function createProject(CreateProjRequest $req, MainProcess $process)
    {
        $pid = $process->registerDB($req->input('proj_name'), $req->user()->user_id);

        // TODO:リダイレクト先では作成したDBをテーブル新規作成で開くようにする
        return $this->redirectBack();
    }

    /**
     * プロジェクトの削除
     *
     * @param TopRequest $req
     * @param MainProcess $process
     * @return unknown
     */
    public function deleteProject(TopRequest $req, MainProcess $process)
    {
        $process->deleteDB($req->input('pid'), $req->user()->user_id);

        return $this->redirectBack();
    }
}
