<?php

/**
 *
 */
class MainServlet extends Servlet {

    /**
    *
    */
    public function doGet($req='') {
        $REQ_SCOPE = array();
        $proj_list = $this->db_editor->listDB();
        $REQ_SCOPE['proj_list'] = $proj_list;
        $tmpl_list = null;
        // リクエストパラメータpidがセットされている時、そのpidのプロジェクトが開かれる
        if (isset($_GET['pid']) && $_GET['pid'] != '') {
            $current_proj_id = intval($_GET['pid']);
            if( is_null( $this->db_editor->open( $current_proj_id ) ) ){
                return $this->redirect($_SERVER['PHP_SELF']);
            }
            $table_list = $this->db_editor->listUsrTables($current_proj_id);
            // リクエストスコープ相当の配列にデータを格納
            $REQ_SCOPE['opend_id'] = $current_proj_id;
            $REQ_SCOPE['current_proj_tbl_list'] = $table_list;
        }
        return $this->foward('view/main_panel.inc', $REQ_SCOPE);
    }

    /**
    *
    */
    public function doPost($req='') {
        pre_dump($_POST);
        if (isset($_POST['fMode'])) {
            if ($_POST['fMode'] == 'del-prj') {
                // プロジェクト削除時
                // 本当はここで入力値チェックをする
                // CSRF対策のトークンチェックも必要
                $proj_id = isset($_POST['pid']) ? intval($_POST['pid']) : null;
                if ($proj_id) {
                    $this->db_editor->deleteDB($proj_id);
                }
            }
        }
        return $this->redirect($_SERVER["REQUEST_URI"]);
        // return $this->doGet($req);
    }
}
