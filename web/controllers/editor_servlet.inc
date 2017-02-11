<?php
/**
 * session check.
 */
require_once($_SERVER['DOCUMENT_ROOT'].'/skill_editor/common.inc');

/**
 *
 */
require_once(full_path('controllers/servlet.inc'));

/**
 *
 */
class EditorServlet extends Servlet {

    /**
     *
     */
    public function doGet($req='') {
        $REQ_SCOPE = array();
        // リクエストパラメータpidがセットされている時、そのpidのプロジェクトが開かれる
        // ただしProject ID は常に0より大なので、0も含め空なら何もせずに画面表示に転送する
        if ( !empty( $_GET['pid'] ) ) {

            $current_proj_id = intval( $_GET['pid'] );
            if ( is_null( $this->db_editor->open( $current_proj_id ) ) ) {
                return $this->redirect( $_SERVER['PHP_SELF'] );
            }
            $REQ_SCOPE['proj_id'] = $current_proj_id;
            $REQ_SCOPE['tbl_list'] = $this->db_editor->listUsrTables( $current_proj_id );

            if ( isset( $_GET['tab'] ) ) {
                $opened_tab = ( $_GET['tab'] == 'new' ) ? 'new' : intval( $_GET['tab'] );
            } else {
                $opened_tab = null;
            }

            if ( empty( $REQ_SCOPE['tbl_list'] ) || ( $opened_tab === 'new' ) ) {
                // テーブル新規作成タブを開く
                $REQ_SCOPE += $this->makeNewTblParmArr( $current_proj_id, $REQ_SCOPE['tbl_list'] );

            } else {
                // 既存テーブルのタブを開く
                $opened_tab = $opened_tab ?? current( array_keys( $REQ_SCOPE['tbl_list'] ) );
                $REQ_SCOPE += $this->makeExistingTblParmArr( $current_proj_id, $opened_tab );

            }
        }

        return $this->foward('view/editor_pain.inc', $REQ_SCOPE);

    }



    /**
     *
     */
    public function doPost($req='') {
        // Project ID は常に0より大なので、0も含め空なら何もせずに画面表示に転送する
        if (!empty($_POST['pid'])) { // テーブルの新規作成時

            // 本当はここで入力値チェックをする
            $proj_id = intval($_POST['pid']);
            if( is_null( $this->db_editor->open( $proj_id ) ) ){
                return $this->redirect(getMyTopURL());
            }
            $tbl_name =  $_POST['tbl_name'];
            $def_cols = $_POST['def_cols'];
            $tbl_info = $this->db_editor->addUserTable($proj_id, $tbl_name, $def_cols);
            $redirect_url = HTMLHandler::sanitizeUrl(getMyTopURL(), ['pid' => $proj_id, 'tab' => $tbl_info['tbl_id'] ]);

        } else {

            $redirect_url = getMyTopURL();

        }
        // TODO: 表の新規作成と同時にサイドバーも更新する
        // return $this->foward('view/editor_pain.inc', $REQ_SCOPE);
        return $this->redirect($redirect_url);
    }



    /**
     * @param int $proj_id
     * @param array $tbl_list
     * @return array
     */
    private function makeNewTblParmArr( int $proj_id, array $tbl_list ) :array {

        $parm_arr = array();
        $parm_arr['new_tab'] = true;
        $parm_arr['data_types'] = DBEditor::getDataTypeList();
        $parm_arr['form_types'] = HTMLHandler::$FROM_TYPES;
        $parm_arr['opened_tab'] = null;
        $parm_arr['proj_name'] = $this->db_editor->projName($proj_id);

        // 列リストデータ
        $parm_arr['col_list'] = array( 'This Table' => [ '0' => 'ID', '-1' => 'Nameless1' ] );
        foreach ( $tbl_list as $tbl_num => $tbl_name ) {
            $parm_arr[ 'col_list' ][ $tbl_name ] = array();
            $col_list = $this->db_editor->listColumns( $proj_id, $tbl_num );
            foreach ( $col_list as $colnum ) {
                if ( $colnum['actual_name'] == 'id' ) {
                    continue;
                }
                $parm_arr['col_list'][ $tbl_name ][ $colnum['id'] ] = $colnum['col_name'];
            }
        }

        // モック生成 CHECK制約リスト
        $parm_arr['chk_list'] = array(
                'for String' => [
                        'maxlen' => 'N文字以内',
                        'minlen' => 'N文字以上'
                ],
                'for Numelic' => [
                        'odd' => '奇数',
                        'even' => '偶数',
                        'primery' => '素数'
                ],
                'for Datetime' => [
                        'past' => '今より過去'
                ]
        );

        // プレビュー用表示テンプレート
        $parm_arr['tbl_tmpls'] = [ 'default' => full_path('resources/templates/default_template.php') ];
        foreach ( $this->db_editor->getAllTemplates() as $tmpl ) {
            $parm_arr['tbl_tmpls'][ $tmpl[ 'tmpl_name' ] ]
            = full_path( sprintf('resources/templates/proj%03d/', $tmpl['proj_id'] ).$tmpl['tmpl_name'] );
        }

        // プレビュー用ダミーデータ
        $parm_arr['tbl_data'] = array(
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

        return $parm_arr;

    }



    /**
     * @param int $proj_id
     * @param int $tbl_id
     * @return array
     */
    private function makeExistingTblParmArr( int $proj_id, int $tbl_id ) :array {
        // TODO: レコードの追加・編集機能の実装
        // TODO: 表示テンプレートの実装
        $tmpl_list = $this->db_editor->getTemplates( $proj_id );
        $selected_tmpl = key( $tmpl_list ); // とりあえず先頭要素のキーを取得して選択されているものとする
        $parm_arr['tbl_tmpl'] = //( $parm_arr['tbl_list'][$tbl_id] == 'skills_view' ) ?
//             full_path( sprintf( 'resources/templates/proj%03d/', $proj_id ).$tmpl_list[ $selected_tmpl ] ) :
            full_path( 'resources/templates/default_template.php' );

        // リクエストスコープ相当の配列にデータを格納
        $parm_arr['opened_tab'] = $tbl_id;
        $parm_arr['tbl_data'] = $this->db_editor->listData( $proj_id, $tbl_id );

        // 肝心のデータはセッションスコープにも入れておく
        $_SESSION['proj'.$proj_id][$tbl_id] = $parm_arr['tbl_data'];

        return $parm_arr;
    }

}

?>