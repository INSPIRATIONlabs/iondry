<?php

class iondry {

  public static function str_clean_css_identifier($string, $prefix) {
    return $prefix.strtolower(drupal_clean_css_identifier($string));
  }

  public static function css_identifier($input, $prefix = 'cls-') {
    if(!is_array($input)) {
      $input = array($input);
    }
    return array_map(self::str_clean_css_identifier, $input);
  }

  /**
   * Factory method
   */
  public static function n($n) {
    return iondry_node::instance($n);
  }

}
