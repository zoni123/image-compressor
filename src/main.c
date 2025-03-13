/*
* Copyright (c) 2025 Sebastian-Marian Badea
*
* This software is licensed under the MIT License.
* See the LICENSE file for details.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <lapacke.h>
#include <cblas.h>
#include "memory_management.h"
#include "i_o.h"
#include "definitions.h"
#include "helper_functions.h"

/* TODO: CLEAN UP THE CODE AND ADD COMMENTS */

int main(int argc, char **argv)
{
	check_files(argc);

	unsigned char compression_level, bmp_header[LINE_LEN];
	char ppm_filetype[3], **filenames = (char **)malloc(sizeof(char *) * argc), extension[LINE_LEN];
	pixel_t **rgb_matrix = NULL, **compressed_matrix = NULL;
	image_metadata_t rgb_mtd, compressed_mtd;
	FILE **files = alloc_images(argc, argv), **outputs = NULL;

	if (!filenames) {
		exit(MEMORY_ALLOCATION_FAILED);
	}

	for (int i = 0; i < argc; i++) {
		filenames[i] = (char *)malloc(LINE_LEN * sizeof(char));
		if (!filenames[i]) {
			for (int j = i - 1; j >= 0; j--) {
				free(filenames[j]);
			}
			free(filenames);
			exit(MEMORY_ALLOCATION_FAILED);
		}
	}

	create_output_files(argc, argv, filenames, extension);
	outputs = alloc_images_w(argc, filenames);

	for (int i = 0; i < argc - 1; i++) {
		char *p;
		p = strchr(filenames[i + 1], '.');
		if (p) {
			strcpy(extension, p + 1);
		}
		if (strcmp(extension, "ppm") == OK) {
			fgets(ppm_filetype, 3, files[i]);
			if (strcmp(ppm_filetype, "P3") == 0) {
				rgb_mtd = read_ppm_image(argv[i + 1], &files[i], &rgb_matrix, 't');
			} else if (strcmp(ppm_filetype, "P6") == 0) {
				rgb_mtd = read_ppm_image(argv[i + 1], &files[i], &rgb_matrix, 'b');
			} else {
				printf("File is not a valid ppm image.\n");
				return INVALID_TYPE;
			}
			printf("How much compression for %s?\n1. A little\n2. A decent amount\n3. A lot\n4. Make it unintelligible\n (Choose a number between 1 and 4)\n", filenames[i + 1]);
			scanf("%hhu", &compression_level);
		} else if (strcmp(extension, "bmp") == OK) {
			rgb_mtd = read_bmp_image(argv[i + 1], &files[i], &rgb_matrix, bmp_header);
			printf("Compression for .bmp is broken in this version for compression levels {2, 3, 4}.\nWill automatically use compression level 1.\n");
			compression_level = 1;
		} else {
			printf("Invalid file type.\n");
			return INVALID_TYPE;
		}
		if (compression_level < 1 || compression_level > 4) {
			printf("Invalid compression level.\n");
			return FAILED_SVD;
		}

		compressed_matrix = alloc_rgb_matrix(rgb_mtd.height / (compression_level + 1), rgb_mtd.width / (compression_level + 1));
		downsample(&rgb_mtd, rgb_matrix, compressed_matrix, compression_level);

		compressed_mtd.height = rgb_mtd.height / (compression_level + 1);
		compressed_mtd.width = rgb_mtd.width / (compression_level + 1);
		compressed_mtd.top = rgb_mtd.top;

		double *r_matrix = (double *)malloc(compressed_mtd.height * compressed_mtd.width * sizeof(double));
		double *g_matrix = (double *)malloc(compressed_mtd.height * compressed_mtd.width * sizeof(double));
		double *b_matrix = (double *)malloc(compressed_mtd.height * compressed_mtd.width * sizeof(double));

		for (int j = 0; j < compressed_mtd.height; j++) {
			for (int k = 0; k < compressed_mtd.width; k++) {
				r_matrix[j * compressed_mtd.width + k] = compressed_matrix[j][k].r;
				g_matrix[j * compressed_mtd.width + k] = compressed_matrix[j][k].g;
				b_matrix[j * compressed_mtd.width + k] = compressed_matrix[j][k].b;
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

		svd(r_matrix, compressed_mtd.height, compressed_mtd.width, u_r, s_r, vt_r);
		svd(g_matrix, compressed_mtd.height, compressed_mtd.width, u_g, s_g, vt_g);
		svd(b_matrix, compressed_mtd.height, compressed_mtd.width, u_b, s_b, vt_b);

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
		printf("\n");
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
					unsigned char r_bin = (unsigned char)compressed_matrix[j][k].r,
					g_bin = (unsigned char)compressed_matrix[j][k].g,
					b_bin = (unsigned char)compressed_matrix[j][k].b;

					fwrite(&g_bin, sizeof(unsigned char), 1, outputs[i]);
					fwrite(&b_bin, sizeof(unsigned char), 1, outputs[i]);
					fwrite(&r_bin, sizeof(unsigned char), 1, outputs[i]);
				}
			}
		} else if (rgb_mtd.image_format == BMP) {
			short int zero = 0;
			int dim = compressed_mtd.height * compressed_mtd.width * 3 + bmp_header[10];
			fclose(outputs[i]);
			outputs[i] = fopen(filenames[i + 1], "wb");

			for (int j = 0; j < 2; j++) {
				fwrite(bmp_header + j, sizeof(unsigned char), 1, outputs[i]);
			}
			fwrite(&dim, sizeof(int), 1, outputs[i]);
			for (int j = 6; j < 18; j++) {
				fwrite(bmp_header + j, sizeof(unsigned char), 1, outputs[i]);
			}
			fwrite(&compressed_mtd.width, sizeof(short), 1, outputs[i]);
			fwrite(&zero, sizeof(short int), 1, outputs[i]);
			fwrite(&compressed_mtd.height, sizeof(short), 1, outputs[i]);
			fwrite(&zero, sizeof(short int), 1, outputs[i]);
			for (int j = 18; j < bmp_header[10] - 8; j++) {
				fwrite(bmp_header + j, sizeof(unsigned char), 1, outputs[i]);
			}
			for (int j = 0; j < compressed_mtd.height; j++) {
				for (int k = 0; k < compressed_mtd.width; k++) {
					unsigned char r_bin = (unsigned char)compressed_matrix[j][k].r,
					g_bin = (unsigned char)compressed_matrix[j][k].g,
					b_bin = (unsigned char)compressed_matrix[j][k].b;

					fwrite(&r_bin, sizeof(unsigned char), 1, outputs[i]);
					fwrite(&g_bin, sizeof(unsigned char), 1, outputs[i]);
					fwrite(&b_bin, sizeof(unsigned char), 1, outputs[i]);
					if (rgb_mtd.top != 0) {
						fwrite(&zero, sizeof(unsigned char), rgb_mtd.top, outputs[i]);
					}
				}
			}
		}
		wipe(r_matrix, g_matrix, b_matrix, u_r, s_r, vt_r, u_g, s_g, vt_g, u_b,
			s_b, vt_b, rgb_matrix, compressed_matrix, rgb_mtd.height, compressed_mtd.height);
	}
	close_files(files, outputs, filenames, argc);
	return 0;
}