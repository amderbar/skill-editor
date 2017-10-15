<?php

namespace Amderbar\App\Actions;

use Amderbar\Lib\Action;
use Amderbar\App\Requests\DataEditorRequest;
use Amderbar\App\Processes\DataEditorProcess;
use Amderbar\Lib\Utils\ArrUtil as Arr;
use Amderbar\Lib\Utils\FileUtil as File;

/**
 *
 */
class DataEditorAction extends Action
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
    public function open(DataEditorRequest $req, DataEditorProcess $process): string
    {
        $data = array();
        $process->init($data['proj_id'] = $req->uriParam('pid'));
        $data['tab_id'] = $req->get('tab');

        // TODO: レコードの追加・編集機能の実装
        // TODO: 表示テンプレートの実装
        $tmpl_list = $process->getAllTemplates();
        $selected_tmpl = key( $tmpl_list ); // とりあえず先頭要素のキーを取得して選択されているものとする
        $data['tbl_tmpl'] = //( $parm_arr['tbl_list'][$tbl_id] == 'skills_view' ) ?
        //             File::fullPath( sprintf( APP_ROOT . '/templates/proj%03d/', $proj_id ).$tmpl_list[ $selected_tmpl ] ) :
        File::fullPath(VIEW_ROOT . '/default_template.php');

        // リクエストスコープ相当の配列にデータを格納
        $data['tbl_data'] = $process->listData($req->get('tab') );
        $data['tbl_data']['default'] = Arr::combine($data['tbl_data']['column_config'], null,'default_val');

        return $this->foward('editor_pain.phtml', $data);
    }

    /**
     *
     * @param DataEditorRequest $req
     * @param DataEditorProcess $process
     */
    public function modify(DataEditorRequest $req, DataEditorProcess $process)
    {

        $columns = array_column($process->listColumns($req->get('pid'), $req->get('tab')), 'actual_name');
        $newdata = ['insert' => [], 'update' => []];
        foreach ($req->get('id') as $sort_num => $row_id) {
            if ($row_id == 'null') {
                continue;
            }
            $tmp_arr = [];
            foreach ($columns as $col_name) {
                if ($col_name == 'id') {
                    continue;
                }
                if ($param = $req->get($col_name)[$sort_num] ?? null) {
                    $tmp_arr[$col_name] = $param;
                }
            }
            if ($tmp_arr) {
                if ($row_id === '') {
                    $newdata['insert'][] = $tmp_arr;
                } else {
                    $newdata['update'][$row_id] = $tmp_arr;
                }
            }
        }

        $process->modifyData($req->get('pid'), $req->get('tab'), $newdata);

        return $this->redirectBack();
    }
}