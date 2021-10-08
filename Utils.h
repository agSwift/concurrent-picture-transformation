#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "sod.h"

  #define IO_ERROR -1
  #define MAX_PIXEL_INTENSITY 255.0

  // Create a new instance of a sod image of the specified width 
  // and height, using the full RGB colour model.
  sod_img create_image(int width, int height);
  
  // Free the memory used by sod image provided as argument
  void free_image(sod_img img);
  
  // Create a sod image from the the image file at the specified location.
  sod_img load_image(const char *path);  
  
  // Saves the given image in the given destination.
  bool save_image(sod_img img, const char *path);
    
  // Clones the image provided as argument
  sod_img copy_image(sod_img img);
  
  // Find the width of the provided image
  int get_image_width(sod_img img);
  
  // Find the height of the provided image
  int get_image_height(sod_img img);
  
  // Find the R/G/B value of the pixel at (x,y) in the image 
  // NOTE: (rgb = 0 for red, rgb = 1 for green, rgb = 2 for blue)
  int get_pixel_value(sod_img img, int rgb, int x, int y);
  
  // Set the R/G/B pixel intensity for pixel at (x,y)
  // NOTE: (rgb = 0 for red, rgb = 1 for green, rgb = 2 for blue)
  void set_pixel_value(sod_img img, int rgb, int x, int y, int val);

#endif
