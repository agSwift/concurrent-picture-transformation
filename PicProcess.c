#include "PicProcess.h"

  #define NO_RGB_COMPONENTS 3
  #define BLUR_REGION_SIZE 9


  void invert_picture(struct picture *pic){
    for(int i = 0 ; i < pic->width; i++){
      for(int j = 0 ; j < pic->height; j++){
        struct pixel rgb = get_pixel(pic, i, j);
        rgb.red = MAX_PIXEL_INTENSITY - rgb.red;
        rgb.green = MAX_PIXEL_INTENSITY - rgb.green;
        rgb.blue = MAX_PIXEL_INTENSITY - rgb.blue;
        set_pixel(pic, i, j, &rgb);
      }
    }   
  }

  void grayscale_picture(struct picture *pic){
    for(int i = 0 ; i < pic->width; i++){
      for(int j = 0 ; j < pic->height; j++){
        struct pixel rgb = get_pixel(pic, i, j);
        int avg = (rgb.red + rgb.green + rgb.blue) / NO_RGB_COMPONENTS;
        rgb.red = avg;
        rgb.green = avg;
        rgb.blue = avg;
        set_pixel(pic, i, j, &rgb);
      }
    }    
  }

  void rotate_picture(struct picture *pic, int angle){
    struct picture tmp;
    tmp.img = copy_image(pic->img);
    tmp.width = pic->width;
    tmp.height = pic->height; 
  
    int new_width = tmp.width;
    int new_height = tmp.height;
  
    if(angle == 90 || angle == 270){
      new_width = tmp.height;
      new_height = tmp.width;
    }
    
    clear_picture(pic);
    init_picture_from_size(pic, new_width, new_height);
  
    for(int i = 0 ; i < new_width; i++){
      for(int j = 0 ; j < new_height; j++){
        struct pixel rgb;
        switch(angle){
          case(90):
            rgb = get_pixel(&tmp, j, new_width -1 - i); 
            break;
          case(180):
            rgb = get_pixel(&tmp, new_width - 1 - i, new_height - 1 - j);
            break;
          case(270):
            rgb = get_pixel(&tmp, new_height - 1 - j, i);
            break;
          default:
            printf("[!] rotate is undefined for angle %i (must be 90, 180 or 270)\n", angle);
            clear_picture(&tmp);
            return;
        }
        set_pixel(pic, i,j, &rgb);
      }
    }
    clear_picture(&tmp);
  }

  void flip_picture(struct picture *pic, char plane){
    struct picture tmp;
    tmp.img = copy_image(pic->img);
    tmp.width = pic->width;
    tmp.height = pic->height;  
    
    if(plane == 'V'){
      printf("flipping over V plane\n");
      for(int i = 0 ; i < tmp.width; i++){
        for(int j = 0 ; j < tmp.height; j++){
          struct pixel rgb = get_pixel(&tmp, i, tmp.height - 1 - j);
          set_pixel(pic, i, j, &rgb);
        }
      }
    } else if (plane == 'H'){
      printf("flipping over H plane\n");  
      for(int i = 0 ; i < tmp.width; i++){
        for(int j = 0 ; j < tmp.height; j++){
          struct pixel rgb = get_pixel(&tmp, tmp.width - 1 - i, j);
          set_pixel(pic, i, j, &rgb);
        }
      }    
    } else {
      printf("[!] flip is undefined for plane %c\n", plane);
    }  
    clear_picture(&tmp);
  }

  void blur_picture(struct picture *pic){
    struct picture tmp;
    tmp.img = copy_image(pic->img);
    tmp.width = pic->width;
    tmp.height = pic->height;  
  
    for(int i = 1 ; i < tmp.width - 1; i++){
      for(int j = 1 ; j < tmp.height - 1; j++){
          
        struct pixel rgb;  
        int sum_red = 0;
        int sum_green = 0;
        int sum_blue = 0;
      
        for(int n = -1; n <= 1; n++){
          for(int m = -1; m <= 1; m++){
            rgb = get_pixel(&tmp, i+n, j+m);
            sum_red += rgb.red;
            sum_green += rgb.green;
            sum_blue += rgb.blue;
          }
        }
      
        rgb.red = sum_red / BLUR_REGION_SIZE;
        rgb.green = sum_green / BLUR_REGION_SIZE;
        rgb.blue = sum_blue / BLUR_REGION_SIZE;
      
        set_pixel(pic, i, j, &rgb);
      }
    }
    clear_picture(&tmp);
  }
