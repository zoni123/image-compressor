/* (c) 2025 Sebastian-Marian Badea - MIT License */
#ifndef HELPER_FUNCTIONS_H
#define HELPER_FUNCTIONS_H
#include "definitions.h"

void svd(double *matrix, int height, int width, double *u, double *s, double *vt);

void downsample(image_metadata_t *rgb_mtd, pixel_t **rgb_matrix, pixel_t **compressed_matrix, int compression_level);

#endif