#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "definitions.h"
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
					rgb_matrix[i][j].r = (unsigned char)r;
					rgb_matrix[i][j].g = (unsigned char)g;
					rgb_matrix[i][j].b = (unsigned char)b;
					k++;
				}
			} else {
				if (fread(&rgb_matrix[i][j], sizeof(pixel_t), 1, input) == 1) {
					k++;
				}
			}
		}
	}
	printf("Read %d pixels.\n", k);
	if (k != mtd->height * mtd->width) {
		exit(INVALID_PIXELS);
	}
}

image_metadata_t read_image(char *filename, FILE *input, pixel_t ***rgb_matrix, char format)
{
	char buffer[LINE_LEN];
	image_metadata_t img_mtd;
	fscanf(input, "%hd%hd%hd", &img_mtd.width, &img_mtd.height, &img_mtd.top);
	*rgb_matrix = alloc_rgb_matrix(&img_mtd);
	if (format == 't') {
		read_rgb_matrix(input, *rgb_matrix, &img_mtd, 't');
	} else if (format == 'b') {
		fclose(input);
		input = fopen(filename, "rb");
		for (int i = 0; i < 3; i++) {
			fgets(buffer, LINE_LEN, input);
		}
		read_rgb_matrix(input, *rgb_matrix, &img_mtd, 'b');
	}
	return img_mtd;
}

int main(int argc, char **argv)
{
	char filetype[3];
	pixel_t **rgb_matrix = NULL;
	image_metadata_t img_mtd;
	check_files(argc);
	FILE **files = alloc_images(argc, argv);
	for (int i = 0; i < argc - 1; i++) {
		if (strcmp(strstr(argv[i + 1], ".ppm"), ".ppm") == OK) {
			fgets(filetype, 3, files[i]);
			if (strcmp(filetype, "P3") == 0) {
				img_mtd = read_image(argv[i + 1], files[i], &rgb_matrix, 't');
			} else if (strcmp(filetype, "P6") == 0) {
				img_mtd = read_image(argv[i + 1], files[i], &rgb_matrix, 'b');
			} else {
				printf("File is not a valid ppm image.\n");
				return INVALID_TYPE;
			}
		}
		free_rgb_matrix(&rgb_matrix, img_mtd.height);
	}
	for (int i = 0; i < argc - 1; i++) {
		fclose(files[i]);
	}
	return 0;
}