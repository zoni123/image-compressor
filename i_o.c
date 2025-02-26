/* (c) 2025 Sebastian-Marian Badea - MIT License */
#include "i_o.h"
#include "memory_management.h"
#include <stdlib.h>
#include <string.h>

void read_rgb_matrix(FILE *input, pixel_t **rgb_matrix, image_metadata_t *mtd, char format)
{
	int k = 0;
	for (int i = 0; i < mtd->height; i++) {
		for (int j = 0; j < mtd->width; j++) {
			if (format == 't') {
				unsigned int r, g, b;
				if (fscanf(input, "%u%u%u", &r, &g, &b) == 3) {
					rgb_matrix[i][j].r = (double)r;
					rgb_matrix[i][j].g = (double)g;
					rgb_matrix[i][j].b = (double)b;
					k++;
				}
				mtd->image_format = P3;
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
				mtd->image_format = P6;
			}
		}
	}

	printf("Read %d pixels.\n", k);
	if (k != mtd->height * mtd->width) {
		printf("Invalid number of pixels.\n");
		exit(INVALID_PIXELS);
	}
}

image_metadata_t read_ppm_image(char *filename, FILE *input, pixel_t ***rgb_matrix,
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

void create_output_files(int argc, char **argv, char **filenames, char *extension)
{
	char *p;
	for (int i = 0; i < argc - 1; i++) {
		strcpy(filenames[i + 1], argv[i + 1]);
		p = strchr(filenames[i + 1], '.');
		if (p) {
			strcpy(extension, p + 1);
		} else {
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
		printf("Provide at least one valid file.\n");
		exit(INVALID_FILE_NUMBER);
	} else if (argc > NUM_FILES + 1) {
		printf("Too many files provided.\n");
		exit(INVALID_FILE_NUMBER);
	}
}