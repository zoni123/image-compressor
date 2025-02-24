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

void svd(double *matrix, int height, int width, double *u, double *s, double *vt) {
    int info;
    double *superb = (double *)malloc(min(height, width) * sizeof(double));
    info = LAPACKE_dgesvd(LAPACK_ROW_MAJOR, 'A', 'A', height, width, matrix,
						  width, s, u, height, vt, width, superb);
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
	char filetype[3], **filenames, *p, extension[LINE_LEN];
	pixel_t **rgb_matrix = NULL, **compressed_matrix = NULL;
	image_metadata_t rgb_mtd, compressed_mtd;
	check_files(argc);
	FILE **files = alloc_images(argc, argv), **outputs = NULL;
	filenames = (char **)malloc(sizeof(char *) * argc);
	if (!filenames) {
		exit(MEMORY_ALLOCATION_FAILED);
	}
	for (int i = 0; i < argc; i++) {
		filenames[i] = (char *)malloc(LINE_LEN * sizeof(char));
		if (!filenames[i]) {
			exit(MEMORY_ALLOCATION_FAILED);
		}
	}
	for (int i = 0; i < argc - 1; i++) {
		strcpy(filenames[i + 1], argv[i + 1]);
		p = strchr(filenames[i + 1], '.');
		if (p) {
			strcpy(extension, p + 1);
		} else {
			return INVALID_TYPE;
		}
		p[0] = '\0';
		strcat(filenames[i + 1], "_compressed.");
		strcat(filenames[i + 1], extension);
	}

	outputs = alloc_images_w(argc, filenames);
	for (int i = 0; i < argc - 1; i++) {
		if (strcmp(extension, "ppm") == OK) {
			fgets(filetype, 3, files[i]);
			if (strcmp(filetype, "P3") == 0) {
				rgb_mtd = read_image(argv[i + 1], files[i], &rgb_matrix, 't');
			} else if (strcmp(filetype, "P6") == 0) {
				rgb_mtd = read_image(argv[i + 1], files[i], &rgb_matrix, 'b');
			} else {
				printf("File is not a valid ppm image.\n");
				return INVALID_TYPE;
			}
		}

		printf("How much compression for %s?\n1. A little\n2. A decent amount\n3. A lot\n4. Make it unintelligible\n (Choose a number between 1 and 4)\n", filenames[i + 1]);
		scanf("%hhu", &compression_level);

		if (compression_level < 1 || compression_level > 4) {
			printf("Invalid compression level.\n");
			return FAILED_SVD;
		}

		compressed_matrix = alloc_rgb_matrix(rgb_mtd.height / (compression_level + 1), rgb_mtd.width / (compression_level + 1));

		for (int j = 0; j < rgb_mtd.height; j += compression_level + 1) {
			for (int k = 0; k < rgb_mtd.width; k += compression_level + 1) {
				pixel_t average = {0.0, 0.0, 0.0};
				int valid_downsample = 1;
				for (int l = 0; l < compression_level + 1; l++) {
					for (int m = 0; m < compression_level + 1; m++) {
						if (j + l >= rgb_mtd.height || k + m >= rgb_mtd.width) {
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

		compressed_mtd.height = rgb_mtd.height / (compression_level + 1);
		compressed_mtd.width = rgb_mtd.width / (compression_level + 1);
		compressed_mtd.top = rgb_mtd.top;

        double *r = (double *)malloc(compressed_mtd.height * compressed_mtd.width * sizeof(double));
        double *g = (double *)malloc(compressed_mtd.height * compressed_mtd.width * sizeof(double));
        double *b = (double *)malloc(compressed_mtd.height * compressed_mtd.width * sizeof(double));

        for (int j = 0; j < compressed_mtd.height; j++) {
            for (int k = 0; k < compressed_mtd.width; k++) {
                r[j * compressed_mtd.width + k] = compressed_matrix[j][k].r;
                g[j * compressed_mtd.width + k] = compressed_matrix[j][k].g;
                b[j * compressed_mtd.width + k] = compressed_matrix[j][k].b;
            }
        }

        double *u_r = (double *)malloc(compressed_mtd.height * compressed_mtd.height * sizeof(double));
        double *s_r = (double *)malloc(compressed_mtd.width * sizeof(double));
        double *vt_r = (double *)malloc(compressed_mtd.width * compressed_mtd.width * sizeof(double));

        double *u_g = (double *)malloc(compressed_mtd.height * compressed_mtd.height * sizeof(double));
        double *s_g = (double *)malloc(compressed_mtd.width * sizeof(double));
        double *vt_g = (double *)malloc(compressed_mtd.width * compressed_mtd.width * sizeof(double));

        double *u_b = (double *)malloc(compressed_mtd.height * compressed_mtd.height * sizeof(double));
        double *s_b = (double *)malloc(compressed_mtd.width * sizeof(double));
        double *vt_b = (double *)malloc(compressed_mtd.width * compressed_mtd.width * sizeof(double));

        svd(r, compressed_mtd.height, compressed_mtd.width, u_r, s_r, vt_r);
        svd(g, compressed_mtd.height, compressed_mtd.width, u_g, s_g, vt_g);
        svd(b, compressed_mtd.height, compressed_mtd.width, u_b, s_b, vt_b);

		int selected_pixels = min(rgb_mtd.height, rgb_mtd.width) / (compression_level * compression_level * 5);

		for (int j = 0; j < compressed_mtd.height; j++) {
			for (int k = 0; k < compressed_mtd.width; k++) {
				compressed_matrix[j][k].r = 0.0;
				compressed_matrix[j][k].g = 0.0;
				compressed_matrix[j][k].b = 0.0;
				for (int l = 0; l < selected_pixels; l++) {
					compressed_matrix[j][k].r += u_r[j * compressed_mtd.height + l] * s_r[l] * vt_r[l * compressed_mtd.width + k];
					compressed_matrix[j][k].g += u_g[j * compressed_mtd.height + l] * s_g[l] * vt_g[l * compressed_mtd.width + k];
					compressed_matrix[j][k].b += u_b[j * compressed_mtd.height + l] * s_b[l] * vt_b[l * compressed_mtd.width + k];
				}
				clamp(&compressed_matrix[j][k].r);
				clamp(&compressed_matrix[j][k].g);
				clamp(&compressed_matrix[j][k].b);
			}
		}

		if (rgb_mtd.image_format == P3) {
			fprintf(outputs[i], "P3\n%d %d\n255\n", compressed_mtd.width, compressed_mtd.height);
			for (int j = 0; j < compressed_mtd.height; j++) {
				for (int k = 0; k < compressed_mtd.width; k++) {
					fprintf(outputs[i], "%0.f %0.f %0.f ", compressed_matrix[j][k].r, compressed_matrix[j][k].g, compressed_matrix[j][k].b);
				}
				fprintf(outputs[i], "\n");
			}
	
		} else if (rgb_mtd.image_format == P6) {
			fprintf(outputs[i], "P6\n%d %d\n255\n", compressed_mtd.width, compressed_mtd.height);
			fclose(outputs[i]);
			outputs[i] = fopen(filenames[i + 1], "ab");
			for (int j = 0; j < compressed_mtd.height; j++) {
				for (int k = 0; k < compressed_mtd.width; k++) {
					unsigned char r_bin = (unsigned char)compressed_matrix[j][k].r;
					unsigned char g_bin = (unsigned char)compressed_matrix[j][k].g;
					unsigned char b_bin = (unsigned char)compressed_matrix[j][k].b;
					fwrite(&g_bin, sizeof(unsigned char), 1, outputs[i]);
					fwrite(&b_bin, sizeof(unsigned char), 1, outputs[i]);
					fwrite(&r_bin, sizeof(unsigned char), 1, outputs[i]);
				}
			}
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
		free_rgb_matrix(&rgb_matrix, rgb_mtd.height);
		free_rgb_matrix(&compressed_matrix, compressed_mtd.height);
	}
	for (int i = 0; i < argc - 1; i++) {
		fclose(files[i]);
		fclose(outputs[i]);
	}
	return 0;
}