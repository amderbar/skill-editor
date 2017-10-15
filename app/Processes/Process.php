<?php

namespace Amderbar\App\Processes;

use Amderbar\Lib\Utils\DbUtil as Db;
use Amderbar\Lib\Utils\FileUtil as File;
use Amderbar\Lib\AmbException;
use stdClass;
use Exception;

/**
 *
 * @author amderbar
 *
 */
class Process {
    /**
     * @var array
     */
    private $con = array ();

    /**
     *
     */
    public function __construct()
    {
        try {
            $this->open(ROOT_DB_ID);
        } catch (AmbException $e) {
            $this->setupSystemDB();
        }
    }

    /**
     *
     * @return array
     */
    public function getDataTypeList() :array
    {
        return SQLiteHandler::getDataTypeDictionary();
    }

    /**
     * @param int $proj_id
     * @param bool $is_new
     * @return Db
     */
    public function open(int $proj_id, bool $is_new = false): Db
    {
        if (!isset($this->con[$proj_id])) {
            $db_file = ($proj_id) ? sprintf(RESOURCE_ROOT . '/proj%03d.db', $proj_id) : ROOT_DB;
            $db_file = File::fullPath($db_file, $is_new);
            if (!$db_file && !$is_new) {
                throw new AmbException('DB connection faild!');
            }
            $this->con[$proj_id] = Db::connect('sqlite:' . $db_file, function (\PDO $pdo) {
                $pdo->setAttribute(\PDO::ATTR_ERRMODE, \PDO::ERRMODE_EXCEPTION);
                $pdo->setAttribute(\PDO::ATTR_DEFAULT_FETCH_MODE, \PDO::FETCH_OBJ);
                $pdo->exec('PRAGMA foreign_keys = true;');
            });
        }
        return $this->con[$proj_id];
    }

    /**
     * @param int $proj_id
     */
    protected function close(int $proj_id): void
    {
        if (isset($this->con[$proj_id])) {
            $this->con[$proj_id]->disconnect();
            unset($this->con[$proj_id]);
        }
    }

    /**
     *
     */
    private function setupSystemDB() :Db
    {
        try {
            $sqls = [];
            // users table
            $sqls[] = 'create table users ('
            . implode(',', [
                'user_id integer primary key'
                , 'name text unique not null'
                , 'passwd text not null'
            ])
            . ');'; 
            // project table
            $sqls[] = 'create table projects ('
            . implode(',', [
                'proj_id integer primary key'
                , 'proj_name text not null'
                , 'owner integer not null references users(user_id) on delete cascade'
                , 'display_order integer not null'
                , 'update_at datetime not null default CURRENT_TIMESTAMP'
            ])
            . ');';
            return $this->open(ROOT_DB_ID, true)
                ->transaction(function (Db $db) use ($sqls) {
                    foreach ($sqls as $sql) {
                        $db->query($sql);
                    }
                    return $db;
                });
        } catch (PDOException | AmbException $e) {
            $this->handleException( $e );
        }
    }

    /**
     * @param int $proj_id
     * @param string $table
     * @return int
     */
    public function getDisplayOrderLast(int $proj_id, string $table) :int
    {
        return ($this->open($proj_id)->query("
            select max(display_order) as last
            from \"{$table}\"
        ")->fetch() ?: new stdClass)->last ?? 1;
    }

    /**
     * @param array $condition
     * @return stdClass|null
     */
    public function getUser(array $condition = []) :?stdClass
    {
        $where = ($condition) ? ' where '
            . implode(' and ', array_map(function ($col_name) {
                return "{$col_name} = :{$col_name}";
            }, array_keys($condition))) : '';
        $params = array_combine(array_map(function ($key) {
            return ":{$key}";
        }, array_keys($condition)), array_values($condition));

        return $this->open(ROOT_DB_ID)
            ->query("select * from users {$where}", $params)
            ->fetch() ?: null;
    }

    /**
     *
     * @return stdClass
     */
    public function registerUser(string $name, string $passwd) :stdClass
    {
        $usr = new stdClass;
        $usr->name = $name;
        $usr->passwd = $passwd;
        $usr->user_id = $this->open(ROOT_DB_ID)
            ->insertGetId('users', [
                'name' => $name,
                'passwd'=> $passwd
            ]);
        return $usr;
    }

    /**
     *
     * @param int $proj_id
     * @return array
     */
    public function listUsrTables(int $proj_id) :array
    {
        try {
            return $this->open($proj_id)
                ->query('
                    select tbl_id, tbl_name
                    from ' . SYSTEM_TBL . '
                    order by display_order, update_at
                ')
                ->fetchAll();
        } catch ( PDOException | AmbException $e ) {
            $this->handleException( $e );
        }
    }

    /**
     * @param Exception $e
     * @throws Exception
     */
    protected function handleException(Exception $e)
    {
        $err_str = get_class( $e ).' throwen:' . $e->getMessage() . PHP_EOL;
        $err_str .= ( $e->queryString ?? '' ) . PHP_EOL;
        $err_str .= var_export( ( $e->bindedParam ?? '' ), true ) . PHP_EOL;
        $err_str .= $e->getTraceAsString();
        error_log( $err_str );
        throw $e;
    }
}
