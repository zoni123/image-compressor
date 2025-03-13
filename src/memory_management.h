/* (c) 2025 Sebastian-Marian Badea - MIT License */
#ifndef MEMORY_MANAGEMENT_H
#define MEMORY_MANAGEMENT_H
#include "definitions.h"
#include <stdio.h>

FILE **alloc_images(int argc, char **argv, char **filenames);

FILE **alloc_images_w(int argc, char **argv);

pixel_t **alloc_rgb_matrix(short int height, short int width);

void free_rgb_matrix(pixel_t ***rgb_matrix, short int height);

void wipe(double *r_matrix, double *g_matrix, double *b_matrix, double *u_r,
		  double *s_r, double *vt_r, double *u_g, double *s_g, double *vt_g,
		  double *u_b, double *s_b, double *vt_b, pixel_t **rgb_matrix,
		  pixel_t **compressed_matrix, short int rgb_height, short int compressed_height);

void close_files(FILE **files, FILE **outputs, char **filenames, int argc);

void alloc_compressed(image_metadata_t compressed_mtd, double **r_matrix,
					  double **g_matrix, double **b_matrix, double **u_r,
					  double **s_r, double **vt_r, double **u_g, double **s_g,
					  double **vt_g, double **u_b, double **s_b, double **vt_b);

#endif