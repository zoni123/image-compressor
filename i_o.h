#ifndef I_O_H
#define I_O_H
#include "definitions.h"
#include <stdio.h>

void read_rgb_matrix(FILE *input, pixel_t **rgb_matrix, image_metadata_t *mtd, char format);

image_metadata_t read_image(char *filename, FILE *input, pixel_t ***rgb_matrix, char format);

#endif