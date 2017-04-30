<?php
/**
 *
 * @author amderbar
 *
 */
class DataEditorProcess extends Process
{
    /**
     *
     * @param int $proj_id
     * @return array
     */
    public function getTemplates(int $proj_id) :array
    {
        try {
            $tmpl_list = $this->open(ROOT_DB_ID)->findByKey('templates', [
                    'proj_id' => $proj_id
            ]);
            return array_combine(
                array_column($tmpl_list, 'id'),
                array_column($tmpl_list, 'tmpl_name')
            );

        } catch ( PDOException | DBManageExeption $e ) {
            $this->handleException( $e );
        }
    }

    /**
     *
     * @param int $proj_id
     * @param int $tbl_id
     * @return array
     */
    public function listColumns(int $proj_id, int $tbl_id) :array
    {
        try {
            return $this->open($proj_id)->findByKeyWithJoin(
                [ SYSTEM_COL, NUM_SETTINGS => [ [NUM_SETTINGS => 'col_id', SYSTEM_COL => 'id'] ] ],
                [ SYSTEM_COL => ['tbl_id' => $tbl_id ] ],
                null, [ SYSTEM_COL.'.id' ]
                );
        } catch ( PDOException | DBManageExeption $e ) {
            $this->handleException( $e );
        }
    }


    /**
     * @param int $proj_id
     * @param int $col_id
     * @param bool $is_actual
     * @return array
     */
    public function getTblColNameFromColid(int $proj_id, int $col_id, ?bool $is_actual = false) :?array
    {
        $result = $this->open($proj_id)->findByKeyWithJoin(
            [ SYSTEM_TBL, SYSTEM_COL => [ [SYSTEM_TBL => 'id', SYSTEM_COL => 'tbl_id'] ] ],
            [ SYSTEM_COL => ['id' => $col_id ] ],
            [ SYSTEM_TBL => ['actual_name as tbl_name'], SYSTEM_COL => ['actual_name as col_name'] ]
            );
        return array_shift( $result );
    }

    /**
     *
     *
     * @param int $proj_id
     * @param int $tbl_id
     * @return array
     */
    public function listData(int $proj_id, int $tbl_id): array
    {
        try {
            $form_type = Arr::combine(
                $this->open(ROOT_DB_ID)->fetchAll('form_master'),
                'id',
                'type'
            );

            $col_names = array();
            $col_forms = array();
            foreach ( $this->listColumns( $proj_id, $tbl_id ) as $col ) {
                $actual_name = $col[ 'actual_name' ];
                $col_names[ $actual_name ] = $col[ 'col_name'];
                $col_forms[ $actual_name ] = array(
                        'type'     => $form_type[ $col[ 'form_type'] ],
                        'default'  => $col[ 'default_val'],
                        'multiple' => boolval( $col[ 'multiple'] )
                );
                if ( isset( $col[ 'step'] ) || isset( $col[ 'max'] ) || isset( $col[ 'min'] ) ) {
                    $col_forms[ $actual_name ]['step'] = $col[ 'step'] ?? 1;
                    $col_forms[ $actual_name ]['max']  = $col[ 'max'] ?? null;
                    $col_forms[ $actual_name ]['min']  = $col[ 'min'] ?? null;
                }
                if ( isset( $col[ 'ref_col'] ) ) {
                    $ref_tbl_col = $this->getTblColNameFromColid( $proj_id, $col[ 'ref_col'], true );
                    $col_forms[ $actual_name ]['ref'] = array();
                    $ref_data = $this->con[$proj_id]->fetchAll( $ref_tbl_col['tbl_name'], ['id', $ref_tbl_col['col_name'] ], ['id'] );
                    foreach ( $ref_data as $row) {
                        $col_forms[ $actual_name ]['ref'][ $row['id'] ] = $row[ $ref_tbl_col['col_name'] ];
                    }
                    if ( empty( $col_forms[ $actual_name ]['ref'] ) ) {
                        $col_forms[ $actual_name ]['ref'] = [];
                    }
                }
            }
            $list_data = $this->open($proj_id)->fetchAll( $this->tblName( $proj_id, $tbl_id, true ), null, ['id'] );

            return ['col_name' => $col_names, 'form' => $col_forms, 'data' => $list_data];

        } catch ( PDOException | DBManageExeption $e ) {
            $this->handleException( $e );
        }
    }
}
