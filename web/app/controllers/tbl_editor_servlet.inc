<?php

/**
 *
 */
class TableEditorServlet extends Servlet
{
    /**
     *
     * @return string
     */
    public function open(TableEditorRequest $req, TableEditorProcess $process): string
    {
        $data['proj_id'] = $req->uriParam('pid');
        $data['proj_name'] = $process->projName($data['proj_id']);

        $data['data_types'] = $process->getDataTypeList();
        // FIXME: hiddenまで入力形式に出てる
        $data['form_types'] = Html::FROM_TYPES;

        // 列リストデータ
        $tbl_list = $process->listUsrTables($data['proj_id']);
        $data['col_list'] = [];
        foreach ($tbl_list as ['id' => $tbl_num, 'tbl_name' => $tbl_name]) {
            $data['col_list'][$tbl_num] = ['tbl_name' => $tbl_name, 'colums' => []];
            foreach ($process->listColumns($pid, $tbl_num) as $colnum) {
                if ($colnum['actual_name'] == 'id') {
                    continue;
                }
                $data['col_list'][$tbl_num]['colums'][$colnum['id']] = $colnum['col_name'];
            }
        }

        // プレビュー用表示テンプレート
        $data['tbl_tmpls'] = ['default' => full_path(RESOURCE_ROOT . '/templates/default_template.php')];
        foreach ($process->getAllTemplates($data['proj_id']) as $tmpl) {
            $data['tbl_tmpls'][$tmpl['tmpl_name']]
            = full_path(sprintf(RESOURCE_ROOT . '/templates/proj%03d/', $tmpl['proj_id']) . $tmpl['tmpl_name']);
        }

        // プレビュー用ダミーデータ
        $dummy_key = 'c' . Str::fnv132(uniqid());
        $data['tbl_data'] = [
            'column_config' => [
                [
                    'key' => 'id',
                    'col_name' => 'No.',
                    'type' => 'hidden',
                    'default' => '',
                    'multiple' => false,
                    'ref_dist' => null,
                    'step' => 1,
                    'max' => null,
                    'min' => null,
                    'uniq' => true,
                    'not_null' => true
                ], [
                    'key' => $dummy_key,
                    'col_name' => null,
                    'type' => 'text',
                    'default' => '',
                    'multiple' => false,
                    'ref_dist' => null,
                    'step' => 1,
                    'max' => null,
                    'min' => null,
                    'uniq' => false,
                    'not_null' => false
                ]
            ],
            'data' => [
                ['id' => 1, $dummy_key => ''],
                ['id' => 2, $dummy_key => '']
            ]
        ];

        return $this->foward('new_tbl.inc', $data);
    }

    /**
     *
     * @param TableEditorRequest $req
     * @param TableEditorProcess $process
     */
    public function register(TableEditorRequest $req, TableEditorProcess $process)
    {
        // TODO: 表の新規作成と同時にサイドバーも更新する
        return $this->redirect(Html::sanitizeUrl(APP_ROOT . '/main/', [
            'pid' => $req->get('pid'),
            'tab' => $process->createUserTable(
                $req->get('pid'),
                $req->get('tbl_name'),
                $req->get('def_cols')
            )['tbl_id']
        ]));
    }
}
