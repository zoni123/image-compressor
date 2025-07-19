/* (c) 2025 Sebastian-Marian Badea - MIT License */
#include "definitions.h"
#include <stdio.h>
#include <stdlib.h>
#include <lapacke.h>
#include <cblas.h>

void svd(double *matrix, int height, int width, double *u, double *s, double *vt)
{
	int conv;
	double *superb = malloc(MIN(height, width) * sizeof(double));
	conv = LAPACKE_dgesvd(LAPACK_ROW_MAJOR, 'A', 'A', height, width, matrix,
						  width, s, u, height, vt, width, superb);
	if (conv > 0) {
		perror("Failed SVD");
		exit(FAILED_SVD);
	}
	free(superb);
}

void downsample(image_metadata_t *rgb_mtd, pixel_t **rgb_matrix, pixel_t **compressed_matrix, int compression_level)
{
	int downsample_block = compression_level + 1;
	double inv_downsample = 1.0 / (downsample_block * downsample_block);

	for (int j = 0; j < rgb_mtd->height; j += downsample_block) {
		for (int k = 0; k < rgb_mtd->width; k += downsample_block) {
			pixel_t average = {0.0, 0.0, 0.0};
			int valid_downsample = 1;
			for (int l = 0; l < downsample_block; l++) {
				for (int m = 0; m < downsample_block; m++) {
					if (j + l >= rgb_mtd->height || k + m >= rgb_mtd->width) {
						valid_downsample = 0;
						break;
					}
					average.r += rgb_matrix[j + l][k + m].r;
					average.g += rgb_matrix[j + l][k + m].g;
					average.b += rgb_matrix[j + l][k + m].b;
				}
			}

			average.r *= inv_downsample;
			average.g *= inv_downsample;
			average.b *= inv_downsample;

			average.r = (average.r < 0) ? 0 : (average.r > 255) ? 255 : average.r;
			average.g = (average.g < 0) ? 0 : (average.g > 255) ? 255 : average.g;
			average.b = (average.b < 0) ? 0 : (average.b > 255) ? 255 : average.b;

			if (valid_downsample) {
				compressed_matrix[j / downsample_block][k / downsample_block] = average;
			}
		}
	}
}