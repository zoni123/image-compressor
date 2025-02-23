#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <lapacke.h>
#include <cblas.h>
#include "memory_management.h"
#include "i_o.h"
#include "definitions.h"

void check_files(int argc)
{
	if (argc < 2) {
		printf("Provide at least one valid file.\n");
		exit(INVALID_FILE_NUMBER);
	} else if (argc > NUM_FILES + 1) {
		printf("Too many files provided.\n");
		exit(INVALID_FILE_NUMBER);
	}
}

void svd(double *rgb_matrix, int height, int width, double *U, double *S, double *VT) {
    int info;
    double *superb = (double *)malloc(min(height, width) * sizeof(double));
    info = LAPACKE_dgesvd(LAPACK_ROW_MAJOR, 'A', 'A', height, width, rgb_matrix,
						  width, S, U, height, VT, width, superb);
    if (info > 0) {
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

int main(int argc, char **argv)
{
	unsigned char compression_level;
	char filetype[3];
	pixel_t **rgb_matrix = NULL;
	image_metadata_t mtd;
	check_files(argc);
	FILE **files = alloc_images(argc, argv), *output = fopen("test_output.ppm", "wt");
	printf("How much compression?\n1. A little\n2. A decent amount\n3. A lot\n4. Make it unintelligible\n (Choose a number between 1 and 4)\n");
	scanf("%hhu", &compression_level);

	if (compression_level < 1 || compression_level > 4) {
		printf("Invalid compression level.\n");
		return FAILED_SVD;
	}

	for (int i = 0; i < argc - 1; i++) {
		if (strcmp(strstr(argv[i + 1], ".ppm"), ".ppm") == OK) {
			fgets(filetype, 3, files[i]);
			if (strcmp(filetype, "P3") == 0) {
				mtd = read_image(argv[i + 1], files[i], &rgb_matrix, 't');
			} else if (strcmp(filetype, "P6") == 0) {
				mtd = read_image(argv[i + 1], files[i], &rgb_matrix, 'b');
			} else {
				printf("File is not a valid ppm image.\n");
				return INVALID_TYPE;
			}
		}

		compression_level = min(mtd.height, mtd.width) / (compression_level * compression_level * 5);

        double *r = (double *)malloc(mtd.height * mtd.width * sizeof(double));
        double *g = (double *)malloc(mtd.height * mtd.width * sizeof(double));
        double *b = (double *)malloc(mtd.height * mtd.width * sizeof(double));

        for (int j = 0; j < mtd.height; j++) {
            for (int k = 0; k < mtd.width; k++) {
                r[j * mtd.width + k] = rgb_matrix[j][k].r;
                g[j * mtd.width + k] = rgb_matrix[j][k].g;
                b[j * mtd.width + k] = rgb_matrix[j][k].b;
            }
        }

        double *u_r = (double *)malloc(mtd.height * mtd.height * sizeof(double));
        double *s_r = (double *)malloc(mtd.width * sizeof(double));
        double *vt_r = (double *)malloc(mtd.width * mtd.width * sizeof(double));

        double *u_g = (double *)malloc(mtd.height * mtd.height * sizeof(double));
        double *s_g = (double *)malloc(mtd.width * sizeof(double));
        double *vt_g = (double *)malloc(mtd.width * mtd.width * sizeof(double));

        double *u_b = (double *)malloc(mtd.height * mtd.height * sizeof(double));
        double *s_b = (double *)malloc(mtd.width * sizeof(double));
        double *vt_b = (double *)malloc(mtd.width * mtd.width * sizeof(double));

        svd(r, mtd.height, mtd.width, u_r, s_r, vt_r);
        svd(g, mtd.height, mtd.width, u_g, s_g, vt_g);
        svd(b, mtd.height, mtd.width, u_b, s_b, vt_b);

		fprintf(output, "P3\n%d %d\n255\n", mtd.width / (compression_level + 1), mtd.height / (compression_level + 1));

		for (int j = 0; j < mtd.height; j++) {
			for (int k = 0; k < mtd.width; k++) {
				rgb_matrix[j][k].r = 0.0;
				rgb_matrix[j][k].g = 0.0;
				rgb_matrix[j][k].b = 0.0;
				for (int l = 0; l < compression_level; l++) {
					rgb_matrix[j][k].r += u_r[j * mtd.height + l] * s_r[l] * vt_r[l * mtd.width + k];
					rgb_matrix[j][k].g += u_g[j * mtd.height + l] * s_g[l] * vt_g[l * mtd.width + k];
					rgb_matrix[j][k].b += u_b[j * mtd.height + l] * s_b[l] * vt_b[l * mtd.width + k];
				}
				clamp(&rgb_matrix[j][k].r);
				clamp(&rgb_matrix[j][k].g);
				clamp(&rgb_matrix[j][k].b);
			}
		}

		for (int j = 0; j < mtd.height; j += compression_level + 1) {
			for (int k = 0; k < mtd.width; k += compression_level + 1) {
				pixel_t average = {0.0, 0.0, 0.0};
				int valid_downsample = 1;
				for (int l = 0; l < compression_level + 1; l++) {
					for (int m = 0; m < compression_level + 1; m++) {
						if (j + l >= mtd.height || k + m >= mtd.width) {
							valid_downsample = 0;
							break;
						}
						average.r += rgb_matrix[j + l][k + m].r;
						average.g += rgb_matrix[j + l][k + m].g;
						average.b += rgb_matrix[j + l][k + m].b;
					}
				}
				average.r /= (compression_level + 1) * (compression_level + 1);
				average.g /= (compression_level + 1) * (compression_level + 1);
				average.b /= (compression_level + 1) * (compression_level + 1);
				clamp(&average.r);
				clamp(&average.g);
				clamp(&average.b);
				if (valid_downsample) {
					fprintf(output, "%0.f %0.f %0.f ", average.r, average.g, average.b);
				}
			}
			fprintf(output, "\n");
		}

        free(r);
        free(g);
        free(b);
        free(u_r);
        free(s_r);
        free(vt_r);
        free(u_g);
        free(s_g);
        free(vt_g);
        free(u_b);
        free(s_b);
        free(vt_b);
		free_rgb_matrix(&rgb_matrix, mtd.height);
	}
	fclose(output);
	for (int i = 0; i < argc - 1; i++) {
		fclose(files[i]);
	}
	return 0;
}