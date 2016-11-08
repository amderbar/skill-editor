<?php
/**
* session check.
*/
require_once($_SERVER['DOCUMENT_ROOT'].'/skill_editor/common.php');
//
require_once(full_path('models/html_handler.php'));
?>
<!DOCTYPE html>
<html>
<head>
	<meta charset="UTF-8">
	<script src="https://code.jquery.com/jquery-3.0.0.min.js"></script>
	<link rel="stylesheet" type="text/css" href="<?=addFilemtime('css/common.css')?>">
	<link rel="stylesheet" type="text/css" href="<?=addFilemtime('css/fonts.css')?>">
	<link rel="stylesheet" type="text/css" href="<?=addFilemtime('css/editor_area.css')?>">
	<title>Editor on Browser</title>
</head>
<body>
    <header class="tab-bar">
        <a href="#" class="icon-list2" title="サイドメニューを閉じる" id="toggle_menu"></a>
        <ul class="editor-tabs">
<?php
    foreach ($tbl_list as $tbl_num => $tbl_name) {
        $tab_class = ($tbl_num == $opened_tab) ? ' class="editting"' : '';
        echo '<li'.$tab_class.'>
        <a href="'.$URL.'?id='.HTMLHandler::specialchars($proj_id).'&tab='.HTMLHandler::specialchars($tbl_num).'">'
        .HTMLHandler::specialchars($tbl_name).'</a></li>'.PHP_EOL;
    }
?>
    <!--<a href="#" class="icon-new-tab btn"></a>-->
        </ul>
    </header>
    <section class="tab-page">
<?php
    //echo HTMLHandler::makeDatalist($data_list);
if (isset($tbl_tmpl)) {
    include($tbl_tmpl);
} elseif (isset($proj_name)) {
    echo '<form action="editor_area.php" method="POST">'.PHP_EOL;
    echo '<header>'.PHP_EOL;
    echo '<h1>New Table '.HTMLHandler::input_text('tbl_name', 'untitled').' @ '.$proj_name.'</h1>'.PHP_EOL;
    echo '</header>'.PHP_EOL;
    echo '<h2>Table Data definition</h2>'.PHP_EOL;
    echo '<table class="data-table" id="def-tbl">'.PHP_EOL;
    echo '<thead>'.PHP_EOL;
    echo '<tr><th></th><th>列1</th><th id="add-col">列追加</th></tr>'.PHP_EOL;
    echo '</thead><tbody>'.PHP_EOL;
    echo '<tr><th>列名</th><td>'.HTMLHandler::input_text('colname[]', '', 'size="11"').'</td></tr>'.PHP_EOL;
    echo '<tr><th>データ型</th><td>'.HTMLHandler::input_select('type[]', ['TEXT'], DBEditor::getDateTypeList()).'</td></tr>'.PHP_EOL;
    echo '<tr><th>初期値</th><td>'.HTMLHandler::input_text('default[]', '', 'size="11"').'</td></tr>'.PHP_EOL;
    echo '<tr><th>一意</th><td>'.HTMLHandler::input_radiocheck('checkbox', 'uniq[]', 1).'</td></tr>'.PHP_EOL;
    echo '<tr><th>非Null</th><td>'.HTMLHandler::input_radiocheck('checkbox', 'not_null[]', 1).'</td></tr>'.PHP_EOL;
    echo '<tr><th>外部参照</th><td>'.HTMLHandler::input_radiocheck('checkbox', 'foreign[]', 1).'</td></tr>'.PHP_EOL;
    echo '</tbody>'.PHP_EOL;
    echo '</table>'.PHP_EOL;
    echo '<h2>Table Display Design</h2>'.PHP_EOL;
    echo HTMLHandler::hidden('id', $proj_id).PHP_EOL;
    echo HTMLHandler::input_submit('save', '作成').PHP_EOL;
    echo '</form>'.PHP_EOL;
}
?>
    </section>
	<script src="<?=addFilemtime('js/editorClient.js')?>"></script>
</body>
</html>