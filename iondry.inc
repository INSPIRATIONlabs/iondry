<?php

class iondry {

  public static function str_clean_css_identifier($string, $prefix) {
    return $prefix.strtolower(drupal_clean_css_identifier($string));
  }

  public static function css_identifiers($input, $prefix = 'cls-') {
    $result = array();
    if(!is_array($input)) {
      $input = array($input);
    }
    foreach($input as $str) {
      $result[] = self::str_clean_css_identifier($str, $prefix);
    }
    return $result;
  }

  /**
   * Factory method
   */
  public static function n($n) {
    return iondry_node::instance($n);
  }

  /**
   * Factory method
   */
  public static function t($term) {
    return iondry_term::instance($term);
  }

  /**
   * Factory method
   */
  public static function f($n) {
    return iondry_file::instance($n);
  }

  public static function v() {
    return iondry_view::instance();
  }
}
