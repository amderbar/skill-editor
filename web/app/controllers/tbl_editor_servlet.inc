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
        $data['form_types'] = Html::$FROM_TYPES;

        // 列リストデータ
        $tbl_list = $process->listUsrTables($data['proj_id']);
        $data['col_list'] = array( 'This Table' => [ '0' => 'ID', '-1' => 'Nameless1' ] );
        foreach ( $data['tbl_list'] as ['id' => $tbl_num, 'tbl_name' => $tbl_name] ) {
            $data[ 'col_list' ][ $tbl_name ] = array();
            $col_list = $process->listColumns( $pid, $tbl_num );
            foreach ( $col_list as $colnum ) {
                if ( $colnum['actual_name'] == 'id' ) {
                    continue;
                }
                // FIXME: 同名のテーブルがあるとそれらを区別できない
                $data['col_list'][ $tbl_name ][ $colnum['id'] ] = $colnum['col_name'];
            }
        }

        // プレビュー用表示テンプレート
        $data['tbl_tmpls'] = [ 'default' => full_path(RESOURCE_ROOT . '/templates/default_template.php') ];
        foreach ($process->getAllTemplates($data['proj_id']) as $tmpl) {
            $data['tbl_tmpls'][ $tmpl[ 'tmpl_name' ] ]
            = full_path( sprintf(RESOURCE_ROOT . '/templates/proj%03d/', $tmpl['proj_id'] ).$tmpl['tmpl_name'] );
        }

        // プレビュー用ダミーデータ
        $data['tbl_data'] = array(
                'col_name' => array(
                        'id' => 'No.',
                        'col' => 'Nameless'
                ),
                'data' => array(
                        ['id' => 1, 'col' => ''],
                        ['id' => 2, 'col' => ''],
                        ['id' => null, 'col' => null]
                )
        );

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
