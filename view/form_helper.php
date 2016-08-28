<?php
/**
 * Utility Class for making html form.
 *
 * Created by PhpStorm.
 * And modefied by Visual Studio Code.
 * User: kohei.sakata
 * Date: 16/07/12
 * Time: 9:16
 */
class FormHelper {
  
  /**
  * Helper function for print text-box.
  * @param $element_name
  * @param $values
  */
  public static function input_text($element_name, $values, $type = 'text') {
    echo '<input type="' . $type . '" name="' . $element_name . '" value="';
    echo htmlentities($values[$element_name]) . '">'.PHP_EOL;
  }

  /**
  * Helper function for print the submit button.
  * @param $element_name
  * @param $label
  */
  public static function input_submit ($element_name, $label, $form_id = null) {
    $form_attr = $form_id ? '" form="'.$form_id : '' ;
    echo '<input type="submit" name="' . $element_name . '" value="';
    echo htmlentities($label) . $form_attr .'">'.PHP_EOL;
  }

  /**
  * Helper function for print the submit button.
  * @param $element_name
  * @param $values
  */
  public static function input_textarea ($element_name, $values) {
    echo '<textarea name="' . $element_name . '">';
    echo htmlentities($values[$element_name]) . '</textarea>'.PHP_EOL;
  }

  /**
  * radio button or check box.
  * @param $type
  * @param $element_name
  * @param $values
  * @param $element_value
  */
  public static function input_radiocheck ($type, $element_name, $values, $element_value) {
    echo '<input type="' . $type . '" name="' . $element_name . '" value="' . $element_value . '" ';
    if ($element_value == $values[$element_name]) {
      echo ' checked="checked"';
    }
    echo '>'.PHP_EOL;
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
    echo '<select name="' . $element_name;
    if ($multiple) {
      echo '[]" multiple="multiple';
    }
    echo '">'.PHP_EOL;
    // set default
    $selected_options = array();
    if ($multiple) {
      foreach ($selected as $val) {
        $selected_options[$val] = true;
      }
    } else {
      $selected_options[$selected] = true;
    }
    // options
    foreach ($options as $option => $label) {
      echo '<option value="' . htmlentities($option) . '"';
      if ($selected_options[$option]) {
        echo ' selected="selected"';
      }
      echo '>' . htmlentities($label) . '</option>'.PHP_EOL;
    }
    // end select tag
    echo '</select>'.PHP_EOL;
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
		echo '<datalist id="'.htmlentities($id_name).'">'.PHP_EOL;
		foreach ($options as $value) {
			echo '<option value="'.htmlentities($value).'">'.PHP_EOL;
		}
		echo '<option value="--">'.PHP_EOL;
		echo '</datalist>'.PHP_EOL;
	}

}