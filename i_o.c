#include "i_o.h"
#include "memory_management.h"
#include <stdlib.h>

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