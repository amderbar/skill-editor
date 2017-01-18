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

            if ( empty( $REQ_SCOPE['tbl_list'] ) || ($opened_tab === 'new') ) { // テーブル新規作成タブを開く

                $REQ_SCOPE['new_tab'] = true;
                $REQ_SCOPE['data_types'] = DBEditor::getDataTypeList();
                $REQ_SCOPE['form_types'] = HTMLHandler::$FROM_TYPES;
                $REQ_SCOPE['opened_tab'] = null;
                $REQ_SCOPE['proj_name'] = $this->db_editor->projName($current_proj_id);
                $REQ_SCOPE['col_list'] = array('This Table' => ['0' => 'ID', '-1' => 'Nameless1']);
                // 列リストデータ
                foreach ($REQ_SCOPE['tbl_list'] as $tbl_num => $tbl_name) {
                    $REQ_SCOPE['col_list'][$tbl_name] = array();
                    $col_list = $this->db_editor->listColumns($current_proj_id, $tbl_num);
                    foreach ($col_list as $colnum) {
                        $REQ_SCOPE['col_list'][$tbl_name][$colnum['id']] = $colnum['col_name'];
                    }
                }
                // モック生成 CHECK制約リスト
                $REQ_SCOPE['chk_list'] = array(
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
                $REQ_SCOPE['tbl_tmpls'] = ['default' => full_path('resources/templates/default_template.php')];
                foreach ( $this->db_editor->getAllTemplates() as $tmpl ) {
                    $REQ_SCOPE['tbl_tmpls'][$tmpl['tmpl_name']]
                     = full_path(sprintf('resources/templates/proj%03d/', $tmpl['proj_id']).$tmpl['tmpl_name']);
                }
                // プレビュー用ダミーデータ
                $REQ_SCOPE['tbl_data'] = array(
                    ['rid' => 1, 'Nameless' => ''],
                    ['rid' => 2, 'Nameless' => ''],
                    ['rid' => 3, 'Nameless' => '']
                );

            } else { // 既存テーブルのタブを開く

                if (is_null($opened_tab)) {
                    $opened_tab = current(array_keys($REQ_SCOPE['tbl_list']));
                }
                $tmpl_list = $this->db_editor->getTemplates($current_proj_id);
                $selected_tmpl = key($tmpl_list); // とりあえず先頭要素のキーを取得して選択されているものとする
                // リクエストスコープ相当の配列にデータを格納
                $REQ_SCOPE['tbl_data'] = $this->db_editor->listData($current_proj_id, $opened_tab);
                $REQ_SCOPE['tbl_tmpl'] = ($REQ_SCOPE['tbl_list'][$opened_tab] == 'skills_view') ?
                    full_path(sprintf('resources/templates/proj%03d/', $current_proj_id).$tmpl_list[$selected_tmpl]) :
                    full_path('resources/templates/default_template.php');
                $REQ_SCOPE['opened_tab'] = $opened_tab;
                // 肝心のデータはセッションスコープにも入れておく
                $_SESSION['proj'.$current_proj_id][$opened_tab] = $REQ_SCOPE['tbl_data'];

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
            $tbl_id = $this->db_editor->addUserTable($proj_id, $tbl_name, $def_cols);
            $redirect_url = HTMLHandler::sanitizeUrl(getMyTopURL(), ['pid' => $proj_id, 'tab' => $tbl_id]);

        } else {

            $redirect_url = getMyTopURL();

        }
        // return $this->foward('view/editor_pain.inc', $REQ_SCOPE);
        return $this->redirect($redirect_url);
    }
}

?>