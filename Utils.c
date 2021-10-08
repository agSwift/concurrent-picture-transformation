#include "Utils.h"
#include <unistd.h>

  #define DEFAULT_COMPRESSION_QUALITY -1
  #define FULL_COLOUR_CHANNELS 3

  sod_img create_image(int width, int height){
    return sod_make_image(width, height, FULL_COLOUR_CHANNELS);   
  }

  void free_image(sod_img img){
    sod_free_image(img);   
  }

  sod_img load_image(const char *path){
    sod_img input;
    if( access(path, F_OK) == IO_ERROR ){
      printf("[!] error reading from file %s (check it exists)\n", path);
      input.data = 0;
      return input;
    }
    input = sod_img_load_from_file(path, SOD_IMG_COLOR);  
    if(input.data == 0){
      printf("[!] unsupported image format (expecting jpeg, png or bmp)\n");
    }
    return input;
  }
    
  bool save_image(sod_img img, const char *path){
    int ret = sod_img_save_as_jpeg(img, path, DEFAULT_COMPRESSION_QUALITY);
    if(ret != SOD_OK){
      printf("[!] error saving file to %s\n", path);
      return false;
    }
    return true;
  }

  sod_img copy_image(sod_img img){
    return sod_copy_image(img);   
  }

  int get_image_width(sod_img img){
    return img.w;   
  }

  int get_image_height(sod_img img){
    return img.h;    
  } 

  int get_pixel_value(sod_img img, int rgb, int x, int y){
    float intensity = sod_img_get_pixel(img, x, y, rgb);
    int rgb_value =  intensity * MAX_PIXEL_INTENSITY;
    return rgb_value;
  }

  void set_pixel_value(sod_img img, int rgb, int x, int y, int val){
    float intensity = val / MAX_PIXEL_INTENSITY;  
    sod_img_set_pixel(img, x, y, rgb, intensity);  
  }
