<?php
if (array_depth($tbl_data['data']) == 2) {
    $tbl_keys = array_keys( $tbl_data['col_name'] );
    echo '<table class="data-table">'.PHP_EOL;
    echo '<tr>'.PHP_EOL;
    foreach ( $tbl_keys as $key ) {
        echo '<th>'.HTMLHandler::escape( $tbl_data['col_name'][$key] ).'</th>';
    }
    echo PHP_EOL.'</tr>';
    foreach ( $tbl_data['data'] as $row ) {
        echo '<tr>'.PHP_EOL;
        foreach ( $tbl_keys as $key ) {
            if ( $key == 'id' ) {
                $id_attr = ( is_null( $row[$key] ) ) ? ' id="new-rec"' : '';
                $cont = '<span'.$id_attr.'>'.HTMLHandler::hidden( 'id[]', $row[$key] ).'</span>';
                echo '<th class="id-col">'.$cont.'</th>';
            } else {
                echo '<td>'.HTMLHandler::escape( $row[$key] ).'</td>';
            }
        }
        echo PHP_EOL.'</tr>';
    }
    echo PHP_EOL.'</table>'.PHP_EOL;
} else {
    pre_dump($tbl_data);
}
?>