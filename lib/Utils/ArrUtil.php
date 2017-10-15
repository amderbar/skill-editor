<?php

namespace Amderbar\Lib\Utils;

/**
 * Undocumented class
 */
class ArrUtil
{
    /**
     *
     * @param array $seed_arr
     * @param string|int|null $key_col
     * @param string|int|null $val_col
     * @return array
     */
    public static function combine(array $seed_arr, $key_col, $val_col):array
    {
        return array_combine(
            is_null($key_col) ? array_keys($seed_arr) : array_column($seed_arr, $key_col),
            is_null($val_col) ? array_keys($seed_arr) : array_column($seed_arr, $val_col)
        );
    }

    /**
     * Undocumented function
     *
     * @param array $src
     * @param string ...$except_keys
     * @return array
     */
    public static function except(array $src, string ...$except_keys) :array
    {
        return array_filter($src, function ($key) use ($except_keys) {
            return !\in_array($key, $except_keys);
        }, ARRAY_FILTER_USE_KEY);
    }

    /**
     * @param string $glue
     * @param array $asoc
     * @return array
     */
    public static function implodeAssoc(string $glue, array $asoc) :array {
        $result = array();
        foreach ($asoc as $key => $child_arr) {
            if ( !is_array( $child_arr ) ) {
                $child_arr = array( $child_arr );
            }
            foreach ($child_arr as $val) {
                $result[] = "{$key}{$glue}{$val}";
            }
        }
        return $result;
    }

    /**
     * https://www.softel.co.jp/blogs/tech/archives/58 からコピペした
     * 多次元配列の次元数を調べる関数
     */
    public static function arrayDepth($a, $c = 0) {
        return (is_array($a) && count($a))
        ? max(array_map("Arr::array_depth", $a, array_fill(0, count($a), ++$c)))
        : $c;
    }

    /**
     * http://10000-hours.jp/wordpress/2015/09/2015091101/ からコピペした
     * 多次元配列の差分を求める関数
     */
    public static function arrayDiffAssocRecursive( ) {
        $args = func_get_args();
        //エラーチェック
        if( empty($args) ) {
            return false;
        }
        foreach($args as $array_one) {
            if( !is_array( $array_one ) ) {
                return false;
            }
        }
        //2つ以上配列の指定がある場合
        $difference = array();
        if( count($args) > 2 ) {
            $difference = array_shift( $args );
            foreach($args as $array_one) {
                $difference = array_diff_assoc_recursive( $difference , $array_one );
            }
        } else {
            foreach($args[0] as $key => $value) {
                if( is_array($value) ) {
                    if( !isset($args[1][$key]) || !is_array($args[1][$key]) ) {
                        $difference[$key] = $value;
                    } else {
                        $new_diff = array_diff_assoc_recursive($value, $args[1][$key]);
                        if( !empty($new_diff) ) {
                            $difference[$key] = $new_diff;
                        }
                    }
                } else if( !array_key_exists($key,$args[1]) || $args[1][$key] !== $value ) {
                    $difference[$key] = $value;
                }
            }
        }
        return $difference;
    }
}
