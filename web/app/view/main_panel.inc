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
    <script src="<?=addFilemtime('js/side_menu.js')?>"></script>
    <link rel="stylesheet" type="text/css" href="<?=addFilemtime('css/common.css')?>">
    <link rel="stylesheet" type="text/css" href="<?=addFilemtime('css/fonts.css')?>">
    <link rel="stylesheet" type="text/css" href="<?=addFilemtime('css/main_panel.css')?>">
    <title><?=APP_NAME?></title>
</head>
<body>
    <nav id="side_menu">
        <header class="with-btns">
            <h2>Projects</h2>
            <ul class="btns">
                <li title="新規作成"><a href="#" id="new-proj" class="icon-database btn"></a></li>
            </ul>
        </header>
        <ul class="side-menu" id="proj-list">
            <?php foreach ($proj_list as $proj_id => $proj_name) {
                echo '<li><div class="with-btns">'.PHP_EOL;
                echo '<h2>'.HTMLHandler::link($proj_name, $URL, ['pid' => $proj_id], ['onclick' => 'changeEditor('.$proj_id.')']).'</h2>'.PHP_EOL;
                echo '<form action="'.$URL.'" method="POST">'.PHP_EOL;
                echo HTMLHandler::hidden('token', session_id());
                echo HTMLHandler::hidden('pid', $proj_id);
                echo '<ul class="btns">
                        <li class="icon-folder-plus btn submit" title="テーブルの追加">'.HTMLHandler::hidden('fMode', 'add-tbl').'</li>
                        <li class="icon-bin btn submit" title="プロジェクトの削除">'.HTMLHandler::hidden('fMode', 'del-prj').'</li>
                      </ul>';
                echo '</form>'.PHP_EOL;
                echo '</div>';
                if (isset($opend_id) && $proj_id == $opend_id) {
                    echo '<ul class="side-children">'.PHP_EOL;
                    foreach ($current_proj_tbl_list as $tbl_num => $tbl_name) {
                        echo '<li class="with-btns">'.PHP_EOL;
                        echo HTMLHandler::link($tbl_name, '#', [], ['onclick' => 'changeEditor('.$proj_id.','.$tbl_num.')']).PHP_EOL;
                        echo '<ul class="btns">
                        <li class="icon-bin btn" title="テーブルの削除"></li>
                        </ul>
                        </li>'.PHP_EOL;
                    }
                    echo '</ul>';
                }
                echo '</li>'.PHP_EOL;
            } ?>
            <!-- <li><input type="file" id="file_select" onchange="handleFileSelect(this)"></li>-->
        </ul>
    </nav>
    <main>
        <header id="tab-bar">
            <button type="button" title="サイドメニューを閉じる" id="toggle_menu"><span class="icon-list2"></span></button>
            <ul class="editor-tabs">
<?php
    if ( isset( $proj_id ) ) {
        foreach ( $tbl_list as $tbl_id => $tbl_name ) {
            $tab_class = ( $tbl_id === $opened_tab ) ? ' class="editting"' : '';
            echo '<li'.$tab_class.'>'.HTMLHandler::link($tbl_name, $URL, ['pid' => $proj_id, 'tab' => $tbl_id]).'</li>'.PHP_EOL;
        }
        if ( isset( $new_tab ) ) {
            echo '<li class="editting"><span id="new-tab-name">Untitled</span></li>'.PHP_EOL;
        } else {
            echo HTMLHandler::link( '', $URL,
                ['pid' => $proj_id, 'tab' => 'new'],
                ['class' => ['icon-new-tab', 'btn'], 'title' => 'テーブルの追加'] );
        }
    }
?>
            </ul>
        </header>
        <iframe name="editor_area" src="editor_area.php<?=(isset($proj_id))?'?pid='.$proj_id:''?>" scrolling="no" frameborder="no"></iframe>
    </main>
</body>
</html>