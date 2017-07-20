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
}
