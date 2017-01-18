<?php

/**
 *
 */
abstract class DTO {
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
	function __construct($tbl_name, $colmuns = null, $auto_primery = true) {
		$this->tbl_name = $tbl_name;
		if ($auto_primery) {
			$this->appendColumn('id', 'integer', ['primery' => true]);
		}
	}

	/**
	 *
	 */
	public function tableName() :string {
		return $this->tbl_name;
	}

	public function appendColumn(string $col_name, string $type, array $constraints = array()) :void {
		$col_hash = array(
				'name' => $col_name,
				'type' => $type,
				'default' => $constraints['default'] ?? null,
				'uniq' => (isset($constraints['uniq'])) ? (bool)$constraints['uniq'] : false,
				'not_null'=> (isset($constraints['not_null'])) ? (bool)$constraints['not_null'] : false,
				'primery' => (isset($constraints['primery'])) ? (bool)$constraints['primery'] : false,
				'increment' => (isset($constraints['increment'])) ? (bool)$constraints['increment'] : false,
				'foreign' => $constraints['foreign'] ?? null // reference and on delete
		);
		$this->colmuns[] = $col_hash;
	}

	/**
	 *
	 */
	public function fetchColumn() {
		return array_shift($this->colmuns);
	}

	/**
	 *
	 */
	public function appendConstraint($con_name, $targets, $references = null) {
		$con_hash = array(
				'type' => $con_name,
				'targets' => $targets, // constraint target columns (itselfs)
				'references' => $references // foreign key constraint references columns
		);
		$this->constraints[] = $con_hash;
	}

	/**
	 *
	 */
	public function fetchConstraint() {
		return array_shift($this->constraints);
	}

	public function rewind() :void {
	    reset($this->colmuns);
	    reset($this->constraints);
	}
}


/**
 *
 */
class TableJoinObject {
    /**  */
    private $tbl_name = "";
    /**  */
    private $colmuns = array();
    /** */
    private $constraints = array();

    /**
     *
     */
    function __construct($tbl_name, $colmuns = null, $auto_primery = true) {
        $this->tbl_name = $tbl_name;
        if ($auto_primery) {
            $this->appendColumn('id', 'integer', ['primery' => true]);
        }
    }

    /**
     *
     */
    public function tableName() :string {
        return $this->tbl_name;
    }

    public function appendColumn(string $col_name, string $type, array $constraints = array()) :void {
        $col_hash = array(
                'name' => $col_name,
                'type' => $type,
                'default' => $constraints['default'] ?? null,
                'uniq' => (isset($constraints['uniq'])) ? (bool)$constraints['uniq'] : false,
                'not_null'=> (isset($constraints['not_null'])) ? (bool)$constraints['not_null'] : false,
                'primery' => (isset($constraints['primery'])) ? (bool)$constraints['primery'] : false,
                'increment' => (isset($constraints['increment'])) ? (bool)$constraints['increment'] : false,
                'foreign' => $constraints['foreign'] ?? null // reference and on delete
        );
        $this->colmuns[] = $col_hash;
    }

    /**
     *
     */
    public function fetchColumn() {
        return array_shift($this->colmuns);
    }

    /**
     *
     */
    public function appendConstraint($con_name, $targets, $references = null) {
        $con_hash = array(
                'type' => $con_name,
                'targets' => $targets, // constraint target columns (itselfs)
                'references' => $references // foreign key constraint references columns
        );
        $this->constraints[] = $con_hash;
    }

    /**
     *
     */
    public function fetchConstraint() {
        return array_shift($this->constraints);
    }

    public function rewind() :void {
        reset($this->colmuns);
        reset($this->constraints);
    }
}
