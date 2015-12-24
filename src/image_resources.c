#include <pebble.h>
#include "image_resources.h"

#define DEBUG 0

#define KEY_BASALT_COLORS 0
#define KEY_REQUEST_DATA 1
#define KEY_WATCH_VERSION 2

#define NUM_WIDTH 41
#define NUM_HEIGHT 60
#define BG_WIDTH 144
#define BG_HEIGHT 168

//number top left coordinates
#ifndef PBL_PLATFORM_CHALK
#define X1 18
#define Y1  7

#define X2 79
#define Y2 24

#define X3 26
#define Y3 81

#define X4 87
#define Y4 98
#endif

//round coordinates
#ifdef PBL_PLATFORM_CHALK
#define X1 36
#define Y1 13

#define X2 97
#define Y2 30

#define X3 44
#define Y3 87

#define X4 105
#define Y4 104
#endif

#ifdef PBL_PLATFORM_APLITE
#define BGCOLOR_DEF 0xFFFFFF
#define NUMCOLOR_0_DEF 0x555555
#define NUMCOLOR_1_DEF 0x000000
#endif

#ifdef PBL_COLOR
#define BGCOLOR_DEF 0x005500
#define NUMCOLOR_0_DEF 0x550000
#define NUMCOLOR_1_DEF 0xFFAA00
#endif

GColor bgColor;
GColor numColors[2];
int colors_initialized = 0;

uint32_t numResources [10][2] = 
{{RESOURCE_ID_0C1,RESOURCE_ID_0C2},
  {RESOURCE_ID_1C1,RESOURCE_ID_1C2},
  {RESOURCE_ID_2C1,RESOURCE_ID_2C2},
  {RESOURCE_ID_3C1,RESOURCE_ID_3C2},
  {RESOURCE_ID_4C1,RESOURCE_ID_4C2},
  {RESOURCE_ID_5C1,RESOURCE_ID_5C2},
  {RESOURCE_ID_6C1,RESOURCE_ID_6C2},
  {RESOURCE_ID_7C1,RESOURCE_ID_7C2},
  {RESOURCE_ID_8C1,RESOURCE_ID_8C2},
  {RESOURCE_ID_9C1,RESOURCE_ID_9C2}};


Layer * window_layer = NULL;
BitmapLayer *margin_layer = NULL;
BitmapLayer *num1[2] = {NULL, NULL};
BitmapLayer *num2[2] = {NULL, NULL};
BitmapLayer *num3[2] = {NULL, NULL};
BitmapLayer *num4[2] = {NULL, NULL};

GBitmap *marginBitmap = NULL;
GBitmap *numBitmaps[10][2] = {{NULL,NULL},{NULL,NULL},
  {NULL,NULL},{NULL,NULL},
  {NULL,NULL},{NULL,NULL},
  {NULL,NULL},{NULL,NULL},
  {NULL,NULL},{NULL,NULL},};


#ifdef PBL_PLATFORM_APLITE
GBitmap *maskBitmap = NULL;

struct maskLayer{
  BitmapLayer * mask;
  BitmapLayer * parent;
  struct maskLayer * next;
};
struct maskLayer * first;

//destroy the mask layer of a given bitmapLayer, if it exists
static void destroy_mask_layer(BitmapLayer *parent){
  struct maskLayer * index = first;
  if(first->parent == parent){
    bitmap_layer_destroy(first->mask);
    free(first);
    first = NULL;
  }
  while(index->next != NULL){
    if(index->next->parent == parent){
      struct maskLayer * old = index->next;
      layer_remove_from_parent(bitmap_layer_get_layer(old->mask));
      bitmap_layer_destroy(old->mask);
      index->next = old->next;
      free(old); 
    }
    index = index->next;
  }
}
//create a new mask layer, if one doesn't already exist
static void create_mask_layer(BitmapLayer * parent){
  struct maskLayer * index;
  if(first == NULL){
    first = malloc(sizeof(struct maskLayer));
    index = first;
  }else{
    index = first;
    while(index->next != NULL){
      if(index->parent == parent) return;
      index = index->next;
    }
    index->next = malloc(sizeof(struct maskLayer));
    index = index->next;
  }
  //index now points to the new mask layer
  index->parent = parent;
  index->next = NULL;
  index->mask = bitmap_layer_create(layer_get_bounds
                                      (bitmap_layer_get_layer(parent)));
  bitmap_layer_set_bitmap(index->mask, maskBitmap);
  bitmap_layer_set_compositing_mode(index->mask, GCompOpSet);
  layer_add_child(bitmap_layer_get_layer(parent), bitmap_layer_get_layer(index->mask));
}
//destroy all mask layers
static void destroy_mask_layers(){
  struct maskLayer * index = first;
  while(index != NULL){
    struct maskLayer * old = index;
    index = index->next;
    layer_remove_from_parent(bitmap_layer_get_layer(old->mask));
    bitmap_layer_destroy(old->mask);
    free(old); 
  }
}
#endif

//Given a GColor, copy its hex value into a buffer string
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

//Given a color hex string, return a corresponding GColor
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

//Loads initial bitmap resources into memory
void load_bitmaps(){
  numBitmaps[0][0]=gbitmap_create_with_resource(numResources[0][0]);
  numBitmaps[0][1]=gbitmap_create_with_resource(numResources[0][1]);
  marginBitmap = gbitmap_create_with_resource(RESOURCE_ID_MARGIN);
}

//Unloads all bitmap resources from memory
void unload_bitmaps(){
  #ifdef PBL_PLATFORM_APLITE
  if(maskBitmap != NULL){
    gbitmap_destroy(maskBitmap);
    maskBitmap = NULL;
  }
  #endif
  int n, c;
  for(n=0;n<10;n++){
    for(c=0;c<2;c++){
      if(numBitmaps[n][c] != NULL)
        gbitmap_destroy(numBitmaps[n][c]);
        numBitmaps[n][c] = NULL;
    }
  }
  gbitmap_destroy(marginBitmap);
  marginBitmap = NULL; 
}

//applies a color to a single bitmap layer
void apply_color(GColor color,BitmapLayer *layer){
  if(DEBUG)APP_LOG(APP_LOG_LEVEL_DEBUG,"applying color to layer");
  GColor * palette = gbitmap_get_palette(bitmap_layer_get_bitmap(layer));
  char col1[7],col2[7];
  gcolor_to_hex_string(col1,palette[0]);
  gcolor_to_hex_string(col2,palette[1]);
  
  if(!gcolor_equal(palette[0], GColorClear))
    palette[0].argb = color.argb;
  if(!gcolor_equal(palette[1], GColorClear))
    palette[1].argb = color.argb;
  #ifdef PBL_PLATFORM_APLITE
  if(!(gcolor_equal(color, GColorWhite))||
      (gcolor_equal(color, GColorBlack)))create_mask_layer(layer); //color is gray, apply gray mask to layer
  else destroy_mask_layer(layer);
  #endif
  layer_mark_dirty(bitmap_layer_get_layer(layer));
}

//Makes sure all initialized bitmaps are set to the correct colors
void apply_colors(){
  if(DEBUG)APP_LOG(APP_LOG_LEVEL_DEBUG,"Applying color updates");
  //update background color fill
  if(window_layer != NULL){
    layer_mark_dirty(window_layer);
  }
  //update first color
  
  apply_color(numColors[0],num1[0]);
  apply_color(numColors[0],num2[0]);
  apply_color(numColors[0],num3[0]);
  apply_color(numColors[0],num4[0]);
  apply_color(numColors[0],margin_layer);
  //update second color
  apply_color(numColors[1],num1[1]);
  apply_color(numColors[1],num2[1]);
  apply_color(numColors[1],num3[1]);
  apply_color(numColors[1],num4[1]);
}

//Initializes variables and loads colors when the watchface loads
void initialize_colors(){
  if(DEBUG)APP_LOG(APP_LOG_LEVEL_DEBUG,"Initializing colors");
  bgColor=GColorFromHEX(BGCOLOR_DEF);
  numColors[0]=GColorFromHEX(NUMCOLOR_0_DEF);
  numColors[1]=GColorFromHEX(NUMCOLOR_1_DEF);
  colors_initialized=1;
  if(persist_exists(KEY_BASALT_COLORS)){
    char buf [19];
    persist_read_string(KEY_BASALT_COLORS, buf, 19);
    if(DEBUG)APP_LOG(APP_LOG_LEVEL_DEBUG,"Loaded color string %s",buf);
        update_colors(buf);    
  }else apply_colors();  
}

//Copies the color string into a buffer
void get_color_string(char buffer[19]){
  gcolor_to_hex_string(buffer, bgColor);
  gcolor_to_hex_string(&buffer[6], numColors[0]);
  gcolor_to_hex_string(&buffer[12], numColors[1]);
}

//save color values to persistant storage
void save_colors(){
  if(!colors_initialized)initialize_colors();
  char buf[19];
  get_color_string(buf);
  persist_write_string(KEY_BASALT_COLORS,buf);
  if(DEBUG)APP_LOG(APP_LOG_LEVEL_DEBUG,"Saving color string %s",buf);
}

//change color values to the ones stored in a color string
void update_colors(char * colorStr){
  if(DEBUG)APP_LOG(APP_LOG_LEVEL_DEBUG,"Updating all colors");
  int colorNum;
  GColor newColors[NUM_COLORS];
  char buf [7];
  for(colorNum = 0; colorNum < NUM_COLORS; colorNum++){
      memcpy(buf, &colorStr[colorNum * 6] ,6);
      buf [6] = '\0';
      if(DEBUG)APP_LOG(APP_LOG_LEVEL_DEBUG, "New Color %d: %s",colorNum,buf);
      newColors[colorNum] = hex_string_to_gcolor(buf);
  }
   //save new colors
  bgColor = newColors [0];
  numColors[0] = newColors[1];
  numColors[1] = newColors[2];
  save_colors();
  //apply color changes
  apply_colors();
}


//fill background with background color
void background_update_callback(Layer *layer, GContext *ctx){
  if(DEBUG)APP_LOG(APP_LOG_LEVEL_DEBUG,"Background color set");
  graphics_context_set_fill_color(ctx, bgColor);
  graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornersAll);
}

//creates bitmap layers, and adds them to a layer
void create_bitmapLayers(Layer * windowLayer){
  if(DEBUG)APP_LOG(APP_LOG_LEVEL_DEBUG,"Creating bitmap layers");
  window_layer = windowLayer;
  layer_set_update_proc(window_layer, background_update_callback);
  load_bitmaps();

  int i;
  for(i=0;i<2;i++){
    num1[i] = bitmap_layer_create(GRect(X1,Y1,NUM_WIDTH,NUM_HEIGHT));
    num2[i] = bitmap_layer_create(GRect(X2,Y2,NUM_WIDTH,NUM_HEIGHT));
    num3[i] = bitmap_layer_create(GRect(X3,Y3,NUM_WIDTH,NUM_HEIGHT));
    num4[i] = bitmap_layer_create(GRect(X4,Y4,NUM_WIDTH,NUM_HEIGHT));
    //set initial bitmaps
    
    bitmap_layer_set_bitmap(num1[i],numBitmaps[0][i]);
    bitmap_layer_set_bitmap(num2[i],numBitmaps[0][i]);
    bitmap_layer_set_bitmap(num3[i],numBitmaps[0][i]);
    bitmap_layer_set_bitmap(num4[i],numBitmaps[0][i]);
    
    bitmap_layer_set_compositing_mode(num1[i], GCompOpSet);
    bitmap_layer_set_compositing_mode(num2[i], GCompOpSet);
    bitmap_layer_set_compositing_mode(num3[i], GCompOpSet);
    bitmap_layer_set_compositing_mode(num4[i], GCompOpSet);
  }  
  margin_layer = bitmap_layer_create(layer_get_bounds(window_layer));
  bitmap_layer_set_bitmap(margin_layer,marginBitmap);
  bitmap_layer_set_compositing_mode(margin_layer, GCompOpSet);
  initialize_colors();
  for(i=0;i<2;i++){
    layer_add_child(window_layer, bitmap_layer_get_layer(num1[i]));
    layer_add_child(window_layer, bitmap_layer_get_layer(num2[i]));
    layer_add_child(window_layer, bitmap_layer_get_layer(num3[i]));
    layer_add_child(window_layer, bitmap_layer_get_layer(num4[i]));
  }   
  layer_add_child(window_layer, bitmap_layer_get_layer(margin_layer));
}

//destroys bitmap layers
void destroy_bitmapLayers(){
  #ifdef PBL_PLATFORM_APLITE
  destroy_mask_layers();
  #endif
  int i;
  for(i=0;i<2;i++){
    bitmap_layer_destroy(num1[i]);
    bitmap_layer_destroy(num2[i]);
    bitmap_layer_destroy(num3[i]);
    bitmap_layer_destroy(num4[i]);
    num1[i]=NULL;
    num2[i]=NULL;
    num3[i]=NULL;
    num4[i]=NULL;
  }
  bitmap_layer_destroy(margin_layer);
  margin_layer = NULL;
  unload_bitmaps();
}

//Given a time string, display the correct number bitmaps
void setTime(char * timeStr){
  //load numberbitmaps
  int timeDigit,layerNum;
  for(timeDigit=0;timeDigit<4;timeDigit++){
    int val = timeStr[timeDigit]-'0';
    for(layerNum=0;layerNum<2;layerNum++){
      if(numBitmaps[val][layerNum] == NULL){
        if(DEBUG)APP_LOG(APP_LOG_LEVEL_DEBUG,"Loading number %d, layer %d, %d bytes in use",
                         val,layerNum,(int)heap_bytes_used());
        numBitmaps[val][layerNum] =
          gbitmap_create_with_resource(numResources[val][layerNum]);
        if(DEBUG)APP_LOG(APP_LOG_LEVEL_DEBUG,"Number loaded, %d bytes in use",(int)heap_bytes_used());
      }
    }
  }
  //set bitmaps
  for(layerNum=0;layerNum<2;layerNum++){
    bitmap_layer_set_bitmap(num1[layerNum],numBitmaps[timeStr[0]-'0'][layerNum]);
    bitmap_layer_set_bitmap(num2[layerNum],numBitmaps[timeStr[1]-'0'][layerNum]);
    bitmap_layer_set_bitmap(num3[layerNum],numBitmaps[timeStr[2]-'0'][layerNum]);
    bitmap_layer_set_bitmap(num4[layerNum],numBitmaps[timeStr[3]-'0'][layerNum]);
    //apply color settings to newly loaded bitmaps
    apply_color(numColors[layerNum],num1[layerNum]);
    apply_color(numColors[layerNum],num2[layerNum]);
    apply_color(numColors[layerNum],num3[layerNum]);
    apply_color(numColors[layerNum],num4[layerNum]);
  }
  //unload unused number bitmaps
  int i;
  for(i=0;i<10;i++){
    if((i != (timeStr[0]-'0'))&&
       (i != (timeStr[1]-'0'))&&
       (i != (timeStr[2]-'0'))&&
       (i != (timeStr[3]-'0'))){
      if(numBitmaps[i][0] != NULL){
        if(DEBUG)APP_LOG(APP_LOG_LEVEL_DEBUG,"Unloading unused number %d, layer 0, %d bytes in use",i,(int)heap_bytes_used());
        gbitmap_destroy(numBitmaps[i][0]);
        numBitmaps[i][0] = NULL;
        if(DEBUG)APP_LOG(APP_LOG_LEVEL_DEBUG,"Number unloaded, %d bytes in use",(int)heap_bytes_used());
      }
      if(numBitmaps[i][1] != NULL){
        if(DEBUG)APP_LOG(APP_LOG_LEVEL_DEBUG,"Unloading unused number %d,%d bytes in use",i,(int)heap_bytes_used());
        gbitmap_destroy(numBitmaps[i][1]);
        numBitmaps[i][1] = NULL;
        if(DEBUG)APP_LOG(APP_LOG_LEVEL_DEBUG,"Number unloaded, %d bytes in use",(int)heap_bytes_used());
      }
    }
  }
}


