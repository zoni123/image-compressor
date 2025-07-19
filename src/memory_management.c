/* (c) 2025 Sebastian-Marian Badea - MIT License */
#include <stdlib.h>
#include <string.h>
#include "memory_management.h"

FILE **alloc_images(int argc, char **argv)
{
	FILE **files = malloc(sizeof(FILE *) * (argc - 1));
	if (files == NULL) {
		perror("Memory allocation failed");
		exit(MEMORY_ALLOCATION_FAILED);
	} else {
		for (int i = 0; i < argc - 1; i++) {
			files[i] = fopen(argv[i + 1], "rt");
			if (!files[i]) {
				perror("Cannot open file");
				for (int j = i - 1; j >= 0; j--) {
					fclose(files[j]);
				}
				free(files);
				exit(NO_FILE);
			} else {
				printf("Successfully opened %s.\n", argv[i + 1]);
			}
		}
	}
	return files;
}

FILE **alloc_images_w(int argc, char **argv)
{
	FILE **files = malloc(sizeof(FILE *) * (argc - 1));
	if (files == NULL) {
		perror("Memory allocation failed");
		exit(MEMORY_ALLOCATION_FAILED);
	} else {
		for (int i = 0; i < argc - 1; i++) {
			files[i] = fopen(argv[i + 1], "wt");
			if (!files[i]) {
				perror("Cannot open file");
				for (int j = i - 1; j >= 0; j--) {
					fclose(files[j]);
				}
				free(files);
				exit(NO_FILE);
			} else {
				printf("Successfully created %s.\n", argv[i + 1]);
			}
		}
	}
	return files;
}

pixel_t **alloc_rgb_matrix(short int height, short int width)
{
	pixel_t **rgb_matrix;
	rgb_matrix = malloc(height * sizeof(pixel_t *));
	if (!rgb_matrix) {
		exit(MEMORY_ALLOCATION_FAILED);
	}
	for (int i = 0; i < height; i++) {
		rgb_matrix[i] = malloc(width * sizeof(pixel_t));
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
	if (r_matrix) {
		free(r_matrix);
	}
	if (g_matrix) {
		free(g_matrix);
	}
	if (b_matrix) {
		free(b_matrix);
	}
	if (u_r) {
		free(u_r);
	}
	if (s_r) {
		free(s_r);
	}
	if (vt_r) {
		free(vt_r);
	}
	if (u_g) {
		free(u_g);
	}
	if (s_g) {
		free(s_g);
	}
	if (vt_g) {
		free(vt_g);
	}
	if (u_b) {
		free(u_b);
	}
	if (s_b) {
		free(s_b);
	}
	if (vt_b) {
		free(vt_b);
	}
	if (rgb_matrix) {
		free_rgb_matrix(&rgb_matrix, rgb_height);
	}
	if (compressed_matrix) {
		free_rgb_matrix(&compressed_matrix, compressed_height);
	}
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

void alloc_compressed(image_metadata_t compressed_mtd, double **r_matrix,
					  double **g_matrix, double **b_matrix, double **u_r,
					  double **s_r, double **vt_r, double **u_g, double **s_g,
					  double **vt_g, double **u_b, double **s_b, double **vt_b)
{
	*r_matrix = malloc(compressed_mtd.height * compressed_mtd.width * sizeof(double));
	*g_matrix = malloc(compressed_mtd.height * compressed_mtd.width * sizeof(double));
	*b_matrix = malloc(compressed_mtd.height * compressed_mtd.width * sizeof(double));

	*u_r = malloc(compressed_mtd.height * compressed_mtd.height * sizeof(double));
	*s_r = malloc(compressed_mtd.width * sizeof(double));
	*vt_r = malloc(compressed_mtd.width * compressed_mtd.width * sizeof(double));

	*u_g = malloc(compressed_mtd.height * compressed_mtd.height * sizeof(double));
	*s_g = malloc(compressed_mtd.width * sizeof(double));
	*vt_g = malloc(compressed_mtd.width * compressed_mtd.width * sizeof(double));

	*u_b = malloc(compressed_mtd.height * compressed_mtd.height * sizeof(double));
	*s_b = malloc(compressed_mtd.width * sizeof(double));
	*vt_b = malloc(compressed_mtd.width * compressed_mtd.width * sizeof(double));
}

void create_output_files(int argc, char **argv, char **filenames, char *extension)
{
	char *p;
	for (int i = 0; i < argc - 1; i++) {
		strcpy(filenames[i + 1], argv[i + 1]);
		if (filenames[i + 1][0] == '.' && filenames[i + 1][1] == '\\') {
			memmove(filenames[i + 1], filenames[i + 1] + 2, strlen(filenames[i + 1]) + 1);
		}
		p = strchr(filenames[i + 1], '.');
		if (p) {
			strcpy(extension, p + 1);
		} else {
			for (int j = 0; j < argc; j++) {
				free(filenames[j]);
			}
			free(filenames);
			exit(INVALID_TYPE);
		}
		p[0] = '\0';
		strcat(filenames[i + 1], "_compressed.");
		strcat(filenames[i + 1], extension);
	}
}

void check_files(int argc)
{
	if (argc < 2) {
		perror("Provide at least one valid file");
		exit(INVALID_FILE_NUMBER);
	} else if (argc > NUM_FILES + 1) {
		perror("Too many files provided");
		exit(INVALID_FILE_NUMBER);
	}
}

void check_filenames(int argc, char ***filenames)
{
	if (!(*filenames)) {
		free(*filenames);
		exit(MEMORY_ALLOCATION_FAILED);
	}

	for (int i = 0; i < argc; i++) {
		(*filenames)[i] = malloc(LINE_LEN * sizeof(char));
		if (!(*filenames)[i]) {
			for (int j = i - 1; j >= 0; j--) {
				free((*filenames)[j]);
			}
			free(*filenames);
			exit(MEMORY_ALLOCATION_FAILED);
		}
	}
}