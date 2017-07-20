<?php

/**
 *
 */
class MainServlet extends Servlet
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

        if (($pid = $req->get('pid')) && in_array($pid, array_column($data['proj_list'], 'id'))) {
            $data['focus_pid'] = $pid;
            $data['tbl_list'] = $process->listUsrTables($pid);
            $data['focus_tbl'] = $req->get('tab') ?? reset($data['tbl_list'])['id'];
        }

        return $this->foward('main_panel.inc', $data);
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
        $pid = $process->registerDB($req->input('proj_name'), $req->user('name'));

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
        // TODO:ここではpidは必須パラメータ。MainServletに移すべきか？
        $process->deleteDB($req->input('pid'), $req->user('name'));

        return $this->redirectBack();
    }
}
