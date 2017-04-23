<?php

/**
 *
 * @author amderbar
 *
 */
class Validater
{

    /**
     * パラメータを指定されたルールに適合するかどうかを確かめ、適合しないルール名リストを返す
     * ルール名はスネークケース文字列で指定し、自動的にメソッド名に置き換える
     *
     * 置き換えルール
     * ルール名に接頭辞'is_'を付けてキャメルケースに直したもの
     *
     * @param unknown $value
     * @param array $rules
     * @return array
     */
    public static function validate ($value, array $rules):array
    {
        return array_filter($rules, function ($rule) use ($value) {
            if (preg_match('/\((.+)\)/', $rule, $params)) {
                $params = $params[1] ?? null;
                $rule = preg_replace('/\((.+)\)/', '', $rule);
            }

            $validateMethod = Str::camel('is_' . $rule);
            return !(self::$validateMethod($value, $params ?? null));
        });
    }

    /**
     * 配列で渡されたパラメータすべてを各々指定されたルールに適合するか確かめる
     *
     * @param array $params
     * @param array $rules
     * @return array
     */
    public static function bulkValidate(array $params, array $rules):array
    {
        $errors = array();
        foreach ($params as $key => $param) {
            if (isset($rules[$key]) && count($result = self::validate($param, $rules[$key]))) {
                $errors[$key] = $result;
            }
        }
        return $errors;
    }

    /**
     * パラメーターが指定されたルールセットのいずれかに適合することを確かめる
     * TODO:運用が難しそう
     *
     * @param array $set
     * @return bool
     */
    protected static function isOr($param, array $rule_set):bool
    {
        return array_sum(array_map(function ($rules) use ($param) {
            return self::validate($param, $rules);
        }, $rule_set)) > 0;
    }

    /**
     * パラメーターが整数値であることを確かめる
     *
     * @param unknown $param
     * @return bool
     */
    protected static function isInteger($param):bool
    {
        return ($param === '') || ($param === strval(intval($param)));
    }

    /**
     * パラメーターが文字列であることを確かめる
     *
     * @param unknown $param
     * @return bool
     */
    protected static function isString($param):bool
    {
        return ($param === '') || is_string($param);
    }

    /**
     * パラメーターが$max以下であることを確かめる
     *
     * @param unknown $param
     * @param int $max
     * @return bool
     */
    protected static function isMax($param, int $max):bool
    {
        if (is_array($param)) {
            return (count($param) <= $max);
        }
        if (self::isInteger($param)) {
            return ($param <= $max);
        }
        if (self::isString($param)) {
            return (strlen($param) <= $max);
        }
        return false;
    }

    /**
     * パラメーターが$min以上であることを確かめる
     *
     * @param unknown $param
     * @param int $min
     * @return bool
     */
    protected static function isMin($param, int $min):bool
    {
        if (is_array($param)) {
            return (count($param) >= $min);
        }
        if (self::isInteger($param)) {
            return ($param >= $min);
        }
        if (self::isString($param)) {
            return (strlen($param) >= $min);
        }
        return false;
    }
}