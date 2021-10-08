#include "Picture.h"

  bool init_picture_from_file(struct picture *pic, const char *path){
    pic->img = load_image(path);
    if( pic->img.data == 0 ){
      return false;
    }    
    pic->width = get_image_width(pic->img);
    pic->height = get_image_height(pic->img);
    return true;
  }

  bool init_picture_from_size(struct picture *pic, int width, int height){
    pic->img = create_image(width, height);
    pic->width = width;
    pic->height = height;
    return true;
  }

  bool save_picture_to_file(struct picture *pic, const char *path){
    return save_image(pic->img, path);   
  }

  // enum mapping to support get/set pixel functions
  enum RGB {RED, GREEN, BLUE};

  struct pixel get_pixel(struct picture *pic, int x, int y){
    // Beware: pixels are stored in a (x,y) vector from the top left of the image.
    struct pixel pix;
    
    pix.red = get_pixel_value(pic->img, RED, x, y);
    pix.green = get_pixel_value(pic->img, GREEN, x, y);
    pix.blue = get_pixel_value(pic->img, BLUE, x, y);
    
    return pix;
  }

  void set_pixel(struct picture *pic, int x, int y, struct pixel *rgb){
    // Beware: pixels are stored in a (x,y) vector from the top left of the image.
    set_pixel_value(pic->img, RED, x, y, rgb->red);
    set_pixel_value(pic->img, GREEN, x, y, rgb->green);
    set_pixel_value(pic->img, BLUE, x, y, rgb->blue);
  }

  bool contains_point(struct picture *pic, int x, int y){
      return x >= 0 && x < pic->width && y >= 0 && y < pic->height;
  }
  
  void clear_picture(struct picture *pic){
    free_image(pic->img); 
  }  
