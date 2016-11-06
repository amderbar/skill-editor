<?php
/**
* session check.
*/
require_once($_SERVER['DOCUMENT_ROOT'].'/skill_editor/common.php');

/**
* 
*/
require_once(full_path('controllers/servlet.php'));
require_once(full_path('models/db_editor.php'));

/**
 * 
 */
class IndexServlet extends Servlet {
    /**  */
    private static $db_editor = null;

    /**
    * 
    */
    public static function doGet($req='') {
        $REQ_SCOPE = array();
        // リクエストパラメータidがセットされている時、そのidのプロジェクトが開かれる
        if (isset($_GET['id']) && $_GET['id']!='') {
            // リクエストスコープ相当の配列にidを格納
            $REQ_SCOPE['proj_id'] = intval($_GET['id']);
        }
        return self::foward('view/index_page.php', $REQ_SCOPE);
    }

    /**
    * 
    */
    public static function doPost($req='') {
        $proj_id = isset($_GET['id']) ? intval($_GET['id']) : null;
        if (isset($_POST['proj-name']) && ($_POST['proj-name'] != '')) {
            // プロジェクトの新規作成時
            $proj_id = self::$db_editor->registerDB($_POST['proj-name']);
            echo $proj_id;
            return;
        } elseif (isset($_FILES['tmpl-file'])) {
            // テンプレートの登録時
            $tmpl_name = self::upLoadFile($proj_id);
            self::$db_editor->registerTemplate($proj_id,$tmpl_name);
        } elseif (isset($_POST['save'])) {
            // データの上書き保存時
            self::saveData($proj_id);
        }
        return self::redirect($_SERVER["REQUEST_URI"]);
        // return self::doGet();
    }

    /**
    * サーブレットの初期設定メソッド
    */
    public static function setup($arg='') {
        if (self::$db_editor == null) {
            self::$db_editor = new DBEditor();
            // self::$db_editor->dropRoot();
        }
    }

    /**
    * ファイルアップロード受付メソッド
    */
    private static function upLoadFile($proj_id) {
        // ファイル名についてのその他のバリデーションが必要
        switch ($_FILES['tmpl-file']['error']) {
            case UPLOAD_ERR_OK:
                $uploaddir = sprintf('resources/templates/proj%03d',$proj_id);
                $file_name = basename($_FILES['tmpl-file']['name']);
                $uploadfile = full_path($uploaddir) .'/'. $file_name;
                if (move_uploaded_file($_FILES['tmpl-file']['tmp_name'], $uploadfile)) {
                    return $file_name;
                } else {
                    die("Possible file upload attack!\n");
                }
                break;
            default:
                echo 'File Upload Failed'.PHP_EOL;
                break;
        }
    }

    /**
    * データ保存メソッド.現状ではSNTRPG_Skills専用
    * しかも既存データの更新は特に考えてない
    */
    private static function saveData($proj_id) {
        // POSTデータに対するバリデーションが必要
        $_POST['save'] = null;
        $column_map = array_keys($_POST);
        array_shift($column_map);
        // pre_dump($column_map);
        $new_data = array();
        $new_data['skills_view'] = call_user_func_array('array_map',$_POST);
        // pre_dump($new_data['skills_view']);
        $old_data = $_SESSION['proj'.$proj_id];
        // 既存データの配列を平滑化
        // さらにデータをキー、idを値とする連想配列化
        foreach ($old_data as $tbl_name => $tbl_data) {
            if ($tbl_name == 'skills') {
                continue;
            }
            $tbl_data = call_user_func_array('array_map',array_merge(array(null),$tbl_data));
            $keys = array_keys($tbl_data);
            if (array_depth($tbl_data) > 1) {
                foreach ($tbl_data[1] as $num => $value) {
                    $tbl_data[$value] = $tbl_data[0][$num];
                }
            } else {
                $tbl_data[$tbl_data[$keys[1]]] = $tbl_data[$keys[0]];
            }
            foreach ($keys as $key) {
                unset($tbl_data[$key]);
            }
            $old_data[$tbl_name] = $tbl_data;
        }
        // pre_dump($old_data['skills']);
        // 親表に関して、新データと既存データの差分を取得
        $table_list = array_keys($old_data);
        foreach ($column_map as $column_name) {
            $tbl_name = $column_name . 's';
            if ($column_name == 'preconditions') {
                $tbl_name = 'conditions';
            }
            if (isset($old_data[$tbl_name])) {
                $new_data[$tbl_name] = array_diff($_POST[$column_name],array_keys($old_data[$tbl_name]));
            }
        }
        // conditions
        foreach ($new_data['conditions'] as $num => $condition) {
            if ($condition == '-') {
                unset($new_data['conditions'][$num]);
            }
        }
        // skills
        $new_data['skills'] = array();
        foreach ($new_data['skills_view'] as $num => $skill) {
            foreach ($skill as $key => $value) {
                $column_name = $column_map[$key];
                if ($column_name == 'preconditions') {
                    $tbl_name = 'conditions';
                    $new_skill['has_preconditions'] =  ($value == '-') ? '0' : '1';
                    unset($new_data['conditions'][$num]);
                    $new_skill['icon'] = '1';
                } else {
                    $tbl_name = $column_name . 's';
                    if (isset($old_data[$tbl_name])) {
                        $new_skill[$column_name] = $old_data[$tbl_name][$value];
                    } else {
                        $new_skill[$column_name] = preg_replace("/\r\n|\r|\n/", "\n", trim($value));
                    }
                }
            }
            $new_skill['list_order'] = strval($num);
            $new_data['skills'][$num] = $new_skill;
        }
        unset($new_data['skills_view']);
        $new_data['skills'] = array_diff_assoc_recursive($new_data['skills'],$old_data['skills']);
        pre_dump($new_data);
    }
}

?>