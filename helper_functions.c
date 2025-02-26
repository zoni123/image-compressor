/* (c) 2025 Sebastian-Marian Badea - MIT License */
#include "definitions.h"
#include <stdio.h>
#include <stdlib.h>
#include <lapacke.h>
#include <cblas.h>

void svd(double *matrix, int height, int width, double *u, double *s, double *vt)
{
	int conv;
	double *superb = (double *)malloc(min(height, width) * sizeof(double));
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