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

int white_move[] = {RESOURCE_ID_MC_LEFT_M_WHITE, RESOURCE_ID_MC_DOWN_M_WHITE, RESOURCE_ID_MC_RIGHT_M_WHITE, RESOURCE_ID_MC_UP_M_WHITE};

int black_move[] = {RESOURCE_ID_MC_LEFT_M_BLACK, RESOURCE_ID_MC_DOWN_M_BLACK, RESOURCE_ID_MC_RIGHT_M_BLACK, RESOURCE_ID_MC_UP_M_BLACK};

int current_direction = 0;

int x_coor = 32, y_coor = 32;

bool walk = false;

AppContextRef real_ctx;


void handle_timer(AppContextRef ctx, AppTimerHandle handle, uint32_t cookie) {
    
    // This implementation assumes only one type of timer used in the app.
    current_layer_angle = 90 % 360;
    
    GRect frame = layer_get_frame(&bitmap_container.layer.layer);
        
    layer_remove_from_parent(&bitmap_container.layer.layer);
    rotbmp_pair_deinit_container(&bitmap_container);
    
    if (walk || x_coor%16!=0 || y_coor%16!=0)
    {
        x_coor += -2*(current_direction==0) + 2*(current_direction==2);
        y_coor += -2*(current_direction==3) + 2*(current_direction==1);
        
        app_timer_send_event(real_ctx, 50, 0);
        
        rotbmp_pair_init_container(white_move[current_direction], black_move[current_direction], &bitmap_container);

    }
    else
        rotbmp_pair_init_container(white_char[current_direction], black_char[current_direction], &bitmap_container);
    
    frame = layer_get_frame(&bitmap_container.layer.layer);
    frame.origin.x = x_coor;
    frame.origin.y = y_coor;
    layer_set_frame(&bitmap_container.layer.layer, frame);
    
    layer_add_child(&hourglass_layer, &bitmap_container.layer.layer);

   // layer_mark_dirty(&bitmap_container);
    rotbmp_pair_layer_set_angle(&bitmap_container.layer, TRIG_MAX_ANGLE * current_layer_angle / 360);

    
    
   // layer_mark_dirty(&bitmap_container.layer);

    
   // app_timer_send_event(ctx, 50 /* milliseconds */, cookie);
    
}

void toggleWalk()
{
    walk = !walk;
    
    if (walk)
        app_timer_send_event(real_ctx, 50 /* milliseconds */, 0 /* Not using a cookie value */);
}



void turnChar(int turnValue)
{
//    if (current_direction <= 1)
//        turnValue *= -1;
    
    if (!walk)
    {
        if (x_coor%16==0 && y_coor%16==0)
        {
            current_direction = (current_direction+turnValue)%4;
            
            if (current_direction <0)
                current_direction = 3;
            
            app_timer_send_event(real_ctx, 50 /* milliseconds */, 0 /* Not using a cookie value */);
        }
        else
        {
            turnChar(turnValue);
        }
    } else
    {
        toggleWalk();
        turnChar(turnValue);
        
    }

}


void handle_deinit(AppContextRef ctx) {
    
    rotbmp_pair_deinit_container(&bitmap_container);
}

// Modify these common button handlers
void up_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
    (void)recognizer;
    (void)window;
    
    turnChar(1);
    
}

void select_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
    (void)recognizer;
    (void)window;
    
    toggleWalk();
}

void down_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
    (void)recognizer;
    (void)window;
    
    turnChar(-1);
}


// This usually won't need to be modified

void click_config_provider(ClickConfig **config, Window *window) {
    (void)window;
    
    config[BUTTON_ID_UP]->click.handler = (ClickHandler) up_single_click_handler;
    config[BUTTON_ID_UP]->click.repeat_interval_ms = 100;
    
    config[BUTTON_ID_SELECT]->click.handler = (ClickHandler) select_single_click_handler;
    config[BUTTON_ID_SELECT]->click.repeat_interval_ms = 100;
    
    config[BUTTON_ID_DOWN]->click.handler = (ClickHandler) down_single_click_handler;
    config[BUTTON_ID_DOWN]->click.repeat_interval_ms = 100;

}


void handle_init(AppContextRef ctx) {
    
    window_init(&window, "Overworld");
    window_stack_push(&window, true /* Animated */);
    
    resource_init_current_app(&VERSION);
    
    rotbmp_pair_init_container(white_char[current_direction], black_char[current_direction], &bitmap_container);
    
    GRect framey = GRect(10, 10, 100, 100);
    
    layer_init(&hourglass_layer, framey);
    
    layer_add_child(&hourglass_layer, &bitmap_container.layer.layer);
    layer_add_child(&window.layer, &hourglass_layer);
    
    // layer_add_child(&window.layer, &bitmap_container.layer.layer);
    
    real_ctx = ctx;
    
    // Start the animation.
    app_timer_send_event(ctx, 50 /* milliseconds */, 0 /* Not using a cookie value */);
    
    window_set_click_config_provider(&window, (ClickConfigProvider) click_config_provider);
    
    
}


void pbl_main(void *params) {
    window_set_fullscreen(&window, true);
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
      .deinit_handler = &handle_deinit,
      .timer_handler = &handle_timer

  };
  app_event_loop(params, &handlers);
}
