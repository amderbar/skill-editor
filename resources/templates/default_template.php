<?php
$primary_colmun = array();
foreach ($tbl_data as $key => $row) {
    if (isset($row['rid'])) {
        $primary_colmun[$key] = $row['rid'];
        unset($tbl_data[$key]['rid']);
    } else {
        $primary_colmun[$key] = null;
    }
}
if (array_depth($tbl_data) == 2) {
    $column_names = array();
    foreach ($tbl_data as $key => $row) {
        $column_names += array_keys($row);
    }
    echo '<table class="data-table">'.PHP_EOL;
    echo '<tr>'.PHP_EOL;
    echo '<th>ID</th>';
    foreach ($column_names as $key) {
        echo '<th>'.HTMLHandler::escape($key).'</th>';
    }
    echo PHP_EOL.'</tr>';
    foreach ($tbl_data as $key => $row) {
        echo '<tr>'.PHP_EOL;
        $hidden = (isset($primary_colmun[$key])) ? HTMLHandler::hidden('rid[]', $primary_colmun[$key]) : '';
        echo '<th>'.$hidden.HTMLHandler::escape($key + 1).'</th>';
        foreach ($column_names as $column_name) {
            echo '<td>';
            if (isset($row[$column_name])) {
                echo HTMLHandler::escape($row[$column_name]);
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