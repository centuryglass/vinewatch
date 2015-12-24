/**
*@File:image_resources.h
*Handles all functionality related
*to bitmap and color manipulation
*/
#pragma once
#include <pebble.h>

#define NUM_COLORS 3

//Copies the color string into a buffer
void get_color_string(char buffer[19]);

//save color values to persistant storage
void save_colors();

//change color values to the ones stored in the color string
void update_colors(char * colorStr);

//creates bitmap layers, and adds them to a layer
void create_bitmapLayers(Layer * window_layer);

//destroys bitmap layers
void destroy_bitmapLayers();

//Given a time string, display the correct number bitmaps
void setTime(char * timeStr);