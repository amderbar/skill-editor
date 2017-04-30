<?php $tbl_keys = array_keys( $tbl_data['col_name'] ) ?>
<table class="data-table">
    <thead><tr>
    <?php foreach ( $tbl_keys as $key ) { ?>
        <th><?=Html::escape( $tbl_data['col_name'][$key] )?></th>
    <?php } ?>
    </tr></thead>
    <tbody><?php foreach ( $tbl_data['data'] as $row ) { ?>
        <?php $id_attr = ( is_null( $row['id'] ) ) ? ' id="new-rec"' : '';?>
        <tr<?=$id_attr?>><?php foreach ( $tbl_keys as $key ) { ?>
            <?php if ( $key == 'id' ) { ?>
                <th class="id-col"><?=Html::hidden( 'id[]', $row[$key] )?></th>
            <?php } else { ?>
                <td class="editable" data-key="<?=Html::escape( $key )?>">
                <?=Html::escape( $row[$key] )?>
                </td>
            <?php } ?>
        <?php } ?></tr>
    <?php } ?></tbody>
</table>
