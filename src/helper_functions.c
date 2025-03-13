/* (c) 2025 Sebastian-Marian Badea - MIT License */
#include "definitions.h"
#include <stdio.h>
#include <stdlib.h>
#include <lapacke.h>
#include <cblas.h>

void svd(double *matrix, int height, int width, double *u, double *s, double *vt)
{
	int conv;
	double *superb = (double *)malloc(MIN(height, width) * sizeof(double));
	conv = LAPACKE_dgesvd(LAPACK_ROW_MAJOR, 'A', 'A', height, width, matrix,
						  width, s, u, height, vt, width, superb);
	if (conv > 0) {
		printf("Failed SVD.\n");
		exit(FAILED_SVD);
	}
	free(superb);
}

void clamp(double *a)
{
	if (*a < 0.0) {
		*a = 0.0;
	} else if (*a > 255.0) {
		*a = 255.0;
	}
}

void downsample(image_metadata_t *rgb_mtd, pixel_t **rgb_matrix, pixel_t **compressed_matrix, int compression_level)
{
	for (int j = 0; j < rgb_mtd->height; j += compression_level + 1) {
		for (int k = 0; k < rgb_mtd->width; k += compression_level + 1) {
			pixel_t average = {0.0, 0.0, 0.0};
			int valid_downsample = 1;
			for (int l = 0; l < compression_level + 1; l++) {
				for (int m = 0; m < compression_level + 1; m++) {
					if (j + l >= rgb_mtd->height || k + m >= rgb_mtd->width) {
						valid_downsample = 0;
						break;
					}
					average.r += rgb_matrix[j + l][k + m].r;
					average.g += rgb_matrix[j + l][k + m].g;
					average.b += rgb_matrix[j + l][k + m].b;
				}
			}

			average.r /= ((compression_level + 1) * (compression_level + 1));
			average.g /= ((compression_level + 1) * (compression_level + 1));
			average.b /= ((compression_level + 1) * (compression_level + 1));

			clamp(&average.r);
			clamp(&average.g);
			clamp(&average.b);

			if (valid_downsample) {
				compressed_matrix[j / (compression_level + 1)][k / (compression_level + 1)] = average;
			}
		}
	}
}