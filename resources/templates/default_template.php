<?php
$primary_colmun = array();
foreach ($tbl_data as $key => $row) {
    $primary_colmun[] = (isset($row['id'])) ? $row['id'] : null;
    if (isset($row['id'])) {
        unset($tbl_data[$key]['id']);
    }
}
if (array_depth($tbl_data) == 2) {
    echo '<table class="data-table">'.PHP_EOL;
    $column_names = array();
    foreach ($tbl_data as $key => $row) {
        $column_names += array_keys($row);
    }
    echo '<tr>'.PHP_EOL;
    echo '<th></th>';
    foreach ($column_names as $key) {
        echo '<th>'.HTMLHandler::specialchars($key).'</th>';
    }
    echo PHP_EOL.'</tr>';
    foreach ($tbl_data as $key => $row) {
        echo '<tr>'.PHP_EOL;
        // if ($primary_colmun[$key]) {
        //     echo '<input type="hidden" name="id[]" value="'.HTMLHandler::specialchars($primary_colmun[$key]).'">';
        // }
        echo '<th>'.HTMLHandler::specialchars($key).'</th>';
        foreach ($column_names as $column_name) {
            echo '<td>';
            if (isset($row[$column_name])) {
                echo HTMLHandler::specialchars($row[$column_name]);
            }
            echo '</td>';
        }
        echo PHP_EOL.'</tr>';
    }
    echo PHP_EOL.'</table>'.PHP_EOL;
} else {
    pre_dump($tbl_data);
}
?>