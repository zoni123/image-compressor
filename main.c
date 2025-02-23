#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <lapacke.h>
#include <cblas.h>
#include "memory_management.h"

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

void read_rgb_matrix(FILE *input, pixel_t **rgb_matrix, image_metadata_t *mtd, char format)
{
	int k = 0;
	for (short int i = 0; i < mtd->height; i++) {
		for (short int j = 0; j < mtd->width; j++) {
			if (format == 't') {
				unsigned int r, g, b;
				if (fscanf(input, "%u%u%u", &r, &g, &b) == 3) {
					rgb_matrix[i][j].r = (double)r;
					rgb_matrix[i][j].g = (double)g;
					rgb_matrix[i][j].b = (double)b;
					k++;
				}
			} else {
				unsigned char r, g, b;
				if (fread(&r, sizeof(unsigned char), 1, input) == 1 &&
					fread(&g, sizeof(unsigned char), 1, input) == 1 &&
					fread(&b, sizeof(unsigned char), 1, input) == 1) {
					rgb_matrix[i][j].r = (double)r;
					rgb_matrix[i][j].g = (double)g;
					rgb_matrix[i][j].b = (double)b;
					k++;
				}
			}
		}
	}
	printf("Read %d pixels.\n", k);
	if (k != mtd->height * mtd->width) {
		printf("Invalid number of pixels.\n");
		exit(INVALID_PIXELS);
	}
}

image_metadata_t read_image(char *filename, FILE *input, pixel_t ***rgb_matrix,
							char format)
{
	char buffer[LINE_LEN];
	image_metadata_t mtd;
	fscanf(input, "%hd%hd%hd", &mtd.width, &mtd.height, &mtd.top);
	*rgb_matrix = alloc_rgb_matrix(mtd.height, mtd.width);
	if (format == 't') {
		read_rgb_matrix(input, *rgb_matrix, &mtd, 't');
	} else if (format == 'b') {
		fclose(input);
		input = fopen(filename, "rb");
		for (int i = 0; i < 3; i++) {
			fgets(buffer, LINE_LEN, input);
		}
		read_rgb_matrix(input, *rgb_matrix, &mtd, 'b');
	}
	return mtd;
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

		fprintf(output, "P3\n%d %d\n255\n", mtd.width, mtd.height);

		double prev_compressed_r = 0.0, prev_compressed_g = 0.0, prev_compressed_b = 0.0;
		for (int j = 0; j < mtd.height; j++) {
			for (int k = 0; k < mtd.width; k++) {
				double compressed_r = 0.0, compressed_g = 0.0, compressed_b = 0.0;
				for (int l = 0; l < compression_level; l++) {
					compressed_r += u_r[j * mtd.height + l] * s_r[l] * vt_r[l * mtd.width + k];
					compressed_g += u_g[j * mtd.height + l] * s_g[l] * vt_g[l * mtd.width + k];
					compressed_b += u_b[j * mtd.height + l] * s_b[l] * vt_b[l * mtd.width + k];
				}
				clamp(&compressed_r);
				clamp(&compressed_g);
				clamp(&compressed_b);
				compressed_r = floor(compressed_r);
				compressed_g = floor(compressed_g);
				compressed_b = floor(compressed_b);
				//if (compressed_r != prev_compressed_r || compressed_g != prev_compressed_g || compressed_b != prev_compressed_b) {
					fprintf(output, "%.0lf %.0lf %.0lf ", compressed_r, compressed_g, compressed_b);
				//}
				prev_compressed_r = compressed_r;
				prev_compressed_g = compressed_g;
				prev_compressed_b = compressed_b;
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
	for (int i = 0; i < argc - 1; i++) {
		fclose(files[i]);
	}
	return 0;
}