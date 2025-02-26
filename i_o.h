/* (c) 2025 Sebastian-Marian Badea - MIT License */
#ifndef I_O_H
#define I_O_H
#include "definitions.h"
#include <stdio.h>

void read_rgb_matrix(FILE *input, pixel_t **rgb_matrix, image_metadata_t *mtd, char format);

image_metadata_t read_ppm_image(char *filename, FILE *input, pixel_t ***rgb_matrix, char format);

void create_output_files(int argc, char **argv, char **filenames, char *extension);

void check_files(int argc);

#endif