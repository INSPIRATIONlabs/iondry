<?php

class iondry_file {

  private static $instances = array() ;
  private $file;

  protected function __construct($fid) {
    $this->file = file_load($fid);
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

  public function lsNodes($field) {
    $r = array();
    $val = field_get_items('file', $this->file, $field);
    if(!empty($val[0])) {
      foreach($val as $item) {
        if(!empty($item['nid'])) {
          $r[] = iondry_node::instance($item['nid']);
        }
      }
      return $r;
    }
    return false;
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
      case 'text_with_summary':
        $out = field_view_value('file', $this->file, $field, $this->get($field));
        return render($out);
      case 'taxonomy_term_reference':
        return $this->getTaxonomyTermName($field);
      case 'text':
        return $this->getString($field);
      case 'datetime':
      case 'date':
        return $this->formatDate($this->get($field), $options);
      case 'image':
        return $this->image($this->get($field), $options);
      default:
        return $this->get($field);
    }
  }

  protected function formatDate($item, $options = array()) {
    $value = $item['value'];
    if($value) {
      if($options['to'] == true && $item['value2']) {
        $value = $item['value2'];
      }
      $date = new DateTime($value, new DateTimeZone($item['timezone_db']));
      $date->setTimezone(new DateTimeZone($item['timezone']));
      if($options['format']) {
        return format_date($date->getTimestamp(), 'custom', $options['format'], $item['timezone'], $options['lang']);
      }
    }
  }

  protected function image($item, $options) {
    if($item) {
      $arr = array('item' => $item);
      if($options['image_style']) {
        $arr['image_style'] = $options['image_style'];
      }
      if($options['class']) {
        $arr['image']['class'] = $options['class'];
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
    return $this->file->title;
  }

  public function fid() {
    return $this->file->fid;
  }

  public function url() {
    // return url('node/'. $this->file->nid);
  }

  public function ls($field) {
    $val = field_get_items('file', $this->file, $field);
    if(!empty($val[0])) {
      return $val;
    }
    return array();
  }

  protected function load($field, $cnt=false) {
    $val = field_get_items('file', $this->file, $field);
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
        if(module_exists('i18n_taxonomy')) {
          $t = i18n_taxonomy_localize_terms($t);
        }
        $result[] = $t;
      }
    }
    return $result;
  }

  /**
   * Factory method
   */
  public static function instance($n) {
    if(is_object($n) && $n->fid) {
      $fid = $n->fid;
    } elseif(is_array($n) && !empty($n['fid'])) {
      $fid = $n['fid'];
    } else {
      $fid = $n;
    }
    if(!isset(self::$instances[$fid])) {
      self::$instances[$fid] = new self($fid);
    }
    return self::$instances[$fid];
  }

}
