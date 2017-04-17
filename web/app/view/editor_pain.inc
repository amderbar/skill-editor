<?php
/**
* session check.
*/
require_once( $_SERVER['DOCUMENT_ROOT'].'/skill_editor/common.inc' );

?>
<!DOCTYPE html>
<html>
<head>
	<meta charset="UTF-8">
	<script src="https://code.jquery.com/jquery-3.0.0.min.js"></script>
	<!-- TODO:出来上がったら圧縮版を読み込むように変更する -->
	<script src="https://unpkg.com/vue"></script>
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
             * フォームの変更を検知して画面遷移時の確認ダイアログを出すようにする
             */
            $('#table-form').change(function(){
                window.onbeforeunload = function(e) {
                    return 'Are you sure?';
                };
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
    <section id="tab-page">
    <form action="<?=$URL?>" method="post" id="table-form">
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
    $js_file = ( isset( $new_tab ) ) ? 'js/new_tbl.js' : 'js/editor_area.js';
}
?>
    </form>
    </section>
    <script src="<?=addFilemtime( $js_file )?>"></script>
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
