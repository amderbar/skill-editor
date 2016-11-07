<?php
/**
* session check.
*/
require_once($_SERVER['DOCUMENT_ROOT'].'/skill_editor/gatekeeper.php');

require_once(full_path('models/html_handler.php'));
?>
<!DOCTYPE html>
<html>
<head>
	<meta charset="UTF-8">
	<script src="https://code.jquery.com/jquery-3.0.0.min.js"></script>
	<link rel="stylesheet" type="text/css" href="<?=addFilemtime('css/common.css')?>">
	<link rel="stylesheet" type="text/css" href="<?=addFilemtime('css/fonts.css')?>">
	<link rel="stylesheet" type="text/css" href="<?=addFilemtime('css/index.css')?>">
	<title>Editor on Browser</title>
</head>
<body>
	<header class="with-btns">
		<strong id="top-title"><a href="<?=$URL?>" class="icon-leaf"></a></strong>
		<ul id="system_menu" class="btns">
			<?php if (isset($tmpl_list)) { // プロジェクトを開いている時だけボタンを表示 ?>
				<li><?php HTMLHandler::input_submit('save', '上書き保存', 'editorArea') ?></li>
				<!-- <li><?php
				// if (count($tmpl_list)) {
				// 	HTMLHandler::input_select('aplied_templete', $selected_tmpl'], $REQ_SCOPE['tmpl_list);
				// } else {
				// 	echo 'デフォルトテンプレート'.PHP_EOL;
				// }
				?></li> -->
				<li><input id="tmpl-Btn" value="テンプレート登録" type="button" onclick="openModal(this)"/></li>
			<?php } ?>
			<a href="#" class="btn" id="download" download="save.txt"><li class="icon-download2" title="データダウンロード"></li></a>
			<a href="#" class="btn"><li class="icon-cog" title="システム設定"></li></a>
		</ul>
	</header>
	<main id="main">
		<nav id="side_menu">
			<iframe name="side_menu" src="side_menu.php" scrolling="no" frameborder="no"></iframe>
		</nav>
		<article>
			<iframe name="editor_area" src="editor_area.php<?=(isset($proj_id))?'?id='.$proj_id:''?>" scrolling="no" frameborder="no"></iframe>
		</article>
	</main>
	<footer><strong id="top-title">Data Editor on Browser</strong> <?=VERSION?></footer>
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
	<script src="<?=addFilemtime('js/editorCommon.js')?>"></script>
</body>
</html>