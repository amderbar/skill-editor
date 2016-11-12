<?php
/**
* 
*/
require_once($_SERVER['DOCUMENT_ROOT'].'/skill_editor/common.inc');

/**
 * Utility Class for making html form.
 *
 * User: amderbar
 * Date: 16/07/12
 * Time: 9:16
 */
class HTMLHandler {
    /** */
    // list属性、step属性によるバリエーションもある
    public static $FROM_TYPES = array(
        'input tag' => array(
            'text' => '一行テキスト',
            'listext' => '入力候補付き一行テキスト',
            'tel' => '電話番号', // textと変わらない
            'url' => 'URL',
            'email' => 'メールアドレス',
            'password' => 'パスワード',
            'datetime' => 'UTC日時', // textと変わらない
            'date' => '日付',
            'month' => '月',
            'week' => '週',
            'time' => '時刻',
            'datetime-local' => 'ローカル日時', // timestampっぽい
            'number' => '数値(直接入力)',
            'range' => '数値(スライダー)',
            'color' => '色',
            'checkbox' => '単一チェックボックス',
            'multicheck' => '選択チェックボックス',
            'radio' => 'ラジオボタン',
            'file' => 'ファイル添付',
            'image' => '画像ボタン'
        ),
        'other tag' => array(
            'textarea' => '複数行テキスト',
            'selectbox' => 'プルダウン'
        )
    );

    /**
    * Helper function for html escape.
    * @param $str
    */
    public static function escape($str) {
        return htmlspecialchars($str, ENT_QUOTES | ENT_SUBSTITUTE, ENCODE);
    }
    
    /**
    * Helper function for sanitize url. this is not yet complete.
    * @param $url
    */
    public static function sanitizeUrl($url) {
        $allow_scheme = array(
            'http' => true,
            'https' => true
        );
        $url_scheme = parse_url($url, PHP_URL_SCHEME);
        if ($url_scheme && !isset($allow_scheme[$url_scheme])) {
            mb_ereg_replace($url_scheme, '', $url);
        }
        return $url;
    }

    /**
    * Helper function for Construction of attributes string.
    * @param mixed hash $attr_hash
    */
    public static function attr($attr_hash) {
        if (empty($attr_hash)) {
            return '';
        }
        $tmp = array();
        foreach ($attr_hash as $name => $value) {
            if (is_array($value)) {
                $value = implode(' ', $value);
            }
            $tmp[] = self::escape($name).'="'.self::escape($value).'"';
        }
        return ' '.implode(' ', $tmp);
    }

    /**
    * Helper function for hyper link.
    * @param string $text
    * @param string $href
    * @param mixed hash $query
    * @param mixed hash $additional
    */
    public static function link($text, $href, $query = null, $additional = null) {
        $href = self::sanitizeUrl($href);
        if (empty($href)) {
           return null;
        }
        $tmp = array();
        foreach ($query as $name => $value) {
            $tmp[] = self::escape($name).'='.urlencode(self::escape($value));
        }
        $query = implode('&', $tmp);
        $query = ($query) ? '?'.$query : $query;
        return '<a href="'.$href.$query.'"'.self::attr($additional).'>'.self::escape($text).'</a>';
    }

    /**
    * Helper function for print text-box like form.
    * @param string $element_name
    * @param mixed $value
    * @param mixed hash $additional
    */
    public static function inputForm($type, $element_name, $value = '', $additional = null) {
        $tag_str = '<input type="' . $type . '" name="' . $element_name . '" value="';
        $tag_str .= self::escape($value) .'"'.self::attr($additional).'>';
        return $tag_str;
    }
    
    /**
    * Helper function for print hidden tag.
    * @param string $element_name
    * @param mixed $value
    * @param mixed hash $additional
    */
    public static function hidden($element_name, $value, $additional = null) {
        return self::inputForm('hidden', $element_name, $value, $additional);
    }

    /**
    * Helper function for print text-box.
    * @param string $element_name
    * @param mixed $value
    * @param mixed hash $additional
    */
    public static function textbox($element_name, $value = '', $additional = null) {
        return self::inputForm('text', $element_name, $value, $additional);
    }

    /**
    * Helper function for print number-box.
    * @param string $element_name
    * @param mixed $value
    * @param mixed hash $additional
    */
    public static function number($element_name, $value = '', $step = null, $additional = array()) {
        if (isset($step)) {
            $additional['step'] = $step;
        }
        return self::inputForm('number', $element_name, $value, $additional);
    }

    /**
    * Helper function for print the submit button.
    * @param $element_name
    * @param $label
    */
    public static function submit($element_name, $label, $form_id = null, $additional = null) {
        $form_attr = $form_id ? '" form="'.$form_id : '' ;
        $additional = $additional ? ' '.$additional : '' ;
        $tag_str = '<input type="submit" name="' . $element_name . '" value="';
        $tag_str .= self::escape($label) . $form_attr .'"'.$additional.'>';
        return $tag_str;
    }

    /**
    * Helper function for print the textarea.
    * @param $element_name
    * @param $values
    */
    public static function textarea($element_name, $values) {
        $tag_str = '<textarea name="' . $element_name . '">';
        $tag_str .= self::escape($values[$element_name]) . '</textarea>';
        return $tag_str;
    }

    /**
    * radio button or check box.
    * @param $type
    * @param $element_name
    * @param $values is an array has checkbox value as key and label text as value of array.
    * @param $checked_value
    */
    public static function radiocheck ($type, $element_name, $values, $checked_value = null) {
        $tag_str_common = '<input type="' . $type . '" name="' . $element_name . '" value="';
        if (!is_array($values)) {
            $values = array($values => null);
        }
        foreach ($values as $value => $label) {
            if ($label) {
                $tag_str = '<label>' . $tag_str_common;
            } else {
                $tag_str = $tag_str_common;
            }
            $tag_str .= self::escape($value) . '" ';
            if ($checked_value == $value) {
                $tag_str .= ' checked="checked"';
            }
            $tag_str .= '>';
            if ($label) {
                $tag_str .= self::escape($label).'</label>';
            }
            return $tag_str.PHP_EOL;
        }
    }

    /**
    * select box
    * @param string $element_name
    * @param string or array $options
    * @param array $selected selected option(s). NOT label.
    * @param boolean $multiple
    * @param hash $additional
    */
    public static function selectbox($element_name, $options, $selected = null, $multiple = false, $additional = null) {
        // start select tag
        $tag_str = '<select name="'. $element_name;
        if ($multiple) {
            $tag_str .= '[]" multiple="multiple';
        }
        $tag_str .= '"'.self::attr($additional).'>'.PHP_EOL;
        // options
        if (is_array($options)) {
            if (array_depth($options) > 1) {
                $tag_str .= self::optgroups($options, $selected);
            } else {
                $tag_str .= self::options($options, $selected);
            }
        } else {
        // string $options is available
        // but this case should be only when it is made by options() or optgroups() function.
            $tag_str .= $options;
        }
        // end select tag
        $tag_str .= '</select>'.PHP_EOL;
        return $tag_str;
    }

    /**
    * select box options
    * @param array $options
    * @param array $selected selected option(s). NOT label.
    * @param hash $additional
    */
    public static function optgroups($optgrps, $selected = null, $additional = null) {
        $tag_str = '';
        foreach ($optgrps as $label => $options) {
            $tag_str .= '<optgroup label="' . self::escape($label).'"'.self::attr($additional).'>'.PHP_EOL;
            $tag_str .= self::options($options, $selected);
            $tag_str .= '</optgroup>'.PHP_EOL;
        }
        return $tag_str;
    }

    /**
    * select box options
    * @param array $options
    * @param array $selected selected option(s). NOT label.
    */
    private static function options($options, $selected = null) {
        // set default
        if (isset($selected)) {
            $selected = array_flip($selected);
        }
        // options
        $tag_str = '';
        foreach ($options as $option => $label) {
            $tag_str .= '<option value="' . self::escape($option) . '"';
            if (isset($selected[$option])) {
                $tag_str .= ' selected="selected"';
            }
            $tag_str .= '>' . self::escape($label) . '</option>'.PHP_EOL;
        }
        return $tag_str;
    }

    /**
    * data list
    * @param $id_name
    * @param $selected
    * @param $options
    * @param bool $multiple
    */
    public static function datalist($id_name,$options) {
        $options = call_user_func_array('array_map',array_merge(array(null),$options));
            if (array_depth($options) > 1) {
                $options = $options[1];
            }
        $tag_str = '<datalist id="'.self::escape($id_name).'">'.PHP_EOL;
        foreach ($options as $value) {
            $tag_str .= '<option value="'.self::escape($value).'">'.PHP_EOL;
        }
        $tag_str .= '<option value="--">'.PHP_EOL;
        $tag_str .= '</datalist>'.PHP_EOL;
        return $tag_str;
    }
}

?>