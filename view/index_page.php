<!DOCTYPE html>
<html>
<head>
	<meta charset="UTF-8">
	<link rel="stylesheet" type="text/css" href="css/stylesheet.css">
	<link rel="stylesheet" type="text/css" href="css/modal.css">
	<title>Editor on Browser</title>
</head>
<body>
	<header class="header">		 
		<h3 id="page_title">Data Editor on Browser</h3>
		<ul id="system_menu">
			<li><a id="download" href="#" download="save.txt" onclick="handleDownload()">名前をつけて保存</a></li>
			<li><a href="#">システム設定</a></li>
		</ul>
	</header>
	<div id="main">
		<nav id="side_menu">
		<div class="header">
			<h4>Projects</h4>
			<ul>
				<li><input type="button" id="now_proj" onclick="openModal(this)" value="新規作成"></li>
			</ul>
		</div>
		<ul class="side_menu">
			<?php foreach ($proj_list as $proj_id => $proj_name) {
				echo '<li><a href="'.parse_url($_SERVER["REQUEST_URI"],PHP_URL_HOST).'?id='.htmlentities($proj_id).'">'.htmlentities($proj_name).'</a></li>'."\n";
			} ?>
			<li><input type="file" id="file_select" onchange="handleFileSelect(this)"></li>
		</ul>
		</nav>
		<div id="editorArea">
		<div class="header">
			<h4>Editor Area</h4>
			<ul>
				<li><input type="button" onclick="" value="ボタン"></li>
			</ul>
		</div>
			<?php if ($proj_template !== null) {
				$tmpl_name = 'view/templates/';
				if (count($proj_template)) {
					$tmpl_name .= $proj_template[0];
				} else {
					// $proj_template[] = 'default_template.php';
					$tmpl_name .= 'default_template.php';
				}
				// echo '<p>' . htmlentities(implode(',',$proj_template)) . '</p>';
				include(full_path($tmpl_name));
			} else {
				echo '<p>プロジェクトを選択してください。</p>';
			} ?>
		</div>
	</div>
	<footer>ver 1.0.0</footer>
	<div id="modal-content">
		<form action="<?=$_SERVER["REQUEST_URI"]?>" method="POST">
			<fieldset>
				<legend>新規プロジェクト作成</legend>
				<table>
				<tr><td>新規プロジェクト名</td><td><input type="text" name="proj_name"></td></tr>
				<tr><td><input type="submit" value="作成" onclick="hideModal()"></td>
				<td><input type="button" value="キャンセル" onclick="hideModal()"></td></tr>
				</table>
			</fieldset>
		</form>
	</div>
	<div id="modal-overlay"></div>
	<script src="javascript/editorClient.js"></script>
	<script src="javascript/editorCommon.js"></script>
</body>
</html>