<?php

namespace Amderbar\App\Processes;

use stdClass;
use PDO;
use PDOException;
use Amderbar\Lib\AmbException;
use Amderbar\Lib\Utils\ArrUtil as Arr;

/**
 * Undocumented class
 */
class EditorProcess extends Process
{
    /**
     * Undocumented variable
     *
     * @var int
     */
    protected $proj_id;
    
    /**
     * Undocumented function
     *
     * @param int $proj_id
     * @return void
     */
    public function init(int $proj_id)
    {
        $this->proj_id = $proj_id;
    }

    /**
     * Undocumented function
     *
     * @param callable $where
     * @return string
     */
    private function getColumnInfoSql(callable $where = null) :string
    {
        return '
            select
                tt.tbl_id
                , tt.tbl_name
                , tt.actual_name as tbl_actual
                , tt.tmpl_id
                , tc.col_id
                , tc.col_name
                , tc.actual_name as col_actual
                , tc.default_val
                , tc.form_type
                , tc.ref_col
                , tc.is_primary
                , tc.multiple
                , tn.step
                , tn.min
                , tn.max
            from ' . SYSTEM_TBL . ' as tt
                join ' . SYSTEM_COL . ' as tc on tc.tbl_id = tt.tbl_id
                left join ' . NUM_SETTINGS . ' as tn on tn.col_id = tc.col_id
            ' . (isset($where) ? $where() : '') . '
            order by tt.display_order, tt.update_at, tc.display_order, tc.update_at
        ';
    }

    /**
     * Undocumented function
     *
     * @param int $proj_id
     * @param int $col_id
     * @return stdClass
     */
    public function getColumnInfo(int $col_id) :stdClass
    {
        return $this->open($this->proj_id)
            ->query($this->getColumnInfoSql(function () {
                return 'where tc.col_id = ?';
            }), [$col_id])
            ->fetch();
    }

    /**
     *
     * @param int $tbl_id
     * @return array
     */
    public function listColumns(int $tbl_id = null) :array
    {
        try {
            return array_reduce(
                $this->open($this->proj_id)
                    ->query($this->getColumnInfoSql(function () use ($tbl_id) {
                        return isset($tbl_id) ? 'where tt.tbl_id = ?' : '';
                    }), isset($tbl_id) ? [$tbl_id] : [])
                    ->fetchAll(PDO::FETCH_ASSOC) ?: []
                    ,function ($carry, $column) {
                        if (!isset($carry[$column['tbl_id']])) {
                            $carry[$column['tbl_id']] = [
                                'tbl_name' => $column['tbl_name'],
                                'tbl_actual' => $column['tbl_actual'],
                                'columns' => []
                            ];
                        }
                        $carry[$column['tbl_id']]['columns'][$column['col_id']]
                            = Arr::except($column, 'tbl_id', 'tbl_name', 'tbl_actual', 'col_id');
                        return $carry;
                    }
                    ,[]
                );
        } catch (PDOException | AmbException $e) {
            $this->handleException($e);
        }
    }

    /**
     *
     * @return array
     */
    public function getAllTemplates() :array
    {
        try {
            $tmpl_list = $this->open($this->proj_id)
                ->query('
                    select tmpl_id, tmpl_name
                    from templates
                    order by display_order, update_at
                ')
                ->fetchAll(PDO::FETCH_ASSOC);
                return Arr::combine($tmpl_list, 'tmpl_id', 'tmpl_name');
        } catch (PDOException | AmbException $e) {
            $this->handleException($e);
        }
    }
}
