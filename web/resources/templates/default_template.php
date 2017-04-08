<?php
if ( array_depth( $tbl_data['data'] ) == 2 ) {
    $tbl_keys = array_keys( $tbl_data['col_name'] );
    echo '<table class="data-table">'.PHP_EOL;
    echo '<tr>'.PHP_EOL;
    foreach ( $tbl_keys as $key ) {
        echo '<th>'.HTMLHandler::escape( $tbl_data['col_name'][$key] ).'</th>';
    }
    echo PHP_EOL.'</tr>';
    foreach ( $tbl_data['data'] as $row ) {
        $id_attr = ( is_null( $row['id'] ) ) ? ' id="new-rec"' : '';
        echo '<tr'.$id_attr.'>'.PHP_EOL;
        foreach ( $tbl_keys as $key ) {
            if ( $key == 'id' ) {
                echo '<th class="id-col">'.HTMLHandler::hidden( 'id[]', $row[$key] ).'</th>';
            } else {
                echo '<td class="editable" data-key="'.HTMLHandler::escape( $key ).'">'
                    .HTMLHandler::escape( $row[$key] ).'</td>';
            }
        }
        echo PHP_EOL.'</tr>';
    }
    echo PHP_EOL.'</table>'.PHP_EOL;
} else {
    pre_dump( $tbl_data );
}
?>