/**
 * Pebble Torch
 *
 * Simple Pebble watchapp to toggle the screen's backlight for use as a torch/flashlight.
 *
 * Copyright (c) 2013, Rex McConnell. All rights reserved.
 *
 * @license BSD 3-Clause License (Revised) (@see LICENSE)
 * @author Rex McConnell <rex@rexmac.com>
 */
#include <pebble.h>

#define BULB_ON  RESOURCE_ID_ICON_LIGHT_BULB_ON
#define BULB_OFF RESOURCE_ID_ICON_LIGHT_BULB_OFF

static Window *window;
static ActionBarLayer *action_bar_layer;
static TextLayer *text_layer;
static AppTimer *warning_timer;

static void warning_timer_callback(void *data) {
  vibes_double_pulse();
  warning_timer = app_timer_register(10000, warning_timer_callback, NULL);
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  light_enable(true);
  if (warning_timer == NULL) {
    warning_timer = app_timer_register(10000, warning_timer_callback, NULL);
  }
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  light_enable(false);
  app_timer_cancel(warning_timer);
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // Initialize the action bar
  action_bar_layer = action_bar_layer_create();
  action_bar_layer_add_to_window(action_bar_layer, window);
  action_bar_layer_set_click_config_provider(action_bar_layer, click_config_provider);
  action_bar_layer_set_icon(action_bar_layer, BUTTON_ID_UP, gbitmap_create_with_resource(BULB_ON));
  action_bar_layer_set_icon(action_bar_layer, BUTTON_ID_DOWN, gbitmap_create_with_resource(BULB_OFF));

  // Initialize text layer (no longer used, but may be useful for debugging)
  /*
  text_layer = text_layer_create((GRect) {
    .origin = { 0, 72 },
    .size = { bounds.size.w - ACTION_BAR_WIDTH - 3, 40 }
  });
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));
  */
}

static void window_unload(Window *window) {
  text_layer_destroy(text_layer);
  action_bar_layer_destroy(action_bar_layer);
}

static void init(void) {
  // Initialize window
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_fullscreen(window, true);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(window, true);
}

static void deinit(void) {
  app_timer_cancel(warning_timer);
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
