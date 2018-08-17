<?php

class iondry_term{

  private $term;

  protected function __construct($term, $vid = null) {
    if(is_object($term) && $term->tid) {
      $this->term = $term;
    } else {
      $this->term = taxonomy_term_load($term->tid);
    }
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

  public function lsTaxonomyTermIDs($field) {
    $r = array();
    $v = $this->loadTaxonomies($field);
    foreach($v as $i) {
      $r[] = (int)$i->tid;
    }
    return $r;
  }

  public function lsTermIds($field) {
    if($this->term) {
      $r = array();
      $val = field_get_items('taxonomy_term', $this->term, $field);
      if(!empty($val[0])) {
        foreach($val as $item) {
          if(!empty($item['tid'])) {
            $r[] = $item['tid'];
          }
        }
      }
      return $r;
    }
    return false;
  }

  public function lsTerms($field) {
    if($this->term) {
      $r = array();
      $tids = array();
      $val = field_get_items('taxonomy_term', $this->term, $field);
      if(!empty($val[0])) {
        foreach($val as $item) {
          if(!empty($item['tid'])) {
            $tids[] = $item['tid'];
          }
        }
        $items = taxonomy_term_load_multiple($tids);
        foreach($items as $item) {
          $r[] = iondry_term::instance($item);
        }
        return $r;
      }
    }
    return false;
  }

  public function getTerm($field) {
    $term = $this->get($field);
    if($term && $term['tid'] && ($t = taxonomy_term_load($term['tid']))) {
      return iondry_term::instance($t);
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
   * returns a string representation of the field value of several fieldtypes
   */
  public function s($field, $options=array()) {
    if($this->term) {
      $type = $this->fieldType($field);
      switch ($type) {
        case 'text_long':
        case 'text_with_summary':
          $out = field_view_value('taxonomy_term', $this->term, $field, $this->get($field));
          return render($out);
        case 'taxonomy_term_reference':
          return $this->getTaxonomyTermName($field);
        case 'text':
        case 'list_text':
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
    return false;
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
      if($options['attributes']) {
        $arr['item']['attributes'] = $options['attributes'];
      }
      if($options['class']) {
        if(empty($arr['item']['attributes'])) {
          $arr['item']['attributes'] = array('class' => array());
          $arr['item']['attributes']['class'][] = $options['class'];
        }
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

  public function name() {
    if($this->term) {
      return $this->term->name;
    }
  }

  public function tid() {
    if($this->term) {
      return $this->term->tid;
    }
  }

  public function ttid() {
    return $this->term->i18n_tsid;
  }

  public function term() {
    if($this->taxonomy_term) {
      return $this->term;
    }
  }

  public function ls($field) {
    if($this->term) {
      $val = field_get_items('taxonomy_term', $this->term, $field);
      if(!empty($val[0])) {
        return $val;
      }
    }
    return array();
  }

  protected function load($field, $cnt=false) {
    $r = array();
    if($this->term) {
      $val = field_get_items('taxonomy_term', $this->term, $field);
      if(!empty($val[0])) {
        if(!$cnt) {
          return $val;
        } else {
          for($i=0;$i<$cnt;$i++) {
            $r[] = $val[$i];
          }
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
  public static function instance($t, $vid = null) {
    if(is_object($t) && $t->tid) {
      return new self($t);
    }
    if(is_array($t) && !empty($t['tid'])) {
      $tid = $t['tid'];
    } else {
      $tid = $t;
    }
    return new self($tid, $vid);
  }

}
