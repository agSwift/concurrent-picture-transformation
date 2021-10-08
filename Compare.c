#include <stdio.h>
#include <string.h>
#include "Utils.h"
#include "Picture.h"

  int main(int argc, char ** argv){
  
    if(argc != 3){
      printf("usage: ./picture_compare <file_path_1> <file_path_2>\n");
      return 1;
    }
  
    // capture and check command line arguments
    const char * pic1_filename = argv[1];
    const char * pic2_filename = argv[2];  
  
    printf("compare %s with %s:\n", pic1_filename, pic2_filename);
  
    // create provided image objects
    struct picture pic1;
    struct picture pic2;
    
    init_picture_from_file(&pic1, pic1_filename);
    init_picture_from_file(&pic2, pic2_filename);
    
    int width = pic1.width;
    int height = pic1.height;
  
    if(width != pic2.width || height != pic2.height){
      printf("[!] fail - pictures do not have equal dimensions\n");
      return 1;
    }
  
    // iterate over the picture pixel-by-pixel and compare RGB values
    for(int i = 0; i < width; i++){
      for(int j = 0; j < height; j++){
        struct pixel pixel1 = get_pixel(&pic1, i, j);
        struct pixel pixel2 = get_pixel(&pic2, i, j);
        
        int red_diff = pixel1.red - pixel2.red;
        int green_diff = pixel1.green - pixel2.green;
        int blue_diff = pixel1.blue - pixel2.blue;
        
        if( red_diff > 1 || red_diff < -1 || green_diff > 1 || green_diff < -1 || blue_diff > 1 || blue_diff < -1 ) {
          printf("[!] fail - pictures not equal at cell (%i,%i)\n", i ,j);
          printf("    pixel1 RGB = \t(%i,\t %i,\t %i)\n", pixel1.red, pixel1.green, pixel1.blue);
          printf("    pixel2 RGB = \t(%i,\t %i,\t %i)\n", pixel2.red, pixel2.green, pixel2.blue);
          return 1;
        }
      }
    }
  
    printf("success - pictures identical!\n");
    return 0;
  
  }
