<?php

namespace Amderbar\Lib\Utils;

use PDO;
use PDOStatement;
use Closure;
use PDOException;
use Exeption;

/**
 * Undocumented class
 */
class DbUtil {

    private $pdo;

    /**
     * Create new connection to db
     *
     * @param string  $dsn 
     * @param Closure  $post_connect
     * @throws PDOException
     * @return PDO
     */
    public static function connect(string $dsn, Closure $post_connect = null) :self
    {
        try {
            $pdo = new PDO($dsn);
            if (isset($post_connect)) {
                $post_connect($pdo);
            }
            return new self($pdo);

        } catch (PDOException $e){
            error_log('Connection failed:'. $e->getMessage());
            throw $e;
        }
    }

    /**
     * Undocumented function
     *
     * @return void
     */
    public function disconnect() :void
    {
        $this->pdo = null;
    }

    /**
     * @param PDO $pdo
    **/
    private function __construct(PDO $pdo)
    {
        $this->pdo = $pdo;
    }

    /**
     *
    **/
    public function transaction(Closure $process_block)
    {
        try {
            $inTransaction = !$this->beginTransaction();
            $ret_val = $process_block($this);
            if (!$inTransaction) {
                $this->commit();
            }
            return $ret_val;
        } catch (Exeption $e) {
            if (!$inTransaction) {
                $this->rollback();
            }
            throw $e;
        }
    }

    /**
     * 
    **/
    public function beginTransaction() :bool
    {
        if (!$faild = $this->pdo->inTransaction()) {
            $faild |= !$this->pdo->beginTransaction();
        }
        return !$faild;
    }

    /**
     * 
    **/
    public function commit() :bool
    {
        return $this->pdo->commit();
    }

    /**
     * 
    **/
    public function rollback(string $identifier = null) :bool
    {
        return $this->pdo->rollback();
    }

    /**
     * 
    **/
    public function query(string $sql, array $parms = null) :PDOStatement
    {
        $stmt = $this->pdo->prepare($sql);
        if ($stmt->execute($parms)) {
            return $stmt;
        } else {
            throw new DBManageExeption('Execute Prepared statement Failed.');
        }
    }

    /**
     * Undocumented function
     *
     * @param string  $sql
     *
     * @return string
     */
    public function quote(string $sql) :string
    {
        $quoted = $this->pdo->quote($sql);
        if ($quoted === false) {
            throw new DBManageExeption('quote is not supported!');
        }
        return $quoted;
    }

    /**
     * Undocumented function
     *
     * @param string  $table
     * @param array  $params
     *
     * @return int
     */
    public function insertGetId(string $table, array $params) :int
    {
        $sql = 'insert into ' . $this->quote($table);
        $sql .= '(' . \implode(',', \array_keys($params)) . ')';
        $sql .= 'values';
        $sql .= '(' . \implode(',', \array_pad([], count($params), '?')) . ')';
        $this->query($sql, array_values($params));
        return $this->pdo->lastInsertId();
    }
}
