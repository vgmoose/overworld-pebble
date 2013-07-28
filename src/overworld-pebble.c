#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"


#define MY_UUID { 0xA7, 0xD0, 0x16, 0x1A, 0x5F, 0xB3, 0x40, 0xBB, 0xA1, 0x7A, 0x51, 0x19, 0x66, 0x16, 0x42, 0x2E }
PBL_APP_INFO(MY_UUID,
             "Overworld", "VGMoose",
             1, 0, /* App version */
             DEFAULT_MENU_ICON,
             APP_INFO_STANDARD_APP);

Window window;
RotBmpPairContainer bitmap_container;
Layer hourglass_layer;

int32_t current_layer_angle = 0;

int white_char[] = {RESOURCE_ID_MC_LEFT_WHITE, RESOURCE_ID_MC_DOWN_WHITE, RESOURCE_ID_MC_RIGHT_WHITE, RESOURCE_ID_MC_UP_WHITE};

int black_char[] = {RESOURCE_ID_MC_LEFT_BLACK, RESOURCE_ID_MC_DOWN_BLACK, RESOURCE_ID_MC_RIGHT_BLACK, RESOURCE_ID_MC_UP_BLACK};



void handle_timer(AppContextRef ctx, AppTimerHandle handle, uint32_t cookie) {
    
    // This implementation assumes only one type of timer used in the app.
    current_layer_angle = (current_layer_angle + 90) % 360;

    // This will automatically mark the layer dirty and update it.
    rotbmp_pair_layer_set_angle(&bitmap_container.layer, TRIG_MAX_ANGLE * current_layer_angle / 360);
    
   // app_timer_send_event(ctx, 50 /* milliseconds */, cookie);
    
}


void handle_init(AppContextRef ctx) {

  window_init(&window, "Overworld");
  window_stack_push(&window, true /* Animated */);
    
    resource_init_current_app(&VERSION);
    
    rotbmp_pair_init_container(white_char[1], black_char[1], &bitmap_container);
    
    GRect framey = GRect(10, 10, 100, 100);
    
    layer_init(&hourglass_layer, framey);
    
    layer_add_child(&hourglass_layer, &bitmap_container.layer.layer);
    layer_add_child(&window.layer, &hourglass_layer);
    
    // layer_add_child(&window.layer, &bitmap_container.layer.layer);
    
    
    // Start the animation.
    app_timer_send_event(ctx, 50 /* milliseconds */, 0 /* Not using a cookie value */);

}


void handle_deinit(AppContextRef ctx) {
    
    rotbmp_pair_deinit_container(&bitmap_container);
}



void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
      .deinit_handler = &handle_deinit,
      .timer_handler = &handle_timer

  };
  app_event_loop(params, &handlers);
}
