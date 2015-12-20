#include <pebble.h>
#include "gbitmap_color_palette_manipulator.h"

#define DEBUG 1

#define KEY_BASALT_COLORS 0
#define KEY_REQUEST_DATA 1

#define NUM_WIDTH 41
#define NUM_HEIGHT 60
#define BG_WIDTH 144
#define BG_HEIGHT 168

//number top left coordinates
#define X1 18
#define Y1  7

#define X2 79
#define Y2 24

#define X3 26
#define Y3 81

#define X4 87
#define Y4 98

#define COLOR1_DEF 0x005500
#define COLOR2_DEF 0x550000
#define COLOR3_DEF 0xFFAA00
#define NUM_COLORS 3

//Declarations
Window *main_window;
BitmapLayer *background_layer;
BitmapLayer *num1;
BitmapLayer *num2;
BitmapLayer *num3;
BitmapLayer *num4;

GBitmap *background;
GBitmap *empty;
GBitmap *numbers[10];

void hollowOutBitmap(GBitmap * bitmap,float percent){
  GRect bounds = gbitmap_get_bounds(bitmap);
  int width = bounds.size.w;
  int height = bounds.size.h;
  uint8_t * bitData = gbitmap_get_data(bitmap);
  int bytesPerRow = gbitmap_get_bytes_per_row(bitmap);
  int bytesPerPixel = width/bytesPerRow;
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Bytes per pixel:%d",bytesPerPixel);
  if(1){//bytesPerPixel == 8){
    uint8_t clear = GColorCeleste.argb;// (uint8_t)0b00000000;
    int row, byte;
    int changed = 0;
    int lowerBounds = height*(1.0-percent)/2;
    int upperBounds = height-(height*(1.0-percent)/2);
    int leftBounds = bytesPerRow*(1.0-percent)/2;
    int rightBounds = bytesPerRow-(bytesPerRow*(1.0-percent)/2);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Bounds: upper=%d, lower =%d, left=%d, right = %d",upperBounds,lowerBounds,leftBounds,rightBounds);
    for(row = 0;row<height;row++){

      //APP_LOG(APP_LOG_LEVEL_DEBUG, "%d < %d < %d?",lowerBounds,row,upperBounds);
      if((row > lowerBounds)&&(row < upperBounds)){
        for(byte = 0;byte<bytesPerRow;byte++){

          if(byte > leftBounds && byte < rightBounds){
              memcpy(&bitData[(row*bytesPerRow)+byte],&clear,sizeof(uint8_t));
              changed++;
          }
        }
      }
    }
    APP_LOG(APP_LOG_LEVEL_DEBUG, "%d percent should be cleared",(int)(percent*100));
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Cleared %d bits of %d, or %d percent",changed,bytesPerRow*bounds.size.h,(100*changed)/(bytesPerRow*bounds.size.h));
  }
  //gbitmap_set_data(bitmap, bitData, GBitmapFormat8Bit, bytesPerRow, false);
}

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Write the current hours and minutes into a buffer
  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ?
                                          "%H%M" : "%I%M", tick_time);

  //update bitmaps
  bitmap_layer_set_bitmap(num1,numbers[s_buffer[0] - '0']);
  bitmap_layer_set_bitmap(num2,numbers[s_buffer[1] - '0']);
  bitmap_layer_set_bitmap(num3,numbers[s_buffer[2] - '0']);
  bitmap_layer_set_bitmap(num4,numbers[s_buffer[3] - '0']);
  //hollowOutBitmap(background, (tick_time->tm_sec/60.0));
  layer_mark_dirty((Layer *)background_layer);
  
}
static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

//Colors
GColor colors[3];
void reload_bitmaps(){
  colors[0]=GColorFromHEX(COLOR1_DEF);
  colors[1]=GColorFromHEX(COLOR2_DEF);
  colors[2]=GColorFromHEX(COLOR3_DEF);
  GBitmap *old[12];
  old[0] = background;
  background = gbitmap_create_with_resource(RESOURCE_ID_BACKGROUND_C);
  old[1] = empty;
  empty = gbitmap_create_with_resource(RESOURCE_ID_EMPTY_C);
  old[2] = numbers[0];
  numbers[0] = gbitmap_create_with_resource(RESOURCE_ID_ZERO_C);
  old[3] = numbers[1];
  numbers[1] = gbitmap_create_with_resource(RESOURCE_ID_ONE_C);
  old[4] = numbers[2];
  numbers[2] = gbitmap_create_with_resource(RESOURCE_ID_TWO_C);
  old[5] = numbers[3];
  numbers[3] = gbitmap_create_with_resource(RESOURCE_ID_THREE_C);
  old[6] = numbers[4];
  numbers[4] = gbitmap_create_with_resource(RESOURCE_ID_FOUR_C);
  old[7] = numbers[5];
  numbers[5] = gbitmap_create_with_resource(RESOURCE_ID_FIVE_C);
  old[8] = numbers[6];
  numbers[6] = gbitmap_create_with_resource(RESOURCE_ID_SIX_C);
  old[9] = numbers[7];
  numbers[7] = gbitmap_create_with_resource(RESOURCE_ID_SEVEN_C);
  old[10] = numbers[8];
  numbers[8] = gbitmap_create_with_resource(RESOURCE_ID_EIGHT_C);
  old[11] = numbers[9];
  numbers[9] = gbitmap_create_with_resource(RESOURCE_ID_NINE_C);
  
  bitmap_layer_set_bitmap(background_layer,background);
  bitmap_layer_set_bitmap(num1,empty);
  bitmap_layer_set_bitmap(num2,empty);
  bitmap_layer_set_bitmap(num3,empty);
  bitmap_layer_set_bitmap(num4,empty);
  update_time();
  int i;
  for(i=0;i<12;i++){
    gbitmap_destroy(old[i]);
  }
;
}
GColor hex_string_to_gcolor(char * string){
  int color,base,i;
  color = 0;
  base = 1;
  for(i=1;i<=6;i++){
      char iChar = string[6-i];
      if(('a' <= iChar) && (iChar <= 'f')) color += ((int) iChar - 'a' + 10) * base;
      else if(('A' <= iChar) && (iChar <= 'F')) color += ((int) iChar - 'A' + 10) * base;
      else if(('0' <= iChar) && (iChar <= '9')) color += ((int) iChar - '0') * base;  
      base *= 16;
    }
  GColor gcolor = GColorFromHEX(color);
  return gcolor;
}
void gcolor_to_hex_string(char outstring[7], GColor color){
  int colorElem[3] = {color.r,color.g,color.b};
  int i;
  for(i=0;i<3;i++){
    char cval = '0';
    if(colorElem[i] == 0) cval = '0';
    else if(colorElem[i] == 1) cval = '5';
    else if(colorElem[i] == 2) cval = 'A';
    else if(colorElem[i] == 3) cval = 'F';
    outstring[i*2] = cval;
    outstring[(i*2)+1] = cval;
  }
  outstring[6] = '\0';
}
void replace_gcolor(GColor oldColor,GColor newColor){
  int i;
  replace_gbitmap_color(oldColor, newColor, background,NULL);
  replace_gbitmap_color(oldColor, newColor, empty,NULL);
  for(i=0;i<10;i++){
    replace_gbitmap_color(oldColor, newColor,numbers[i],NULL);
  }
}
void fix_conflicting_colors(GColor newColors[NUM_COLORS]){
  int c1,c2;
  for(c1=0;c1<NUM_COLORS;c1++){
    for(c2=0;c2<NUM_COLORS;c2++){
        if(gcolor_equal(colors[c1], newColors[c2])){
          //color conflict would occur, sub in a placeholder color
          char validChars[4] = {'0','5','A','F'};
          GColor tempColor;
          int cnum,valid;
          valid = 0;
          for(cnum = 0;(cnum<64&&!valid);cnum++){
            int temp, remainder, i;
            temp = cnum;
            remainder = cnum % 4;
            char colStr[7];
            colStr[6]= '\0';
            for(i=5;i>0;i-=2){
              colStr[i]=validChars[remainder];
              colStr[i-1]=colStr[i];
              temp -=remainder;
              if(temp == 0) remainder = 0;
              else{
                temp /= 4;
                remainder = temp % 4;
              }
            }
            tempColor = hex_string_to_gcolor(colStr);
            valid = 1;
            for(i=0;i<NUM_COLORS;i++){
              if(gcolor_equal(tempColor,colors[i])) valid = 0;
              if(gcolor_equal(tempColor,newColors[i])) valid = 0;
            }            
          }
          if(valid){
            replace_gcolor(colors[c1], tempColor);
            colors[c1] = tempColor;
            APP_LOG(APP_LOG_LEVEL_DEBUG, "Updated Colors: %s, %s, %s",get_gcolor_text(colors[0]),get_gcolor_text(colors[1]),get_gcolor_text(colors[2]));
            fix_conflicting_colors(newColors);
            return;
          }
        }
    }
  }
}//prevent common errors when swapping colors
void save_colors(){
  char buf[19];
  gcolor_to_hex_string(buf, colors[0]);
  gcolor_to_hex_string(&buf[6], colors[1]);
  gcolor_to_hex_string(&buf[12], colors[2]);
  persist_write_string(KEY_BASALT_COLORS,buf);
  APP_LOG(APP_LOG_LEVEL_DEBUG,"Saving color string %s",buf);
}//save color values to persistant storage
void update_colors(GColor newColors[NUM_COLORS]){
  if(DEBUG)APP_LOG(APP_LOG_LEVEL_DEBUG,"Updating Colors");
  if(DEBUG)APP_LOG(APP_LOG_LEVEL_DEBUG, "Old Colors: %s, %s, %s",get_gcolor_text(colors[0]),get_gcolor_text(colors[1]),get_gcolor_text(colors[2]));
  if(DEBUG)APP_LOG(APP_LOG_LEVEL_DEBUG, "New Colors: %s, %s, %s",get_gcolor_text(newColors[0]),get_gcolor_text(newColors[1]),get_gcolor_text(newColors[2]));
  if(gcolor_equal(colors[0], colors[1])
    ||gcolor_equal(colors[0],colors[2])
    ||gcolor_equal(colors[1],colors[2])){
    reload_bitmaps();
  }
  
  fix_conflicting_colors(newColors);
  int i;
  for(i=0;i<NUM_COLORS;i++){
    replace_gcolor(colors[i], newColors[i]);
    colors[i] = newColors[i];
  }
  layer_mark_dirty((Layer *) background_layer);
  layer_mark_dirty((Layer *) num1);
  layer_mark_dirty((Layer *) num2);
  layer_mark_dirty((Layer *) num3);
  layer_mark_dirty((Layer *) num4);
}
int colors_initialized = 0;
void initialize_colors(){
  colors[0]=GColorFromHEX(COLOR1_DEF);
  colors[1]=GColorFromHEX(COLOR2_DEF);
  colors[2]=GColorFromHEX(COLOR3_DEF);
  if(persist_exists(KEY_BASALT_COLORS)){
    char buf [19];
    persist_read_string(KEY_BASALT_COLORS, buf, 19);
    if(DEBUG)APP_LOG(APP_LOG_LEVEL_DEBUG,"Loaded color string %s",buf);
    GColor newColors[3];
    newColors[0] = hex_string_to_gcolor(buf);
    newColors[1] = hex_string_to_gcolor(&buf[6]);
    newColors[2] = hex_string_to_gcolor(&buf[12]);
    update_colors(newColors);
  }
  colors_initialized=1;
}
void send_colors(){
  if(!colors_initialized)initialize_colors();
  char buf[19];
  gcolor_to_hex_string(buf, colors[0]);
  gcolor_to_hex_string(&buf[6], colors[1]);
  gcolor_to_hex_string(&buf[12], colors[2]);
  DictionaryIterator * it;
  app_message_outbox_begin(&it);
  dict_write_cstring(it, KEY_BASALT_COLORS, buf);
  if(DEBUG)APP_LOG(APP_LOG_LEVEL_DEBUG,"Sending color string %s",buf);
  app_message_outbox_send();
}//send color values to config page



static void in_received_handler(DictionaryIterator *iter, void *context) {
  if(DEBUG)APP_LOG(APP_LOG_LEVEL_DEBUG,"Message recieved from javascript");
  Tuple *colors_tuple = dict_find(iter,KEY_BASALT_COLORS);
  Tuple *request_tuple = dict_find(iter,KEY_REQUEST_DATA);
  if(colors_tuple != NULL){
    if(DEBUG)APP_LOG(APP_LOG_LEVEL_DEBUG,"Javascript sending updated colors");
    int colorNum;
    GColor newColors[3];
    char buf [7];
    for(colorNum = 0; colorNum < NUM_COLORS; colorNum++){
      memcpy(buf, &(colors_tuple->value->cstring)[colorNum * 6] ,6);
      buf [6] = '\0';
      if(DEBUG)APP_LOG(APP_LOG_LEVEL_DEBUG, "New Color %d: %s",colorNum,buf);
      newColors[colorNum] = hex_string_to_gcolor(buf);
    }
    update_colors(newColors);
  }
  if(request_tuple != NULL){
    if(DEBUG)APP_LOG(APP_LOG_LEVEL_DEBUG,"Javascript requesting data");
    char *request = request_tuple->value->cstring;
    if(DEBUG)APP_LOG(APP_LOG_LEVEL_DEBUG,"Request: %s",request);
    if(strcmp((request),"color") == 0){
      if(DEBUG)APP_LOG(APP_LOG_LEVEL_DEBUG,"Data Requested: watchface colors");
      send_colors();
    }
  }
}





static void main_window_load(Window *window){
  //load bitmaps
  #if defined(PBL_BW)
  background = gbitmap_create_with_resource(RESOURCE_ID_BACKGROUND);
  empty = gbitmap_create_with_resource(RESOURCE_ID_EMPTY);
  numbers[0] = gbitmap_create_with_resource(RESOURCE_ID_ZERO);
  numbers[1] = gbitmap_create_with_resource(RESOURCE_ID_ONE);
  numbers[2] = gbitmap_create_with_resource(RESOURCE_ID_TWO);
  numbers[3] = gbitmap_create_with_resource(RESOURCE_ID_THREE);
  numbers[4] = gbitmap_create_with_resource(RESOURCE_ID_FOUR);
  numbers[5] = gbitmap_create_with_resource(RESOURCE_ID_FIVE);
  numbers[6] = gbitmap_create_with_resource(RESOURCE_ID_SIX);
  numbers[7] = gbitmap_create_with_resource(RESOURCE_ID_SEVEN);
  numbers[8] = gbitmap_create_with_resource(RESOURCE_ID_EIGHT);
  numbers[9] = gbitmap_create_with_resource(RESOURCE_ID_NINE);
  #elif defined(PBL_COLOR)
  background = gbitmap_create_with_resource(RESOURCE_ID_BACKGROUND_C);
  empty = gbitmap_create_with_resource(RESOURCE_ID_EMPTY_C);
  numbers[0] = gbitmap_create_with_resource(RESOURCE_ID_ZERO_C);
  numbers[1] = gbitmap_create_with_resource(RESOURCE_ID_ONE_C);
  numbers[2] = gbitmap_create_with_resource(RESOURCE_ID_TWO_C);
  numbers[3] = gbitmap_create_with_resource(RESOURCE_ID_THREE_C);
  numbers[4] = gbitmap_create_with_resource(RESOURCE_ID_FOUR_C);
  numbers[5] = gbitmap_create_with_resource(RESOURCE_ID_FIVE_C);
  numbers[6] = gbitmap_create_with_resource(RESOURCE_ID_SIX_C);
  numbers[7] = gbitmap_create_with_resource(RESOURCE_ID_SEVEN_C);
  numbers[8] = gbitmap_create_with_resource(RESOURCE_ID_EIGHT_C);
  numbers[9] = gbitmap_create_with_resource(RESOURCE_ID_NINE_C);
  #endif
  //load bitmap layers
  background_layer = bitmap_layer_create(GRect(0,0,BG_WIDTH,BG_HEIGHT));
  num1 = bitmap_layer_create(GRect(X1,Y1,NUM_WIDTH,NUM_HEIGHT));
  num2 = bitmap_layer_create(GRect(X2,Y2,NUM_WIDTH,NUM_HEIGHT));
  num3 = bitmap_layer_create(GRect(X3,Y3,NUM_WIDTH,NUM_HEIGHT));
  num4 = bitmap_layer_create(GRect(X4,Y4,NUM_WIDTH,NUM_HEIGHT));
  //set initial bitmaps
  bitmap_layer_set_bitmap(background_layer,background);
  bitmap_layer_set_bitmap(num1,empty);
  bitmap_layer_set_bitmap(num2,empty);
  bitmap_layer_set_bitmap(num3,empty);
  bitmap_layer_set_bitmap(num4,empty);
  //add bitmap layers to the window
  Layer *window_layer = window_get_root_layer(window);
  layer_add_child(window_layer, bitmap_layer_get_layer(background_layer));
  layer_add_child(window_layer, bitmap_layer_get_layer(num1));
  layer_add_child(window_layer, bitmap_layer_get_layer(num2));
  layer_add_child(window_layer, bitmap_layer_get_layer(num3));
  layer_add_child(window_layer, bitmap_layer_get_layer(num4));
}

static void main_window_unload(Window *window){
  //unload bitmap files
  gbitmap_destroy(background);
  gbitmap_destroy(empty);
  int i;
  for(i=0;i<10;i++){
    gbitmap_destroy(numbers[i]);
  }
  //unload bitmap layers
  bitmap_layer_destroy(background_layer);
  bitmap_layer_destroy(num1);
  bitmap_layer_destroy(num2);
  bitmap_layer_destroy(num3);
  bitmap_layer_destroy(num4);
}

void handle_init(void) {
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  //initialize main window, set handlers
  main_window = window_create();
  window_set_window_handlers(main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  window_stack_push(main_window, true);
    //initialize colors
  initialize_colors();
  //Register app message functions
  app_message_register_inbox_received(in_received_handler);
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  update_time();
}

void handle_deinit(void) {
  save_colors();
  window_destroy(main_window);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
