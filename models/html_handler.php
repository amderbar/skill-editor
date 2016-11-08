<?php
/**
* 
*/
require_once($_SERVER['DOCUMENT_ROOT'].'/skill_editor/common.php');

/**
 * Utility Class for making html form.
 *
 * User: amderbar
 * Date: 16/07/12
 * Time: 9:16
 */
class HTMLHandler {
    /**  */
    private static $CODE = 'UTF-8';

    /**
    * Helper function for html escape.
    * @param $str
    */
    public static function specialchars($str) {
        return htmlspecialchars($str, ENT_QUOTES | ENT_SUBSTITUTE, self::$CODE);
    }
    
    /**
    * Helper function for print hidden tag.
    * @param $element_name
    * @param $values
    */
    public static function hidden($element_name, $value) {
        $tag_str = '<input type="hidden" name="' . $element_name . '" value="';
        $tag_str .= self::specialchars($value) . '">'.PHP_EOL;
        return $tag_str;
    }

    /**
    * Helper function for print text-box like form.
    * @param $element_name
    * @param $value
    */
    public static function input($type, $element_name, $value = '', $additional = null) {
        $additional = $additional ? ' '.$additional : '' ;
        $tag_str = '<input type="' . $type . '" name="' . $element_name . '" value="';
        $tag_str .= self::specialchars($value) .'"'.$additional.'>'.PHP_EOL;
        return $tag_str;
    }

    /**
    * Helper function for print text-box.
    * @param $element_name
    * @param $value
    */
    public static function input_text($element_name, $value = '', $additional = null) {
        return self::input('text', $element_name, $value, $additional);
    }

    /**
    * Helper function for print the submit button.
    * @param $element_name
    * @param $label
    */
    public static function input_submit ($element_name, $label, $form_id = null, $additional = null) {
        $form_attr = $form_id ? '" form="'.$form_id : '' ;
        $additional = $additional ? ' '.$additional : '' ;
        $tag_str = '<input type="submit" name="' . $element_name . '" value="';
        $tag_str .= self::specialchars($label) . $form_attr .'"'.$additional.'>'.PHP_EOL;
        return $tag_str;
    }

    /**
    * Helper function for print the submit button.
    * @param $element_name
    * @param $values
    */
    public static function input_textarea ($element_name, $values) {
        $tag_str = '<textarea name="' . $element_name . '">';
        $tag_str .= self::specialchars($values[$element_name]) . '</textarea>'.PHP_EOL;
        return $tag_str;
    }

    /**
    * radio button or check box.
    * @param $type
    * @param $element_name
    * @param $values is an array has checkbox value as key and label text as value of array.
    * @param $checked_value
    */
    public static function input_radiocheck ($type, $element_name, $values, $checked_value = null) {
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
            $tag_str .= self::specialchars($value) . '" ';
            if ($checked_value == $value) {
                $tag_str .= ' checked="checked"';
            }
            $tag_str .= '>';
            if ($label) {
                $tag_str .= self::specialchars($label).'</label>';
            }
            return $tag_str.PHP_EOL;
        }
    }

    /**
    * select box
    * @param $element_name
    * @param $selected selected option(s). NOT label.
    *        if $multiple == true then it should be an array, else it should be a scalar variable.
    * @param $options
    * @param bool $multiple
    */
    public static function input_select($element_name, $selected, $options, $multiple = false) {
    // start select tag
        $tag_str = '<select name="' . $element_name;
        if ($multiple) {
            $tag_str .= '[]" multiple="multiple';
        }
        $tag_str .= '">'.PHP_EOL;
        // set default
        $selected = array_flip($selected);
        // options
        foreach ($options as $option => $label) {
            $tag_str .= '<option value="' . self::specialchars($option) . '"';
            if (isset($selected[$option])) {
                $tag_str .= ' selected="selected"';
            }
            $tag_str .= '>' . self::specialchars($label) . '</option>'.PHP_EOL;
        }
        // end select tag
        $tag_str .= '</select>'.PHP_EOL;
        return $tag_str;
    }

    /**
    * data list
    * @param $id_name
    * @param $selected
    * @param $options
    * @param bool $multiple
    */
    public static function makeDatalist($id_name,$options) {
        $options = call_user_func_array('array_map',array_merge(array(null),$options));
            if (array_depth($options) > 1) {
                $options = $options[1];
            }
        $tag_str = '<datalist id="'.self::specialchars($id_name).'">'.PHP_EOL;
        foreach ($options as $value) {
            $tag_str .= '<option value="'.self::specialchars($value).'">'.PHP_EOL;
        }
        $tag_str .= '<option value="--">'.PHP_EOL;
        $tag_str .= '</datalist>'.PHP_EOL;
        return $tag_str;
    }
}

?>