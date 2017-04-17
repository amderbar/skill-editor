<?php
    $forms = array ();
    $forms[ 'tbl_name' ]  = HTMLHandler::textbox( 'tbl_name', 'untitled', [ 'required' => true, 'autofocus' => true ] );
    $forms[ 'col_name' ]  = HTMLHandler::textbox( 'def_cols[1][col_name]', '', [ 'required' => true ] );
?>
<?=HTMLHandler::startForm( $URL, [ 'pid' => $proj_id,'tab' => 'new' ], 'POST', [ 'id' => 'tbl-def' ] );?>
    <header>
        <h1>New Table <?=$forms['tbl_name'];?> @ <?=HTMLHandler::escape( $proj_name );?></h1>
    </header>
    <h2>Table Data definition</h2>
    <fieldset name="def-cols">
    <legend>Column definitions</legend>
    <table class="data-table" id="def-tbl">
        <thead>
            <tr>
                <th></th><th>No列</th><th class="col-h"><label>列</labal></th>
                <th id="add-col">列追加</th>
            </tr>
        </thead>
        <tbody>
            <tr><th>列名</th><td>No.</td><td><?=$forms['col_name'];?></td></tr>
            <tr>
                <th>入力形式</th><td>--</td>
                <td>
                    <?=HTMLHandler::selectbox( 'def_cols[1][form_type]', $form_types );?>
                    <div name="step" hidden>
                        <label>刻み幅:
                            <?=HTMLHandler::number( 'def_cols[1][step]', 1, null, [ 'disabled' => true ] );?>
                        </label>
                    </div>
                    <div name="max" hidden>
                        <label>最大値:
                            <?=HTMLHandler::number( 'def_cols[1][max]', null, null, [ 'disabled' => true ] );?>
                        </label>
                    </div>
                    <div name="min" hidden>
                        <label>最小値:
                            <?=HTMLHandler::number( 'def_cols[1][min]', null, null, [ 'disabled' => true ] );?>
                        </label>
                    </div>
                    <div name="multi" hidden>
                        <label>
                            <?=HTMLHandler::radiocheck( 'checkbox', 'def_cols[1][multiple]', [ 1 => null ], [], [ 'disabled' => true ] );?>
                            複数選択をする
                        </label>
                    </div>
                    <div name="ref" hidden>
                        <label>選択肢<br>
                            <?=HTMLHandler::selectbox( 'def_cols[1][ref_dist]', $col_list, [ 0 ], false, [ 'disabled' => true ] );?>
                        </label>
                    </div>
                </td>
            </tr><tr><th>初期値</th><td>自動</td>
                <td>
                    <?=HTMLHandler::textbox( 'def_cols[1][default]', '' );?>
                </td>
            </tr><tr><th>一意</th>
                <td>
                    <?=HTMLHandler::radiocheck( 'checkbox', '', [ 1 => null ], [ 1 ], [ 'disabled' => true ] );?>
                </td><td>
                    <?=HTMLHandler::radiocheck( 'checkbox', 'def_cols[1][uniq]', [ 1 => null ] );?>
                </td>
            </tr><tr><th>非Null</th>
                <td>
                    <?=HTMLHandler::radiocheck( 'checkbox', '', [ 1 => null ], [ 1 ], [ 'disabled' => true] );?>
                </td><td>
                    <?=HTMLHandler::radiocheck( 'checkbox', 'def_cols[1][not_null]', [ 1 => null ] );?>
                </td>
            </tr>
<?php
// TODO:CHECK制約に相当する部分の実装
// echo '<tr><th>その他</th><td></td><td><label>'
// .HTMLHandler::radiocheck('checkbox', 'def_cols[1][add_chk]', 1, null).'条件を追加する</label><div>'
// .HTMLHandler::selectbox('def_cols[1][check]', $chk_list, null, true).'</div></td></tr>'.PHP_EOL;
?>
        </tbody>
    </table>
    </fieldset>
<?php
// TODO:表制約の付加
// echo <<< _HTML_
// <fieldset name="def-cons">
// <legend>Constraints definitions</legend>
// <table class="data-table">
// <thead>
// <tr><th>制約タイプ</th><th>ID列</th><th class="col-h"><label>列</labal></th><th id="add-col">列追加</th></tr>
// </thead>
// <tbody>

// </tbody>
// </table>
// </fieldset>
?>
    <?=HTMLHandler::hidden( 'pid', $proj_id );?>
    <?=HTMLHandler::submit( 'save', '作成' );?>
</form>

<?php $options = array_keys( $tbl_tmpls );?>
<h2>
    Table Desgin Preview : Template
    <?=HTMLHandler::selectbox( 'tbl_templs', $options, [ 0 ], false, [ 'id' => 'tbl_templs'] );?>
</h2>
<div id="preview">
<?php foreach ( $tbl_tmpls as $name => $path ) { ?>
    <div id="<?=HTMLHandler::escape( $name );?>"<?=( $name == $options[ 0 ] ) ? '' : ' hidden';?>>
        <?php include ( $path );?>
    </div>
<?php } ?>
</div>