<!DOCTYPE html>
<html>
<head>
	<meta charset="UTF-8">
	<link rel="stylesheet" type="text/css" href="css/stylesheet.css">
	<title>Editor on Browser</title>
</head>
<body>
	<header>		 
		<h3 id="page_title">Data Editor on Browser</h3>
		<ul id="system_menu">
			<li><a id="download" href="#" download="save.txt" onclick="handleDownload()">名前をつけて保存</a></li>
			<li><a href="#">システム設定</a></li>
		</ul>
	</header>
	<div id="main">
		<nav id="side_menu">
		<h4>Projects</h4>
		<ul class="side_menu">
			<?php if (count($proj_list) > 0) {
				echo '<li>';
				echo implode('</li><li>',$proj_list);
				echo '</li>';
			} ?>
			<li><input type="button" id="new_proj" onclick="" value="新規作成"></li>
			<li><input type="file" id="file_select" onchange="handleFileSelect(this)"></li>
		</ul>
		</nav>
		<div id="editorArea">
			<?php if ($current_proj) {
				echo '<p>' . htmlentities($current_proj) . '</p>';
			} else {
				echo '<p>プロジェクトを選択してください。</p>';
			} ?>
		</div>
	</div>
	<footer>ver 1.0.0</footer>
	<script src="javascript/editorClient.js"></script>
</body>
</html>