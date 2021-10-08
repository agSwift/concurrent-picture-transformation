#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "Utils.h"
#include "Picture.h"
#include "PicProcess.h"

  // list of all possible picture transformations
  static char *cmd_strings[] = { 
    "invert",  
    "grayscale", 
    "rotate",
    "flip",
    "blur"
  };

// -------------- picture transformation function wrappers -------------- \\

  void invert_picture_wrapper(struct picture *pic, const char *unused){
    printf("calling invert\n");
    invert_picture(pic);
  }

  void grayscale_picture_wrapper(struct picture *pic, const char *unused){
    printf("calling grayscale\n");
    grayscale_picture(pic);
  }

  void rotate_picture_wrapper(struct picture *pic, const char *extra_arg){
    int angle = atoi(extra_arg);
    printf("calling rotate (%i)\n", angle);
    rotate_picture(pic, angle);
  }

  void flip_picture_wrapper(struct picture *pic, const char *extra_arg){
    char plane = extra_arg[0];
    printf("calling flip (%c)\n", plane);
    flip_picture(pic, plane);
  }

  void blur_picture_wrapper(struct picture *pic, const char *unused){
    printf("calling blur\n");
    blur_picture(pic);
  }

// ------------------------------------------------------------------------ \\

  // function pointer look-up table for picture transformation functions
  static void (* const cmds[])(struct picture *, const char *) = { 
    invert_picture_wrapper,
    grayscale_picture_wrapper,
    rotate_picture_wrapper,
    flip_picture_wrapper,
    blur_picture_wrapper
  };

  // size of look-up table (for safe IO error reporting)
  static int no_of_cmds = sizeof(cmds) / sizeof(cmds[0]);


// ---------- MAIN PROGRAM ---------- \\

  int main(int argc, char **argv){

    printf("Running the C Picture Processor... \n");

    // capture and check command line arguments
    const char * filename = argv[1];
    const char * target_file = argv[2];
    const char * process = argv[3];
    const char * extra_arg = argv[4];
    
    if(filename == NULL || target_file == NULL || process == NULL){
      printf("[!] insufficient command line arguments provided\n");
      exit(IO_ERROR);
    }        
  
    printf("  filename  = %s\n", filename);
    printf("  target    = %s\n", target_file);
    printf("  process   = %s\n", process);
    printf("  extra arg = %s\n", extra_arg);
  
    printf("\n");
  
    // create original image object
    struct picture pic;
    if(!init_picture_from_file(&pic, filename)){
      exit(IO_ERROR);   
    }    
  
    // identify the picture transformation to run
    int cmd_no = 0;
    while(cmd_no < no_of_cmds && strcmp(process, cmd_strings[cmd_no])){
      cmd_no++;
    }
  
    // IO error check
    if(cmd_no == no_of_cmds){
      printf("[!] invalid process requested: %s is not defined\n    aborting...\n", process);  
      exit(IO_ERROR);   
    }
  
    // dispatch to appropriate picture transformation function
    cmds[cmd_no](&pic, extra_arg);

    // save resulting picture and report success
    save_picture_to_file(&pic, target_file);
    printf("-- picture processing complete --\n");
    
    clear_picture(&pic);
    return 0;
  }
