<?php

class iondry_node {

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

  public function getNode($field) {
    $node = $this->get($field);
    if($node && $node['nid']) {
      return node_load($node['nid']);
    }
    return false;
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

  /**
   * returns a string representation of the field value
   */
  public function s($field, $options=array()) {
    $type = $this->fieldType($field);
    switch ($type) {
      case 'text_long':
        $out = field_view_value('node', $this->node, $field, $this->get($field));
        return render($out);
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

  protected function formatDate($item, $options = array()) {
    $value = $item['value'];
    if($options['to'] == true && $item['value2']) {
      $value = $item['value2'];
    }
    $date = new DateTime($value, new DateTimeZone($item['timezone_db']));
    $date->setTimezone(new DateTimeZone($item['timezone']));
    if($options['format']) {
      return format_date($date->getTimestamp(), 'custom', $options['format'], $item['timezone'], $options['lang']);
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
  public static function instance($n) {
    if(is_object($n) && $n->nid) {
      $nid = $n->nid;
    } elseif(is_array($n) && !empty($n['nid'])) {
      $nid = $n['nid'];
    } else {
      $nid = $n;
    }
    if(!isset(self::$instances[$nid])) {
      self::$instances[$nid] = new self($nid);
    }
    return self::$instances[$nid];
  }

}
