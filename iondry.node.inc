<?php

class iondry {

  private static $instances = array();
  private $node;

  protected function __construct($nid) {
    $this->node = node_load($nid);
  }

  public function fieldType($field) {
    $v = field_info_field('node', $this->node, $field);
    return $v['type'];
  }

  public function taxonomy($field) {
    $v = $this->loadTaxonomies($field, 1);
    if($v[0]) {
      return $v[0];
    }
    return false;
  }

  public function taxonomies($field) {
    return $this->loadTaxonomies($field);
  }


  public function taxonomyName($field) {
    $v = $this->loadTaxonomies($field, 1);
    if($v[0]) {
      return $v[0]->name;
    }
    return false;
  }

  public function taxonomyNames($field) {
    $r = array();
    $v = $this->loadTaxonomies($field);
    foreach($v as $i) {
      $r[] = $i->name;
    }
    return $r;
  }


  public function s($field) {
    $type = $this->fieldType($field);
    switch $type {
      case 'taxonomy_term_reference':
        return $this->taxonomyName($field);
        break;
      case 'text':
      default:
        return $this->get($field);
    }
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

  protected function load($field, $cnt=false) {
    $val = field_get_items('node', $this->node, $field);
    $r = array();
    if(!empty($val[0])) {
      if(!$cnt) {
        return $val;
      } else {
        for($i=0;$i<$cnt;$i++) {
          $r[] = $val[$i];
        }
      }
    }
    return $r;
  }

  protected function loadTaxonomies($field, $cnt=false) {
    $result = array();
    $res = $this->load($field, $cnt);
    foreach($res as $v) {
      if(!empty($v['tid'])) {
        $t = taxonomy_term_load($v['tid']);
        $result[] = $t;
      }
    }
    return $result;
  }

  /**
   * Factory method
   */
  public static function n($n) {
    if(is_object($n) && $n->nid) {
      $nid = $n->nid;
    } else {
      $nid = $n;
    }
    if(!isset(self::$instances[$nid])) {
      self::$instances[$nid] = new self($nid);
    }
    return self::$instances[$nid];
  }

}
