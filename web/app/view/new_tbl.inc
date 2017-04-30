<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <link rel="stylesheet" type="text/css" href="<?=addFilemtime(APP_ROOT . '/css/common.css')?>">
    <link rel="stylesheet" type="text/css" href="<?=addFilemtime(APP_ROOT . '/css/fonts.css')?>">
    <link rel="stylesheet" type="text/css" href="<?=addFilemtime(APP_ROOT . '/css/editor_area.css')?>">
    <script src="https://code.jquery.com/jquery-3.0.0.min.js"></script>
    <?php /* TODO:出来上がったら圧縮版を読み込むように変更する */ ?>
    <script src="https://unpkg.com/vue"></script>
    <script type="text/javascript">
    <!--//<?php/*エディタ部共通のJavaScript*/?>
        $(function() {
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
    <title><?=APP_NAME?></title>
</head>
<body>
    <?=Html::startForm(APP_ROOT . "/editor/table/register", null, 'POST', ['id' => 'table-form', 'target' => '_parent']) ?>
<?php
    $forms = array ();
    $forms[ 'tbl_name' ]  = Html::textbox( 'tbl_name', 'untitled', [ 'required' => true, 'autofocus' => true ] );
    $forms[ 'col_name' ]  = Html::textbox( 'def_cols[1][col_name]', '', [ 'required' => true ] );
?>
    <?=Html::hidden( 'pid', $proj_id );?>
    <header>
        <h1>New Table <?=$forms['tbl_name'];?> @ <?=Html::escape( $proj_name );?></h1>
    </header>
    <h2>Table Data definition</h2>
    <fieldset name="def-cols">
    <legend>Column definitions</legend>
    <table class="data-table" id="def-tbl">
        <thead>
            <tr>
                <th></th><th>No列</th><th class="col-h"><label>列</labal></th>
                <th id="add-col" rowspan="6">
                    <button type="button" title="列追加" id="add-column">+</button>
                </th>
            </tr>
        </thead>
        <tbody>
            <tr><th>列名</th><td>No.</td><td><?=$forms['col_name'];?></td></tr>
            <tr>
                <th>入力形式</th><td>--</td>
                <td>
                    <?=Html::selectbox( 'def_cols[1][form_type]', $form_types );?>
                    <div name="step" hidden>
                        <label>刻み幅:
                            <?=Html::number( 'def_cols[1][step]', 1, null, [ 'disabled' => true ] );?>
                        </label>
                    </div>
                    <div name="max" hidden>
                        <label>最大値:
                            <?=Html::number( 'def_cols[1][max]', null, null, [ 'disabled' => true ] );?>
                        </label>
                    </div>
                    <div name="min" hidden>
                        <label>最小値:
                            <?=Html::number( 'def_cols[1][min]', null, null, [ 'disabled' => true ] );?>
                        </label>
                    </div>
                    <div name="multi" hidden>
                        <label>
                            <?=Html::radiocheck( 'checkbox', 'def_cols[1][multiple]', [ 1 => null ], [], [ 'disabled' => true ] );?>
                            複数選択をする
                        </label>
                    </div>
                    <div name="ref" hidden>
                        <label>選択肢<br>
                            <?=Html::selectbox( 'def_cols[1][ref_dist]', $col_list, [ 0 ], false, [ 'disabled' => true ] );?>
                        </label>
                    </div>
                </td>
            </tr><tr><th>初期値</th><td>自動</td>
                <td>
                    <?=Html::textbox( 'def_cols[1][default]', '' );?>
                </td>
            </tr><tr><th>一意</th>
                <td>
                    <?=Html::radiocheck( 'checkbox', '', [ 1 => null ], [ 1 ], [ 'disabled' => true ] );?>
                </td><td>
                    <?=Html::radiocheck( 'checkbox', 'def_cols[1][uniq]', [ 1 => null ] );?>
                </td>
            </tr><tr><th>非Null</th>
                <td>
                    <?=Html::radiocheck( 'checkbox', '', [ 1 => null ], [ 1 ], [ 'disabled' => true] );?>
                </td><td>
                    <?=Html::radiocheck( 'checkbox', 'def_cols[1][not_null]', [ 1 => null ] );?>
                </td>
            </tr>
        </tbody>
    </table>
    </fieldset>
    <?=Html::submit( 'save', '作成' );?>

    <?php $options = array_keys( $tbl_tmpls );?>
    <h2>Table Desgin Preview : Template
        <?=Html::selectbox( 'tbl_templs', $options, [ 0 ], false, [ 'id' => 'tbl_templs'] );?>
    </h2>
    <div id="preview">
    <?php foreach ( $tbl_tmpls as $name => $path ) { ?>
        <div id="<?=Html::escape( $name );?>"<?=( $name == $options[ 0 ] ) ? '' : ' hidden';?>>
            <?php include ( $path );?>
        </div>
    <?php } ?>
    </div>
<?=Html::endForm()?>
    <script src="<?=addFilemtime( APP_ROOT . '/js/new_tbl.js' )?>"></script>
</body>
</html>
