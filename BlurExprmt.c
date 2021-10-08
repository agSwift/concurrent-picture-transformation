#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "PicProcess.h"
#include "Picture.h"
#include "Utils.h"

#define PIC_INPUT_ERROR (-1)
#define MALLOC_FAILURE (-1)
#define BLUR_REGION_SIZE 9

/* Represents the total number of threads that can be alive at once (can
 * be changed)  */
#define MAX_THREADS 1000

struct worker {
  pthread_t p_thread;
  struct picture *pic_initial;

  /* Top-left corner coordinates of a section */
  int top_left_x;
  int top_left_y;

  /* Height and width of a section */
  int height;
  int width;
};

struct experiment {
  char *name;
  int x_divides;
  int y_divides;
};

static void *blur_section(void *args);
static double run_exprmt(struct picture *input_pic, int x_divides,
                         int y_divides);

// ---------- MAIN PROGRAM ---------- \\

/* To run the program, issue the command below:
  ./blur_opt_exprmt <path to image file you would like to blur> */

/* A blurred result image can be found in images/blurredOutput.jpg for testing */
int main(int argc, char **argv) {
  /* Check input arguments */
  if (argc != 2) {
    return PIC_INPUT_ERROR;
  }

  /* Initialize picture struct */
  struct picture pic_initial;
  if (!init_picture_from_file(&pic_initial, argv[1])) {
    return PIC_INPUT_ERROR;
  }

  /* Excluding border pixels */
  int total_sect_width = pic_initial.width - 2;
  int total_sect_height = pic_initial.height - 2;

  /* Setup experiments */
  struct experiment experiments[] = {
      {.name = "Sequential", .x_divides = 1, .y_divides = 1},
      {.name = "Row-by-Row", .x_divides = 1, .y_divides = total_sect_height},
      {.name = "Column-by-Column",
       .x_divides = total_sect_width,
       .y_divides = 1},
      {.name = "Sectors-2-by-2", .x_divides = 2, .y_divides = 2},
      {.name = "Sectors-4-by-4", .x_divides = 4, .y_divides = 4},
      {.name = "Sectors-8-by-8", .x_divides = 8, .y_divides = 8},
      {.name = "Sectors-16-by-16", .x_divides = 16, .y_divides = 16},
      {.name = "Sectors-32-by-32", .x_divides = 32, .y_divides = 32},
      {.name = "Sectors-64-by-64", .x_divides = 64, .y_divides = 64},
      {.name = "Pixel-by-Pixel",
       .x_divides = total_sect_width,
       .y_divides = total_sect_height}};

  int total_exprmts = sizeof(experiments) / sizeof(*experiments);

  /* Run experiments */
  for (int i = 0; i < total_exprmts; i++) {
    struct experiment *exprmt = &experiments[i];

    double time_elapsed =
        run_exprmt(&pic_initial, exprmt->x_divides, exprmt->y_divides);
    printf("Experiment: %s, Time: %f ms\n", exprmt->name, time_elapsed);
  }

  clear_picture(&pic_initial);
}

/* Divide picture into a section grid without border pixels, with x_divides
 * vertically and y_divides horizontally, then run the experiment */
static double run_exprmt(struct picture *pic_initial, int x_divides,
                         int y_divides) {
  /* Store a temporary copy of the initial input picture */
  struct picture pic_temp;
  pic_temp.img = copy_image(pic_initial->img);
  pic_temp.width = pic_initial->width;
  pic_temp.height = pic_initial->height;

  /* Initialize workers */
  struct worker **workers = malloc(sizeof(struct worker *) * x_divides);
  if (workers == NULL) {
    exit(MALLOC_FAILURE);
  }

  workers[0] = malloc(sizeof(struct worker) * y_divides * x_divides);
  if (workers[0] == NULL) {
    exit(MALLOC_FAILURE);
  }

  for (int i = 1; i < x_divides; i++) {
    workers[i] = workers[i - 1] + y_divides;
  }

  int section_width = (pic_temp.width - 2) / x_divides;
  int section_height = (pic_temp.height - 2) / y_divides;

  /* Initialise worker arguments */
  for (int x = 0; x < x_divides; x++) {
    for (int y = 0; y < y_divides; y++) {
      workers[x][y].pic_initial = &pic_temp;

      /* Check if pixels are at boundary */
      if (x == x_divides - 1) {
        workers[x][y].width =
            (pic_temp.width - 2) - ((x_divides - 1) * section_width);
      }
      if (y == y_divides - 1) {
        workers[x][y].height =
            (pic_temp.width - 2) - ((y_divides - 1) * section_width);
      }

      workers[x][y].width = section_width;
      workers[x][y].height = section_height;
      workers[x][y].top_left_x = 1 + section_width * x;
      workers[x][y].top_left_y = 1 + section_height * y;
    }
  }

  /* Time clock info:
   * https://stackoverflow.com/questions/42046712/how-to-record-elaspsed-wall-time-in-c
   */

  /**** START CLOCK ****/
  /*--------------------------------------*/
  struct timespec start;
  clock_gettime(CLOCK_MONOTONIC, &start);

  /* Start p_threads */
  pthread_t *p_threads = malloc(sizeof(pthread_t) * MAX_THREADS);
  int counter = 0;
  int setup_threads = 0;

  for (int x = 0; x < x_divides; x++) {
    for (int y = 0; y < y_divides; y++) {
      /* All threads currently in use - wait for the first thread started to
       * finish
       */
      if (setup_threads >= MAX_THREADS) {
        pthread_join(p_threads[counter], NULL);
      }

      pthread_create(&p_threads[counter], NULL, blur_section, &workers[x][y]);

      setup_threads++;
      counter++;

      /* Range: 0 <= Counter < MAX_THREADS */
      if (counter >= MAX_THREADS) {
        counter -= MAX_THREADS;
      };
    }
  }

  /* Wait for remaining threads still working to finish */
  int working_threads =
      MAX_THREADS < setup_threads ? MAX_THREADS : setup_threads;
  for (int i = 0; i < working_threads; i++) {
    pthread_join(p_threads[i], NULL);
  }

  /*--------------------------------------*/
  /**** STOP CLOCK ****/
  struct timespec end;
  clock_gettime(CLOCK_MONOTONIC, &end);

  /* Time in milliseconds */
  double time_spent =
      ((end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec)) / 1e6;

  /* Only save one blurred picture result to test code, as the
   * run_exprmt function works for all types of experiments */
  save_picture_to_file(&pic_temp, "images/blurredOutput.jpg");

  /* Free memory and clean-up temporary copy picture */
  free(workers[0]);
  free(workers);
  clear_picture(&pic_temp);

  return time_spent;
}

/* Blurs a specific section of an image */
static void *blur_section(void *args) {
  struct worker *worker = (struct worker *)args;
  struct picture *pic = worker->pic_initial;

  /* Temporary copy of input picture */
  struct picture tmp_pic;
  tmp_pic.img = copy_image(pic->img);
  tmp_pic.width = pic->width;
  tmp_pic.height = pic->height;

  /* Top-left corner coordinates of the section */
  int top_left_x = worker->top_left_x;
  int top_left_y = worker->top_left_y;

  /* Blur pixels */
  for (int i = top_left_x; i < top_left_x + worker->width; i++) {
    for (int j = top_left_y; j < top_left_y + worker->height; j++) {
      struct pixel rgb;
      int sum_red = 0;
      int sum_green = 0;
      int sum_blue = 0;

      for (int n = -1; n <= 1; n++) {
        for (int m = -1; m <= 1; m++) {
          rgb = get_pixel(&tmp_pic, i + n, j + m);
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
  clear_picture(&tmp_pic);
  return NULL;
}
