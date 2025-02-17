#ifndef MEMORY_MANAGEMENT_H
#define MEMORY_MANAGEMENT_H
#include "definitions.h"
#include <stdio.h>

FILE **alloc_images(int argc, char **argv);

pixel_t **alloc_rgb_matrix(image_metadata_t* mtd);

void free_rgb_matrix(pixel_t ***rgb_matrix, short int height);

#endif