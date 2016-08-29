<?php

class iondry_view {

  /**
   * Loads nodes from a view
   */
  public function load_nodes($view, $display=false, $args=false, $options = array()) {
    $result = array();
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
            $result[] = iondry::n($item->nid);
          }
        }
      }
      return $result;
    }
  }

  /**
   * Factory method
   */
  public static function instance() {
    return new self();
  }

}
