<?php
/**
* session check.
*/
require_once( $_SERVER['DOCUMENT_ROOT'].'/skill_editor/common.inc' );

// TODO: 保存せずにページ移動しようとしたときのダイアログ表示
$js_file = ( isset( $new_tab ) ) ? 'js/new_tbl.js' : 'js/editor_area.js';
?>
<!DOCTYPE html>
<html>
<head>
	<meta charset="UTF-8">
	<script src="https://code.jquery.com/jquery-3.0.0.min.js"></script>
    <script src="<?=addFilemtime( $js_file )?>"></script>
    <script type="text/javascript">
    <!--//<?php/*エディタ部共通のJavaScript*/?>
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
	<link rel="stylesheet" type="text/css" href="<?=addFilemtime('css/common.css')?>">
	<link rel="stylesheet" type="text/css" href="<?=addFilemtime('css/fonts.css')?>">
	<link rel="stylesheet" type="text/css" href="<?=addFilemtime('css/editor_area.css')?>">
	<title><?=APP_NAME?></title>
</head>
<body>
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
    <section id="tab-page">
<?php
if ( isset( $proj_id ) ) {
    if ( isset( $new_tab ) ) {
        include( full_path('view/new_tbl.inc') );

    } else {
        // TODO: 各列の入力タイプ設定に従って、テンプレートタグを出力
        // テンプレート内にはセレクトボックスなどのオプションも含める
        foreach ($tbl_data['form'] as $col_name => $def_hash) {
            if ( $col_name == 'id' ) {
                continue;
            }
            prtFormTemplate( $col_name, $def_hash );
        }

        include( $tbl_tmpl );
    }
}
?>
    </section>
</body>
</html>

<?php
// function defineitions

/**
 * @param string $col_name
 * @param array $def_hash
 * @return string
 */
function prtFormTemplate(string $col_name, array $def_hash ) :void {
    $tmpl_str = '<template id="'.$col_name.'">';
    $add_attr = array();
    switch ( $def_hash['type'] ) {
        case 'textarea':
            $tmpl_str .= HTMLHandler::textarea( 'data['.$col_name.']', $def_hash['default'], $add_attr );
            break;

        case 'select':
            $tmpl_str .= HTMLHandler::selectbox( 'data['.$col_name.']', $def_hash['ref'], $def_hash['default'], $def_hash['multiple'], $add_attr );
            break;

        case 'multicheck':
            $def_hash['type'] = 'checkbox';
        case 'radio':
            $vals = $def_hash['ref'];
            $chked_vals = $def_hash['ref'];
        case 'checkbox':
            if ( !isset( $vals ) ) {
                $vals = [ 1 => '' ];
            }
            if ( !isset( $chked_vals ) ) {
                $chked_vals = [];
            }
            $tmpl_str .= HTMLHandler::radiocheck( $def_hash['type'], 'data['.$col_name.']', $vals, $chked_vals, $add_attr );
            break;

        default:
            if ( $def_hash['type'] == 'listext' || $def_hash['type'] == 'numlist' ) {
                $id_attr = uniqid('dl_');
                echo HTMLHandler::datalist( $id_attr, $def_hash['ref'] );
                $add_attr['list'] = $id_attr;
                if ( $def_hash['type'] == 'listext' ) {
                    $def_hash['type'] = 'text';
                } else if ( $def_hash['type'] == 'numlist' ) {
                    $def_hash['type'] = 'mumber';
                }
            }
            if ( isset( $def_hash['step'] ) ) {
                $add_attr['step'] = $def_hash['step'];
            }
            if ( isset( $def_hash['max'] ) ) {
                $add_attr['max'] = $def_hash['max'];
            }
            if ( isset( $def_hash['step'] ) ) {
                $add_attr['min'] = $def_hash['min'];
            }
            $tmpl_str .= HTMLHandler::input( $def_hash['type'], 'data['.$col_name.']', $def_hash['default'], $add_attr );
            break;
    }

    $tmpl_str .= '</template>';

    echo $tmpl_str;
}
