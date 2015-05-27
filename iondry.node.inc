<?php

class iondry {

  private static $instances = array() ;
  private $node;

  protected function __construct($nid) {
    $this->node = node_load($nid);
  }

  public function fieldType($field) {
    $v = field_info_field($field);
    return $v['type'];
  }

  public function getTaxonomyTerm($field) {
    $v = $this->loadTaxonomies($field, 1);
    if($v[0]) {
      return $v[0];
    }
    return false;
  }

  public function lsTaxonomyTerms($field) {
    return $this->loadTaxonomies($field);
  }


  public function getTaxonomyTermName($field) {
    $v = $this->loadTaxonomies($field, 1);
    if($v[0]) {
      return $v[0]->name;
    }
    return false;
  }

  public function lsTaxonomyTermNames($field) {
    $r = array();
    $v = $this->loadTaxonomies($field);
    foreach($v as $i) {
      $r[] = $i->name;
    }
    return $r;
  }

  public function getString($field) {
    $v = $this->load($field);
    if(!empty($v) && $v[0]) {
      if($v[0]['safe_value']) {
        return $v[0]['safe_value'];
      } else {
        return $v[0]['value'];
      }
    }
  }

  public function s($field, $options=array()) {
    $type = $this->fieldType($field);
    switch ($type) {
      case 'taxonomy_term_reference':
        return $this->getTaxonomyTermName($field);
      case 'text':
        return $this->getString($field);
      case 'datetime':
        return $this->formatDate($this->get($field), $options);
      case 'image':
        return $this->image($this->get($field), $options);
      default:
        return $this->get($field);
    }
  }

  protected function formatDate($item, $options) {
    $date = new DateTime($item['value'], new DateTimeZone($item['timezone_db']));
    $date->setTimezone(new DateTimeZone($item['timezone']));
    if($options['format']) {
      return $date->format($options['format']);
    }
  }

  protected function image($item, $options) {
    if($item) {
      $arr = array('item' => $item);
      if($options['image_style']) {
        $arr['image_style'] = $options['image_style'];
      }
      return lazyretina_image_formatter($arr);
    }
  }

  public function get($field) {
    $val = $this->ls($field);
    if(!empty($val[0])) {
      return $val[0];
    }
    return false;
  }

  public function title() {
    return $this->node->title;
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
