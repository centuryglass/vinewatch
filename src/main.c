#include <pebble.h>
#include "image_resources.h"

#define DEBUG 0

#define KEY_BASALT_COLORS 0
#define KEY_REQUEST_DATA 1
#define KEY_WATCH_VERSION 2

Window *main_window;

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Write the current hours and minutes into a buffer
  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ?
                                          "%H%M" : "%I%M", tick_time);
  //update bitmaps
  setTime(s_buffer);
}
static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static int watch_version = 0;

//Sends saved colors and watch version to javascript
void send_info(){
  char buf[19];
  get_color_string(buf);
  DictionaryIterator * it;
  app_message_outbox_begin(&it);
  dict_write_cstring(it, KEY_BASALT_COLORS, buf);
  dict_write_int16(it,KEY_WATCH_VERSION,watch_version);
  if(DEBUG)APP_LOG(APP_LOG_LEVEL_DEBUG,"Sending color string %s, watch version %d",buf,(int)watch_info_get_model());
  if(DEBUG)APP_LOG(APP_LOG_LEVEL_DEBUG,"Outbound message is of size %ld",(long)dict_size(it));
  app_message_outbox_send();
}


static void in_received_handler(DictionaryIterator *iter, void *context) {
  if(DEBUG)APP_LOG(APP_LOG_LEVEL_DEBUG,"Message recieved from javascript");
  Tuple *colors_tuple = dict_find(iter,KEY_BASALT_COLORS);
  Tuple *request_tuple = dict_find(iter,KEY_REQUEST_DATA);
  if(colors_tuple != NULL){
    if(DEBUG)APP_LOG(APP_LOG_LEVEL_DEBUG,"Javascript sending updated colors");
    update_colors(colors_tuple->value->cstring);
  }
  if(request_tuple != NULL){
    if(DEBUG)APP_LOG(APP_LOG_LEVEL_DEBUG,"Javascript requesting data");
    char *request = request_tuple->value->cstring;
    if(DEBUG)APP_LOG(APP_LOG_LEVEL_DEBUG,"Request: %s",request);
    send_info();   
  }
}


static void main_window_load(Window *window){
  if(DEBUG)APP_LOG(APP_LOG_LEVEL_DEBUG,"Loading main window");
  Layer *window_layer = window_get_root_layer(window);
  //load bitmap layers
  create_bitmapLayers(window_layer);
}

static void main_window_unload(Window *window){
  destroy_bitmapLayers();
}

void handle_init(void) {
  //Get watch version
  watch_version = watch_info_get_model(); 
  //Register app message functions
  app_message_register_inbox_received(in_received_handler);
  app_message_open(256,256);
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  //initialize main window, set handlers
  main_window = window_create();
  window_set_window_handlers(main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  window_stack_push(main_window, true);
  update_time();
  send_info();
}

void handle_deinit(void) {
  window_destroy(main_window);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
