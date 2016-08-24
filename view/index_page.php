<?php
	require_once(full_path('view/form_helper.php'));
	$url = parse_url($_SERVER["REQUEST_URI"],PHP_URL_HOST);
?>
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
			<li><a id="download" href="#" download="save.txt" onclick="handleDownload()">データダウンロード</a></li>
			<li><a href="#">システム設定</a></li>
		</ul>
	</header>
	<div id="main">
		<nav id="side_menu">
		<div class="header">
			<h4>Projects</h4>
			<ul>
				<li><input type="button" id="proj-Btn" onclick="openModal(this)" value="新規作成"></li>
			</ul>
		</div>
		<ul class="side_menu">
			<?php foreach ($proj_list as $proj_id => $proj_name) {
				$href = $url .'?id='.htmlentities($proj_id);
				echo '<li><a href="'.$href.'">'.htmlentities($proj_name).'</a><input class="deleteBtn" value="削除" type="button"/></li>'.PHP_EOL;
			} ?>
			<li><input type="file" id="file_select" onchange="handleFileSelect(this)"></li>
		</ul>
		</nav>
		<div>
		<div class="header">
			<h4>Editor Area</h4>
			<?php if ($proj_template !== null) { // プロジェクトを開いている時だけボタンを表示 ?>
			<ul>
				<li><?php FormHelper::input_submit('save', '上書き保存', 'editorArea') ?></li>
				<li><?php if (count($proj_template)) {
					FormHelper::input_select('aplied_templete', $selected_tmpl, $proj_template);
				} else {
					echo 'デフォルトテンプレート'.PHP_EOL;
				} ?></li>
				<li><input id="tmpl-Btn" value="テンプレート登録" type="button" onclick="openModal(this)"/></li>
			</ul>
			<?php } ?>
		</div>
			<?php if ($proj_template !== null) {
				$tmpl_name = 'view/templates/';
				if (count($proj_template)) {
					$tmpl_name .= sprintf('proj%03d/',$current_proj_id).$proj_template[$selected_tmpl];
				} else {
					// $proj_template[] = 'default_template.php';
					$tmpl_name .= 'default_template.php';
				}
				// 以下は現状SNTRPG_Skills専用
				// echo '<p>' . htmlentities(implode(',',$proj_template)) . '</p>';
				echo '<form action="'.$url .'?id='.htmlentities($current_proj_id).'" method="POST" id="editorArea">'.PHP_EOL;
				FormHelper::makeDatalist('timings',$data_list['timings']);
				FormHelper::makeDatalist('renges',$data_list['renges']);
				FormHelper::makeDatalist('targets',$data_list['targets']);
				$tmpl = file_get_contents(full_path($tmpl_name));
				foreach ($data_list['skills_view'] as $data_row) {
					$tmpl_row = $tmpl;
					foreach ($data_row as $key => $value) {
						if ($key == 'icon') {
							$value = 'img/'.$value;
						}
						$tmpl_row = str_replace('{'.$key.'}',htmlentities($value),$tmpl_row);
					}
					echo $tmpl_row.PHP_EOL;
				}
				echo '<input value="新規レコード" type="button"/>'.PHP_EOL;
				echo '</form>'.PHP_EOL;
			} else {
				echo '<p>プロジェクトを選択してください。</p>';
			} ?>
		</div>
	</div>
	<footer>ver 1.0.0</footer>
	<div class="modal-content" id="new-proj">
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
	</div>
	<div class="modal-content" id="new-tmpl">
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
	</div>
	<div id="modal-overlay"></div>
	<script src="javascript/editorClient.js"></script>
	<script src="javascript/editorCommon.js"></script>
</body>
</html>