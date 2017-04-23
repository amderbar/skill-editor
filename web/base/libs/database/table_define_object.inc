<?php

/**
 *
 */
abstract class DataTransfarObject {
    /**  */
    private $parms = array();

    /**
     *
     */
    abstract function getTableName();

    /**
     *
     */
    public function getParms() {
        return $this->parms;
    }

    /**
     *
     */
    public function setParm($parm, $value) {
        $this->parms[$parm] = $value;
    }
}

/**
 *
 */
class TableDefineObject {
    /**  */
    private $tbl_name = "";
    /**  */
    private $colmuns = array();
    /** */
    private $constraints = array();

    /**
     *
     */
    function __construct($tbl_name, $colmuns = null, $auto_primery = true)
    {
        $this->tbl_name = $tbl_name;
        if ($auto_primery) {
            $this->appendColumn('id', 'integer', ['primery' => true]);
        }
    }

    /**
     *
     */
    public function tableName() :string
    {
        return $this->tbl_name;
    }

    public function appendColumn(string $col_name, string $type, array $constraints = array()) :TableDefineObject
    {
        $col_hash = array(
                'name' => $col_name,
                'type' => $type,
                'default' => $constraints['default'] ?? null,
                'uniq' => boolval( $constraints['uniq'] ?? null ),
                'not_null'=> boolval( $constraints['not_null'] ?? null ),
                'primery' => boolval( $constraints['primery'] ?? null ),
                'foreign' => $constraints['foreign'] ?? null // reference and on delete
        );
        $this->colmuns[] = $col_hash;
        return $this;
    }

    /**
     *
     */
    public function fetchColumn()
    {
        return array_shift($this->colmuns);
    }

    /**
     *
     */
    public function appendConstraint($con_name, $targets, $references = null):TableDefineObject
    {
        $con_hash = array(
                'type' => $con_name,
                'targets' => $targets, // constraint target columns (itselfs)
                'references' => $references // foreign key constraint references columns
        );
        $this->constraints[] = $con_hash;
        return $this;
    }

    /**
     *
     */
    public function fetchConstraint()
    {
        return array_shift($this->constraints);
    }

    /**
     *
     */
    public function rewind() :TableDefineObject
    {
        reset($this->colmuns);
        reset($this->constraints);
        return $this;
    }
}

