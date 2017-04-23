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
        $data['proj_list'] = $process->listDB();

        if ($req->get('pid')) {
            $data['focus_pid'] = $req->get('pid');
            $data['tbl_list'] = $process->listUsrTables($req->get('pid'));
        }

        return $this->foward('main_panel.inc', $data);
    }
}
