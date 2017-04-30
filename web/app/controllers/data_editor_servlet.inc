<?php

/**
 *
 */
class DataEditorServlet extends Servlet
{
    /**
     *
     * @return string
     */
    public function index(): string
    {
        return $this->foward('editor_pain.inc', []);
    }

    /**
     *
     * @return string
     */
    public function open(int $pid, DataEditorRequest $req, DataEditorProcess $process): string
    {
        $data = array();
        $data['proj_id'] = $pid;

        // TODO: レコードの追加・編集機能の実装
        // TODO: 表示テンプレートの実装
        $tmpl_list = $process->getTemplates($pid);
        $selected_tmpl = key( $tmpl_list ); // とりあえず先頭要素のキーを取得して選択されているものとする
        $data['tbl_tmpl'] = //( $parm_arr['tbl_list'][$tbl_id] == 'skills_view' ) ?
        //             full_path( sprintf( APP_ROOT . '/templates/proj%03d/', $proj_id ).$tmpl_list[ $selected_tmpl ] ) :
        full_path(RESOURCE_ROOT . '/templates/default_template.php');

        // リクエストスコープ相当の配列にデータを格納
        $data['tbl_data'] = $process->listData( $pid, $req->get('tab') );
        $data['tbl_data']['default'] = Arr::combine($data['tbl_data']['form'], null,'default');

        return $this->foward('editor_pain.inc', $data);
    }

    /**
     *
     * @param DataEditorRequest $req
     * @param DataEditorProcess $process
     */
    public function modify(DataEditorRequest $req, DataEditorProcess $process)
    {

        var_export_log($req->getAll());

        return $this->redirectBack();
    }

}
