<?php

class iondry_view {


  /**
   * Loads nodes from a view
   */
  public function load_nodes($view, $display=false, $args=false, $options = array()) {
    $result = array();
    $nids = $this->load_nodeIds($view, $display, $args, $options);
    if(count($nids)) {
      $res = node_load_multiple($nids);
      foreach($res as $item) {
        $result[] = iondry::n($item);
      }
    }
    return $result;
  }

  public function load_nodeIds($view, $display=false, $args=false, $options = array()) {
    $nids = array();
    $view = views_get_view($view);
    if($view) {
      if($args) {
        $view->set_arguments($args);
      }
      if($display) {
        $view->set_display($display);
      }
      if($options && $options['limit']) {
        $view->set_items_per_page($options['limit']);
      }
      $view->execute();
      if(!empty($view->result) && $view->result[0]) {
        foreach($view->result as $item) {
          if(!empty($item->nid)) {
            $nids[] = $item->nid;
          } elseif(!empty($item->entity)) { // View of type search index returns different structured objects
            $nids[] = $item->entity;
          }
        }
      }
    }
    return $nids;
  }

  public function load_termIds($view, $display=false, $args=false, $options = array()) {
    $tids = array();
    $view = views_get_view($view);
    if($view) {
      if($args) {
        $view->set_arguments($args);
      }
      if($display) {
        $view->set_display($display);
      }
      if($options && $options['limit']) {
        $view->set_items_per_page($options['limit']);
      }
      $view->execute();
      if(!empty($view->result) && $view->result[0]) {
        foreach($view->result as $item) {
          if(!empty($item->tid)) {
            $tids[] = $item->tid;
          }
        }
      }
    }
    return $tids;
  }

  /**
   * Factory method
   */
  public static function instance() {
    return new self();
  }

}
