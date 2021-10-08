#ifndef PICTURE_H
#define PICTURE_H

#include "Utils.h"
#include <stdbool.h>

  // The pixel struct is used to represent a pixel of an image in RGB format
  struct pixel {
    int red;
    int green;
    int blue;
  };

  // The picture struct provides a wrapper for image manipulation 
  // via the SOD library (https://sod.pixlab.io/intro.html)
  struct picture {    
    // sod representation of an image
    sod_img img;
    int width;
    int height;
  };    
      
  // initialise picture struct with image from a provided file
  bool init_picture_from_file(struct picture *pic, const char *path);

  // initialise picture struct of the specified size 
  bool init_picture_from_size(struct picture *pic, int width, int height); 

  // save picture to specified file
  bool save_picture_to_file(struct picture *pic, const char *path);

  // extract a single pixel from the image as a colour struct
  struct pixel get_pixel(struct picture *pic, int x, int y);

  // set a single pixel in the image from a colour struct
  void set_pixel(struct picture *pic, int x, int y, struct pixel *rgb);

  // check if coordinates are within bounds of the stored image
  bool contains_point(struct picture *pic, int x, int y);
  
  // clean up the underlying image representation
  void clear_picture(struct picture *pic);

#endif
