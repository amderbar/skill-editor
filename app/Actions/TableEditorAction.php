<?php

namespace Amderbar\App\Actions;

use Amderbar\Lib\Action;
use Amderbar\App\Requests\TableEditorRequest;
use Amderbar\App\Processes\TableEditorProcess;
use Amderbar\Lib\Utils\HtmlUtil as Html;
use Amderbar\Lib\Utils\StrUtil as Str;
use Amderbar\Lib\Utils\FileUtil as File;

/**
 *
 */
class TableEditorAction extends Action
{
    /**
     *
     * @return string
     */
    public function open(TableEditorRequest $req, TableEditorProcess $process): string
    {
        $process->init($data['proj_id'] = $req->uriParam('pid'));
        $data['proj_name'] = $process->projName();

        // FIXME: hiddenまで入力形式に出てる
        $data['form_types'] = Html::FROM_TYPES;

        // 列リストデータ
        $data['col_list'] = $process->listColumns();

        // プレビュー用表示テンプレート
        $data['tbl_tmpls'] = ['default' => File::fullPath(VIEW_ROOT . '/default_template.php')];
        // foreach ($process->getAllTemplates() as $tmpl) {
        //     $data['tbl_tmpls'][$tmpl['tmpl_name']]
        //     = File::fullPath(sprintf(RESOURCE_ROOT . '/templates/proj%03d/', $tmpl['proj_id']) . $tmpl['tmpl_name']);
        // }

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

        return $this->foward('new_tbl.phtml', $data);
    }

    /**
     *
     * @param TableEditorRequest $req
     * @param TableEditorProcess $process
     */
    public function register(TableEditorRequest $req, TableEditorProcess $process)
    {
        // TODO: 表の新規作成と同時にサイドバーも更新する
        $proj_id = $req->get('pid');
        $process->init($proj_id);
        return $this->redirect(Html::sanitizeUrl(APP_ROOT . '/main/', [
            'pid' => $proj_id,
            'tab' => $process->createUserTable(
                $req->get('tbl_name'),
                $req->get('def_cols')
            )['tbl_id']
        ]));
    }
}
