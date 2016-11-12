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
    <?php/*エディタ部共通のJavaScript*/?>
    <script type="text/javascript">
    <!--
        $(function() {
            /**
             * サイドメニューの表示、非表示切り替え関数
             */
            $('#toggle_menu').click(function () {
                var side_menu = $('#side_menu', parent.document);
                if(side_menu.is(':visible')){
                    $(this).attr('title', 'サイドメニューを開く');
                } else {
                    $(this).attr('title', 'サイドメニューを閉じる');
                }
                side_menu.toggle('fast');
            });
            /**
             * ページ読み込み時とウィンドウリサイズ時にタブバーの高さを取得してタブページのtop位置を変更する関数
             * リサイズ操作が止まった時にだけ実行するように工夫している。
             */
            var timer = false;
            function resetTop() {
                var top_height = $('#tab-bar').height();
                $('#tab-page').css('top', top_height + 10);
            }
            resetTop();
            $(window).resize(function(){
                if (timer !== false) {
                    clearTimeout(timer);
                }
                timer = setTimeout(function() {
                    resetTop();
                }, 50);
            });
        });
    //-->
    </script>
<?php
    if (isset($new_tab)) {
        echo '<script src="'.addFilemtime('js/new_tbl.js').'"></script>'.PHP_EOL;
    } else {
        echo '<script src="'.addFilemtime('js/editor_area.js').'"></script>'.PHP_EOL;
    }
?>
	<link rel="stylesheet" type="text/css" href="<?=addFilemtime('css/common.css')?>">
	<link rel="stylesheet" type="text/css" href="<?=addFilemtime('css/fonts.css')?>">
	<link rel="stylesheet" type="text/css" href="<?=addFilemtime('css/editor_area.css')?>">
	<title>Editor on Browser</title>
</head>
<body>
    <header id="tab-bar">
        <a href="#" class="icon-list2" title="サイドメニューを閉じる" id="toggle_menu"></a>
        <ul class="editor-tabs">
<?php
    if (isset($proj_id)) {
        foreach ($tbl_list as $tbl_num => $tbl_name) {
            $tab_class = ($tbl_num === $opened_tab) ? ' class="editting"' : '';
            echo '<li'.$tab_class.'>'.HTMLHandler::link($tbl_name, $URL, ['id' => $proj_id, 'tab' => $tbl_num], $additional = null).'</li>'.PHP_EOL;
        }
        if (isset($new_tab)) {
            echo '<li class="editting"><span id="new-tab-name">Untitled</span></li>'.PHP_EOL;
        } else {
            echo HTMLHandler::link('', $URL, ['id' => $proj_id, 'tab' => 'new'], ['class' => ['icon-new-tab', 'btn'], 'title' => 'テーブルの追加']);
        }
    }
?>
        </ul>
    </header>
    <section id="tab-page">
<?php
if (isset($proj_id)) {
    if (isset($new_tab)) {
        $forms = array();
        $forms['tbl_name'] = HTMLHandler::textbox('tbl_name', 'untitled');
        $forms['proj_name'] = HTMLHandler::escape($proj_name);
        $forms['colname'] = HTMLHandler::textbox('colname[]', '', ['required' => 'required']);
        echo <<< _HTML_
        <form action="editor_area.php" method="POST" id="tbl-def">
            <header>
                <h1>New Table {$forms['tbl_name']} @ {$forms['proj_name']}</h1>
            </header>
            <h2>Table Data definition</h2>
            <table class="data-table" id="def-tbl">
                <thead>
                    <tr><th></th><th class="col-h"><label>列</labal></th><th id="add-col">列追加</th></tr>
                </thead>
                <tbody>
                    <tr><th>列名</th><td>{$forms['colname']}</td></tr>
_HTML_;
        echo '<tr><th>入力形式</th><td>'.PHP_EOL;
        echo HTMLHandler::selectbox('form_type[]', $form_types).PHP_EOL;
        echo '<div name="step" hidden><label>刻み幅:'.HTMLHandler::number('step[]', 1).'</label></div>'.PHP_EOL;
        echo '<div name="max" hidden><label>最大値:'.HTMLHandler::number('max[]').'</label></div>'.PHP_EOL;
        echo '<div name="min" hidden><label>最小値:'.HTMLHandler::number('min[]').'</label></div>'.PHP_EOL;
        echo '<div name="multi" hidden><label>'.HTMLHandler::radiocheck('checkbox', 'multi[]', 1).'複数選択をする</label></div>'.PHP_EOL;
        // ↓プロジェクト内の全列リストが必要。表ごとにoptgroupにする
        echo '<div name="ref" hidden><label>選択肢<br>'.HTMLHandler::selectbox('ref_dist[]', $tbl_list).'</label></div>'.PHP_EOL;
        echo '</td></tr>'.PHP_EOL;
        // echo '<tr><th>データ型</th><td>'.HTMLHandler::selectbox('type[]', $data_types, ['TEXT']).'</td></tr>'.PHP_EOL;
        echo '<tr><th>初期値</th><td>'.HTMLHandler::textbox('default[]', '').'</td></tr>'.PHP_EOL;
        echo '<tr><th>一意</th><td>'.HTMLHandler::radiocheck('checkbox', 'uniq[]', 1).'</td></tr>'.PHP_EOL;
        echo '<tr><th>非Null</th><td>'.HTMLHandler::radiocheck('checkbox', 'not_null[]', 1).'</td></tr>'.PHP_EOL;
        // echo '<tr><th>外部参照</th><td>'.HTMLHandler::radiocheck('checkbox', 'foreign[]', 1).'</td></tr>'.PHP_EOL;
        echo <<< _HTML_
                </tbody>
            </table>
_HTML_;
        echo HTMLHandler::hidden('id', $proj_id).PHP_EOL;
        echo HTMLHandler::submit('save', '作成').PHP_EOL;
        echo '</form>'.PHP_EOL;

        echo '<h2>Table Desgin Preview : Template ';
        $options = array_keys($tbl_tmpls);
        echo HTMLHandler::selectbox('tbl_templs', $options, [0], false, ['id' => 'tbl_templs']);
        echo '</h2>'.PHP_EOL;
        echo '<div id="preview">'.PHP_EOL;
        foreach ($tbl_tmpls as $name => $path) {
            $is_hidden = ($name == $options[0]) ? '' : ' hidden';
            echo '<div id="'.HTMLHandler::escape($name).'"'.$is_hidden.'>'.PHP_EOL;
            include($path);
            echo '</div>'.PHP_EOL;
        }
        echo '</div>'.PHP_EOL;
    } else {
        include($tbl_tmpl);
    }
}
?>
    </section>
</body>
</html>