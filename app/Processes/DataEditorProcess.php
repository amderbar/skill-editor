<?php

namespace Amderbar\App\Processes;

use PDO;
use Amderbar\Lib\Utils\HtmlUtil as Html;
use Amderbar\Lib\Utils\ArrUtil as Arr;
use \Amderbar\Lib\Utils\DebugUtil as Log;

/**
 *
 * @author amderbar
 *
 */
class DataEditorProcess extends EditorProcess
{
    /**
     *
     *
     * @param int $proj_id
     * @param int $tbl_id
     * @return array
     */
    public function listData(int $tbl_id): array
    {
        try {
            $db_con = $this->open($this->proj_id);
            $col_forms = [];
            $col_list = $this->listColumns($tbl_id);
            foreach ($col_list[$tbl_id]['columns'] as $col) {
                $actual_name = $col['col_actual'];
                $col_forms[$actual_name] = $col;
                if (isset($col['ref_col'])) {
                    $ref_tbl_col = $this->getColumnInfo($col['ref_col']);
                    $col_forms[$actual_name]['ref_data'] = Arr::combine(
                        $db_con
                            ->query("
                                select id, {$ref_tbl_col->col_actual}
                                from {$ref_tbl_col->tbl_actual}
                            ")
                            ->fetchAll(PDO::FETCH_ASSOC),
                        'id',
                        $ref_tbl_col->col_actual
                    );
                }
            }
            $list_data = $db_con
                ->query("
                    select *
                    from {$col_list[$tbl_id]['tbl_actual']}
                    order by id
                ")
                ->fetchAll();

            return ['column_config' => $col_forms, 'data' => $list_data];

        } catch ( PDOException | DBManageExeption $e ) {
            $this->handleException( $e );
        }
    }

    /**
     *
     * @param int $proj_id
     * @param int $tbl_id
     * @param array $data
     */
    public function modifyData(int $proj_id, int $tbl_id, array $data) :void
    {
        $tbl_name = $this->tblName( $proj_id, $tbl_id, true );
        $db_con = $this->open($proj_id);
        foreach ($data['update'] as $id => $parms) {
            var_export_log($parms);
            $db_con->update($tbl_name, $parms, ['id' => $id]);
        }
        foreach ($data['insert'] as $parms) {
            $db_con->insert($tbl_name, $parms);
        }
    }
}
