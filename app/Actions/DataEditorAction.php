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
        return $this->foward('data_editor.inc', []);
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
        // プレビュー用表示テンプレート
        // $data['tbl_tmpls'] = ['table', 'list', 'cards'];
        $data['tbl_tmpls'] = ['table'];
        // foreach ($process->getAllTemplates() as $tmpl) {
        //     $data['tbl_tmpls'][$tmpl['tmpl_name']]
        //     = File::fullPath(sprintf(RESOURCE_ROOT . '/templates/proj%03d/', $tmpl['proj_id']) . $tmpl['tmpl_name']);
        // }

        // リクエストスコープ相当の配列にデータを格納
        $data['tbl_data'] = $process->listData($req->get('tab') );
        $data['tbl_data']['default'] = Arr::combine($data['tbl_data']['meta'], null,'default');

        return $this->foward('data_editor.phtml', $data);
    }

    /**
     *
     * @param DataEditorRequest $req
     * @param DataEditorProcess $process
     */
    public function update(DataEditorRequest $req, DataEditorProcess $process)
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

        $process->updateData($req->get('pid'), $req->get('tab'), $newdata);

        return $this->redirectBack();
    }
}
