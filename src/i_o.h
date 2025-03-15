/* (c) 2025 Sebastian-Marian Badea - MIT License */
#ifndef I_O_H
#define I_O_H
#include "definitions.h"
#include <stdio.h>

void read_rgb_matrix(FILE *input, pixel_t **rgb_matrix, image_metadata_t *mtd, char format);

image_metadata_t read_ppm_image(char *filename, FILE **input, pixel_t ***rgb_matrix, char format);

image_metadata_t read_bmp_image(char *filename, FILE **input, pixel_t ***rgb_matrix, unsigned char bmp_header[LINE_LEN]);

void write_bmp_header(FILE *output, unsigned char bmp_header[LINE_LEN], image_metadata_t mtd);

#endif