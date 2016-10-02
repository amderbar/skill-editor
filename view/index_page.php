<?php
	require_once(full_path('view/form_helper.php'));
	require_once(full_path('view/editor_area.php'));

	/**
	* 
	*/
	function makeSideItems($projects,$opened_proj_tbls = null) {
		foreach ($projects as $proj_id => $proj_name) {
			$href = $GLOBALS['URL'] .'?id='.htmlentities($proj_id);
			echo '<li>';
			echo '<a href="'.$href.'">'.htmlentities($proj_name).'</a>';
			echo '<input class="deleteBtn" value="削除" type="button"/>';
			if (isset($_GET['id']) and $_GET['id'] == $proj_id) {
				echo '<ul>'.PHP_EOL;
				foreach ($opened_proj_tbls as $tbl_num => $tbl_name) {
					echo '<li><a href="'.$href.'#tab'.htmlentities($tbl_num).'" onclick="changeTab(this);">'.htmlentities($tbl_name).'</a></li>'.PHP_EOL;
				}
				echo '</ul>';
			}
			echo '</li>'.PHP_EOL;
		}
	}
?>
<!DOCTYPE html>
<html>
<head>
	<meta charset="UTF-8">
	<link rel="stylesheet" type="text/css" href="<?=addFilemtime('css/stylesheet.css')?>">
	<link rel="stylesheet" type="text/css" href="<?=addFilemtime('css/modal.css')?>">
	<link rel="stylesheet" type="text/css" href="<?=addFilemtime('css/editor_area.css')?>">
	<title>Editor on Browser</title>
</head>
<body>
	<header class="header">
		<h1 id="top-title"><a href="<?=$GLOBALS['URL']?>">Data Editor on Browser</a></h1>
		<ul id="system_menu">
			<?php if (isset($REQ_SCOPE['tmpl_list'])) { // プロジェクトを開いている時だけボタンを表示 ?>
				<li><?php FormHelper::input_submit('save', '上書き保存', 'editorArea') ?></li>
				<!-- <li><?php
				// if (count($REQ_SCOPE['tmpl_list'])) {
				// 	FormHelper::input_select('aplied_templete', $REQ_SCOPE['selected_tmpl'], $REQ_SCOPE['tmpl_list']);
				// } else {
				// 	echo 'デフォルトテンプレート'.PHP_EOL;
				// }
				?></li> -->
				<li><input id="tmpl-Btn" value="テンプレート登録" type="button" onclick="openModal(this)"/></li>
			<?php } ?>
			<li><a id="download" href="" download="save.txt" onclick="handleDownload()">データダウンロード</a></li>
			<li><a href="#">システム設定</a></li>
		</ul>
	</header>
	<main id="main">
		<nav id="side_menu">
			<header class="header">
				<h2>Projects</h2>
				<ul>
					<li><input type="button" id="proj-Btn" onclick="openModal(this)" value="新規作成"></li>
				</ul>
			</header>
			<ul class="side-menu">
				<?php if (isset($REQ_SCOPE['current_proj_data_list'])) {
					makeSideItems($REQ_SCOPE['proj_list'],$REQ_SCOPE['current_proj_tbl_list']);
				} else {
					makeSideItems($REQ_SCOPE['proj_list']);
				} ?>
				<li><input type="file" id="file_select" onchange="handleFileSelect(this)"></li>
			</ul>
		</nav>
		<article id="editor">
			<?php if (isset($REQ_SCOPE['tmpl_list'])) {
				$editor_area = new EditorArea($GLOBALS['URL'],$_GET['id']);
				$editor_area->makePage(
					$REQ_SCOPE['current_proj_tbl_list'],
					$REQ_SCOPE['tmpl_list'],
					$REQ_SCOPE['selected_tmpl'],
					$REQ_SCOPE['current_proj_data_list']
				);
			} elseif (condition) {
				# code...
			// } else {
			// 	echo '<p>プロジェクトを選択してください。</p>';
			} ?>
		</article>
	</main>
	<footer>ver 1.0.0</footer>
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
	<script src="<?=addFilemtime('javascript/editorClient.js')?>"></script>
	<script src="<?=addFilemtime('javascript/editorCommon.js')?>"></script>
</body>
</html>