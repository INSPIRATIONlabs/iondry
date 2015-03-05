<?php

class iondry {

  private static $instances = array();
  private $node;

  protected function __construct($nid) {
    $this->node = node_load($nid);
  }

  public function fieldType($field) {

  }

  public function get($field) {
    $val = $this->ls($field);
    if(!empty($val[0])) {
      return $val;
    }
    return false;
  }

  public function ls($field) {
    $val = field_get_items('node', $this->node, $field);
    if(!empty($val[0])) {
      return $val;
    }
    return array();
  }

  /**
   * Factory method
   */
  public static function n($n) {
    if(is_object($n) && $n->nid) {
      $nid = $n;
    } else {
      $nid = $n;
    }
    if(!isset(self::$instances[$nid])) {
      self::$instances[$nid] = new self($nid);
    }
    return self::$instances[$nid];
  }

}
