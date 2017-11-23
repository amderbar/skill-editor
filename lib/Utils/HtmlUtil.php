<?php

namespace Amderbar\Lib\Utils;

/**
 * Utility Class for making html form.
 *
 * User: amderbar
 * Date: 16/07/12
 * Time: 9:16
 */
class HtmlUtil
{
    /** */
    // list属性、step属性によるバリエーションもある
    const FROM_TYPES = array(
            'text' => '一行テキスト',
            'listext' => '入力候補付き一行テキスト',
            'textarea' => '複数行テキスト',
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
            'numlist' => '入力候補付き数値(直接入力)',
            'range' => '数値(スライダー)',
            'color' => '色',
            'select' => 'プルダウン',
            'checkbox' => '単一チェックボックス',
            'multicheck' => '選択チェックボックス',
            'radio' => 'ラジオボタン',
            'file' => 'ファイル添付',
            'image' => '画像',
    );

    /**
     * Helper function for html escape.
     *
     * @param string $str
     * @return string
     */
    public static function escape(string $str) :string
    {
        return htmlspecialchars($str, ENT_QUOTES | ENT_SUBSTITUTE, ENCODE);
    }

    /**
    * Helper function for sanitize url. this is not yet complete.
    * FIXME: URLのきちんとしたサニタイズ
    * @param $url
    */
    public static function sanitizeUrl($url, $query = null) {
        $allow_scheme = array(
            'http' => true,
            'https' => true
        );
        $url_scheme = parse_url($url, PHP_URL_SCHEME);
        if ($url_scheme && !isset($allow_scheme[$url_scheme])) {
            mb_ereg_replace($url_scheme, '', $url);
        }
        if (empty($url)) {
           return null;
        }
        $tmp = array();
        foreach ($query ?? [] as $name => $value) {
            $tmp[] = self::escape($name).'='.urlencode(self::escape($value));
        }
        $query = implode('&', $tmp);
        $query = ($query) ? '?'.$query : $query;
        return $url.$query;
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
            $attr_str = self::escape($name);
            if (!is_bool($value)) {
                if (is_array($value)) {
                    $value = implode(' ', $value);
                }
                $attr_str .= '="'.self::escape($value).'"';
            }
            $tmp[] = $attr_str;
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
    public static function link($text, $url, $query = null, $additional = null) {
        $href = self::sanitizeUrl($url, $query);
        return '<a href="'.$href.'"'.self::attr($additional).'>'.self::escape($text).'</a>';
    }

    /**
    * Helper function for form open tag.
    * @param string $element_name
    * @param mixed $value
    * @param mixed hash $additional
    */
    public static function startForm($url, $query = null, $method = 'POST', $additional = null) {
        $href = self::sanitizeUrl($url, $query);
        return '<form action="'.$href.'" method="'.$method.'" '.self::attr($additional).'>';
    }

    /**
     * Helper function for form end tag.
     *
     * @return string
     */
    public static function endForm() :string
    {
        return '</form>';
    }

    /**
    * Helper function for print text-box like form.
    * @param string $element_name
    * @param mixed $val
    * @param mixed hash $additional
    */
    public static function input($type, $element_name, $val = null, $additional = null) {
        $value = ( isset($val) ) ? ' value="'.self::escape($val).'"' : '';
        $tag_str = '<input type="'.self::escape($type).'" name="'.self::escape($element_name).'"'.$value;
        $tag_str .= self::attr($additional).'>';
        return $tag_str;
    }

    /**
    * Helper function for print hidden tag.
    * @param string $element_name
    * @param mixed $value
    * @param mixed hash $additional
    */
    public static function hidden($element_name, $value, $additional = null) {
        return self::input('hidden', $element_name, $value, $additional);
    }

    /**
    * Helper function for print text-box.
    * @param string $element_name
    * @param mixed $value
    * @param mixed hash $additional
    */
    public static function textbox($element_name, $value = '', $additional = null) {
        return self::input('text', $element_name, $value, $additional);
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
        return self::input('number', $element_name, $value, $additional);
    }

    /**
    * Helper function for print the submit button.
    * @param $element_name
    * @param $label
    */
    public static function submit($element_name, $label, $form_id = null, $additional = null) {
        $form_attr = $form_id ? '" form="'.$form_id : '' ;
        $additional = $additional ? ' '.$additional : '' ;
        $tag_str = '<input type="submit" name="'.self::escape($element_name).'" value="';
        $tag_str .= self::escape($label) . $form_attr .'"'.$additional.'>';
        return $tag_str;
    }

    /**
    * Helper function for print the textarea.
    * @param $element_name
    * @param $value
    */
    public static function textarea($element_name, $value = null, $additional = null) {
        $tag_str = '<textarea name="'.self::escape($element_name).'" '.self::attr($additional).'>';
        $tag_str .= self::escape($value[$element_name]) . '</textarea>';
        return $tag_str;
    }

    /**
    * radio button or check box.
    * @param $type 'checkbox' or 'radio'
    * @param $element_name
    * @param $values is an array has checkbox value as key and label text as value of array.
    * @param $checked_values
    */
    public static function radiocheck( string $type, string $element_name, array $values, array $checked_values = array(), ?array $additional = null ) {
        $expected_type = [ 'checkbox' => true, 'radio' => true ];
        if ( !isset( $expected_type[$type] ) ) {
            throw new RuntimeException($type.' is not expected type attribute.');
        }
        $chked = array_flip( $checked_values );
        $html_collection = array();
        foreach ($values as $value => $label) {
            $add_each = $additional;
            if ( isset( $chked[ $value ] ) ) {
                $add_each['checked'] = true;
            }
            $tag_str = self::input( $type, $element_name, $value, $add_each );
            if ( $label || $label === 0) {
                $tag_str = '<label>'.$tag_str.self::escape($label).'</label>';
            }
            $html_collection[] = $tag_str;
        }
        return implode(PHP_EOL, $html_collection);
    }

    /**
    * select box
    * @param string $element_name
    * @param callable $options_build_callback
    * @param array $selected selected value(s). NOT label.
    * @param boolean $multiple
    * @param array $additional
    */
    public static function selectbox(
        string $element_name,
        callable $options_build_callback,
        array $selected = null,
        bool $multiple = false,
        array $additional = null) :string
    {
        // start select tag
        $tag_str = '<select name="'.self::escape($element_name);
        $tag_str .= ($multiple) ? '[]" multiple' : '"';
        $tag_str .= self::attr($additional).'>'.PHP_EOL;
        // options
        $tag_str .= (new class extends HtmlUtil
        {
            private $selected;
            private $options;
            public function __construct(array $selected = [])
            {
                $this->selected = array_flip($selected);
                $this->options = [];
            }
            public function optgroup(string $label, callable $optgrp_builder, array $attr = []) :void
            {
                $this->options[] = '<optgroup label="' . self::escape($label).'"'.self::attr($attr).'>';
                $optgrp_builder($this);
                $this->options[] = '</optgroup>';
            }
            public function option($value, string $label, array $attr = []) :void
            {
                $tag_str = '<option value="' . self::escape($value) . '"';
                $tag_str .= isset($this->selected[$value]) ? ' selected' : '';
                $tag_str .= self::attr($attr) . '>' . self::escape($label) . '</option>';
                $this->options[] = $tag_str;
            }
            public function toHtml() :string
            {
                return implode(PHP_EOL, $this->options);
            }
            public function build(callable $callback) :string
            {
                $callback($this);
                return $this->toHtml();
            }
        })->build($options_build_callback);
        // end select tag
        $tag_str .= '</select>'.PHP_EOL;
        return $tag_str;
    }

    /**
     * @param string $id_name
     * @param array $options
     * @return string
     */
    public static function datalist( string $id_name, array $options ) :string {
        $tag_str = '<datalist id="'.self::escape( $id_name ).'">'.PHP_EOL;
        foreach ($options as $value) {
            $tag_str .= '<option value="'.self::escape($value).'">'.PHP_EOL;
        }
        $tag_str .= '</datalist>'.PHP_EOL;
        return $tag_str;
    }

    /**
     * Undocumented function
     *
     * @return string
     */
    public static function csrfToken() :string
    {
        return uniqid();
    }
}

?>