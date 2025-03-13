/* (c) 2025 Sebastian-Marian Badea - MIT License */
#include <stdlib.h>
#include "memory_management.h"

FILE **alloc_images(int argc, char **argv)
{
	FILE **files = (FILE **)malloc(sizeof(FILE *) * (argc - 1));
	if (files == NULL) {
		printf("Memory allocation failed.\n");
		exit(MEMORY_ALLOCATION_FAILED);
	} else {
		for (int i = 0; i < argc - 1; i++) {
			files[i] = fopen(argv[i + 1], "rt");
			if (!files[i]) {
				printf("Cannot open file %s.\n", argv[i + 1]);
				for (int j = i - 1; j >= 0; j--) {
					fclose(files[i]);
				}
				free(files);
				exit(NO_FILE);
			} else {
				printf("Successfully opened %s.\n", argv[i + 1]);
			}
		}
	}
	printf("\n");
	return files;
}

FILE **alloc_images_w(int argc, char **argv)
{
	FILE **files = (FILE **)malloc(sizeof(FILE *) * (argc - 1));
	if (files == NULL) {
		printf("Memory allocation failed.\n");
		exit(MEMORY_ALLOCATION_FAILED);
	} else {
		for (int i = 0; i < argc - 1; i++) {
			files[i] = fopen(argv[i + 1], "wt");
			if (!files[i]) {
				printf("Cannot open file %s.\n", argv[i + 1]);
				for (int j = i - 1; j >= 0; j--) {
					fclose(files[i]);
				}
				free(files);
				exit(NO_FILE);
			} else {
				printf("Successfully created %s.\n", argv[i + 1]);
			}
		}
	}
	printf("\n");
	return files;
}

pixel_t **alloc_rgb_matrix(short int height, short int width)
{
	pixel_t **rgb_matrix;
	rgb_matrix = (pixel_t **)malloc(height * sizeof(pixel_t *));
	if (!rgb_matrix) {
		exit(MEMORY_ALLOCATION_FAILED);
	}
	for (int i = 0; i < height; i++) {
		rgb_matrix[i] = (pixel_t *)malloc(width * sizeof(pixel_t));
		if (!rgb_matrix[i]) {
			for (int j = i - 1; j >= 0; j--) {
				free(rgb_matrix[j]);
			}
			free(rgb_matrix);
			exit(MEMORY_ALLOCATION_FAILED);
		}
	}
	return rgb_matrix;
}

void free_rgb_matrix(pixel_t ***rgb_matrix, short int height)
{
	if (*rgb_matrix) {
		for (int i = 0; i < height; i++) {
			if ((*rgb_matrix)[i]) {
				free((*rgb_matrix)[i]);
				(*rgb_matrix)[i] = NULL;
			}
		}
		free(*rgb_matrix);
		*rgb_matrix = NULL;
	}
}

void wipe(double *r_matrix, double *g_matrix, double *b_matrix, double *u_r,
	double *s_r, double *vt_r, double *u_g, double *s_g, double *vt_g,
	double *u_b, double *s_b, double *vt_b, pixel_t **rgb_matrix,
	pixel_t **compressed_matrix, short int rgb_height, short int compressed_height)
{
	free(r_matrix);
	free(g_matrix);
	free(b_matrix);
	free(u_r);
	free(s_r);
	free(vt_r);
	free(u_g);
	free(s_g);
	free(vt_g);
	free(u_b);
	free(s_b);
	free(vt_b);
	free_rgb_matrix(&rgb_matrix, rgb_height);
	free_rgb_matrix(&compressed_matrix, compressed_height);
}

void close_files(FILE **files, FILE **outputs, char **filenames, int argc)
{
	for (int i = 0; i < argc - 1; i++) {
		if (files[i]) {
			fclose(files[i]);
			files[i] = NULL;
		}
		if (outputs[i]) {
			fclose(outputs[i]);
			outputs[i] = NULL;
		}
	}
	for (int i = 0; i < argc; i++) {
		free(filenames[i]);
	}
	free(filenames);
	free(files);
	free(outputs);
}