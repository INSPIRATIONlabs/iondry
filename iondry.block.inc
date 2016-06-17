<?php

class iondry_block {


  /**
   * Helper function to find and render a block.
   * @params  $module: Modulename,
   *          $delta: Identifier
   */
  public static function render($module, $delta) {
    $block = block_load($module,$delta);
    $renderable_block=  _block_get_renderable_array(_block_render_blocks(array($block)));
    return drupal_render($renderable_block);
  }

}
?>
