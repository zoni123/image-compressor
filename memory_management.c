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
	return files;
}

pixel_t **alloc_rgb_matrix(image_metadata_t* mtd)
{
	pixel_t **rgb_matrix;
	rgb_matrix = (pixel_t **)malloc(mtd->height * sizeof(pixel_t *));
	if (!rgb_matrix) {
		exit(MEMORY_ALLOCATION_FAILED);
	}
	for (short int i = 0; i < mtd->height; i++) {
		rgb_matrix[i] = (pixel_t *)malloc(mtd->width * sizeof(pixel_t));
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
		for (short int i = 0; i < height; i++) {
			if ((*rgb_matrix)[i]) {
				free((*rgb_matrix)[i]);
				(*rgb_matrix)[i] = NULL;
			}
		}
		free(*rgb_matrix);
		*rgb_matrix = NULL;
	}
}