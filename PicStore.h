#ifndef PICSTORE_H
#define PICSTORE_H

#include "Picture.h"
#include "Utils.h"

struct pic_store {
  // TODO: implement this struct with a picture container
};

// picture library initialisation 
void init_picstore(struct pic_store *pstore);

// command-line interpreter routines
void print_picstore(struct pic_store *pstore);
void load_picture(struct pic_store *pstore, const char *path, const char *filename);
void unload_picture(struct pic_store *pstore, const char *filename);
void save_picture(struct pic_store *pstore, const char *filename, const char *path);

#endif

