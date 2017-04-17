<?php
/**
* session check.
*/
require_once($_SERVER['DOCUMENT_ROOT'].'/skill_editor/common.inc');
?>
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <script src="https://code.jquery.com/jquery-3.0.0.min.js"></script>
    <link rel="stylesheet" type="text/css" href="<?=addFilemtime('css/common.css')?>">
    <link rel="stylesheet" type="text/css" href="<?=addFilemtime('css/fonts.css')?>">
    <link rel="stylesheet" type="text/css" href="<?=addFilemtime('css/index.css')?>">
    <title><?=APP_NAME?></title>
</head>
<body>
    <header class="with-btns">
        <strong id="top-title"><a href="<?=$URL?>" class="icon-leaf"></a></strong>
        <ul id="system_menu" class="btns">
            <?php if (isset($tmpl_list)) { // プロジェクトを開いている時だけボタンを表示 ?>
                <li><?php HTMLHandler::input_submit('save', '上書き保存', 'editorArea') ?></li>
                <!-- <li><?php
                // if (count($tmpl_list)) {
                //     HTMLHandler::selectbox('aplied_templete', $REQ_SCOPE['tmpl_list'], $selected_tmpl);
                // } else {
                //     echo 'デフォルトテンプレート'.PHP_EOL;
                // }
                ?></li> -->
                <li><input id="tmpl-Btn" value="テンプレート登録" type="button" onclick="openModal(this)"/></li>
            <?php } ?>
            <li><a href="#" class="btn icon-download2" id="download" download="save.txt" title="データダウンロード"></a></li>
            <li><a href="#" class="icon-cog btn" title="システム設定"></a></li>
        </ul>
    </header>
    <iframe id="main" name="main" src="main.php<?=(isset($proj_id))?'?pid='.$proj_id:''?>" scrolling="no" frameborder="no"></iframe>
    <footer><strong id="top-title"><?=APP_NAME?></strong> <?=VERSION?></footer>
    <section class="modal-content" id="new-proj">
        <form action="<?=$_SERVER["REQUEST_URI"]?>" method="POST">
            <fieldset>
                <legend>新規プロジェクト作成</legend>
                <table>
                <tr><td>新規プロジェクト名</td><td><input type="text" name="proj-name"></td></tr>
                <tr>
                <!--<td><input type="submit" value="作成" onclick="hideModal()"></td>-->
                <td><input type="button" value="キャンセル" onclick="hideModal()"></td>
                <td>現在新規プロジェクトの作成はできません</td>
                </tr>
                </table>
            </fieldset>
        </form>
    </section>
    <section class="modal-content" id="new-tmpl">
        <form action="<?=$_SERVER["REQUEST_URI"]?>" method="POST" enctype="multipart/form-data">
            <fieldset>
                <legend>テンプレート登録</legend>
                <table>
                <tr><td>ファイルアップロード</td>
                <td><input type="hidden" name="MAX_FILE_SIZE" value="30000">
                <input type="file" name="tmpl-file"></td></tr>
                <tr>
                <td><input type="submit" value="登録" onclick="hideModal()"></td>
                <td><input type="button" value="キャンセル" onclick="hideModal()"></td>
                </tr>
                </table>
            </fieldset>
        </form>
    </section>
    <div id="modal-overlay"></div>
    <script src="<?=addFilemtime('js/common.js')?>"></script>
</body>
</html>