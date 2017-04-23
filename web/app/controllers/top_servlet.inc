<?php
/**
 *
 * @author amderbar
 *
 */
class TopServlet extends Servlet
{
    /**
     *
     * @param IndexRequest $req
     * @return unknown
     */
    public function index(TopRequest $req)
    {
        $data = array();
        $data['proj_id'] = $req->get('pid');

        return $this->foward('top_page.inc', $data);
    }

    /**
     * プロジェクトの新規作成
     *
     * @param TopRequest $req
     * @param TopProcess $process
     * @return unknown
     */
    public function createProject(TopRequest $req, TopProcess $process)
    {
        $process->registerDB($req->input('proj_name'));

        return $this->redirectBack();
    }

    /**
     * プロジェクトの削除
     *
     * @param TopRequest $req
     * @param TopProcess $process
     * @return unknown
     */
    public function deleteProject(TopRequest $req, TopProcess $process)
    {
        // TODO:ここではpidは必須パラメータ。MainServletに移すべきか？
        $process->deleteDB($req->input('pid'));

        return $this->redirectBack();
    }

    /**
     * テンプレートの登録
     *
     * @param TopRequest $req
     * @param TopProcess $process
     * @return unknown
     */
    public function registerTemplate(TopRequest $req, TopProcess $process)
    {
        $proj_id = $req->input('proj_id');
        $tmpl_name = $this->upLoadFile($proj_id);
        $process->registerTemplate($proj_id, $tmpl_name);

        return $this->redirectBack();
    }

    /**
     * データの上書き保存
     *
     * @param TopRequest $req
     * @param TopProcess $process
     * @return unknown
     */
    public function save(TopRequest $req, TopProcess $process)
    {
        $process->saveData($req->input('proj_id'));

        return $this->redirectBack();
    }

    /**
    * ファイルアップロード受付メソッド
    */
    private function upLoadFile(int $proj_id)
    {
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
}
