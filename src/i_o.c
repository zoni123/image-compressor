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
			} else if (format == 'b'){
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
			} else if (format == 'm') {
				mtd->image_format = BMP;
				mtd->top = (4 - (mtd->width * 3) % 4) % 4;
				unsigned char r, g, b;
				if (fread(&r, sizeof(unsigned char), 1, input) == 1 &&
					fread(&g, sizeof(unsigned char), 1, input) == 1 &&
					fread(&b, sizeof(unsigned char), 1, input) == 1) {
					rgb_matrix[i][j].r = (double)r;
					rgb_matrix[i][j].g = (double)g;
					rgb_matrix[i][j].b = (double)b;
					k++;
				}
				if (mtd->top != 0) {
					fseek(input, mtd->top, SEEK_CUR);
				}
			}
		}
	}

	printf("Read %d pixels.\n", k);
	if (k != mtd->height * mtd->width) {
		for (int i = k; i < mtd->height * mtd->width; i++) {
			memset(&rgb_matrix[i / mtd->width][i % mtd->width], 0, sizeof(pixel_t));
		}
	}
}

image_metadata_t read_ppm_image(char *filename, FILE **input, pixel_t ***rgb_matrix,
								char format)
{
	char buffer[LINE_LEN];
	image_metadata_t mtd;

	fscanf(*input, "%hd%hd%hd", &mtd.width, &mtd.height, &mtd.top);
	*rgb_matrix = alloc_rgb_matrix(mtd.height, mtd.width);

	if (format == 't') {
		read_rgb_matrix(*input, *rgb_matrix, &mtd, 't');
	} else if (format == 'b') {
		fclose(*input);
		*input = fopen(filename, "rb");
		for (int i = 0; i < 3; i++) {
			fgets(buffer, LINE_LEN, *input);
		}
		read_rgb_matrix(*input, *rgb_matrix, &mtd, 'b');
	}
	return mtd;
}

image_metadata_t read_bmp_image(char *filename, FILE **input, pixel_t ***rgb_matrix,
								unsigned char bmp_header[LINE_LEN])
{
	image_metadata_t mtd;
	int height, width;

	fclose(*input);
	*input = fopen(filename, "rb");

	for (int i = 0; i < 18; i++) {
		fread(bmp_header + i, sizeof(unsigned char), 1, *input);
	}

	fread(&width, sizeof(int), 1, *input);
	mtd.width = (short)width;
	fread(&height, sizeof(int), 1, *input);
	mtd.height = (short)height;

	for (int i = 18; i < bmp_header[10] - 8; i++) {
		fread(bmp_header + i, sizeof(unsigned char), 1, *input);
	}
	
	*rgb_matrix = alloc_rgb_matrix(mtd.height, mtd.width);
	read_rgb_matrix(*input, *rgb_matrix, &mtd, 'm');
	return mtd;
}

void write_bmp_header(FILE *output, unsigned char bmp_header[LINE_LEN], image_metadata_t mtd)
{
	short int zero = 0;
	int dim = mtd.height * (mtd.width * 3 + mtd.top) + bmp_header[10];

	for (int i = 0; i < 2; i++) {
		fwrite(bmp_header + i, sizeof(unsigned char), 1, output);
	}
	fwrite(&dim, sizeof(int), 1, output);
	for (int i = 6; i < 18; i++) {
		fwrite(bmp_header + i, sizeof(unsigned char), 1, output);
	}
	fwrite(&mtd.width, sizeof(short), 1, output);
	fwrite(&zero, sizeof(short int), 1, output);
	fwrite(&mtd.height, sizeof(short), 1, output);
	fwrite(&zero, sizeof(short int), 1, output);
	for (int i = 18; i < bmp_header[10] - 8; i++) {
		fwrite(bmp_header + i, sizeof(unsigned char), 1, output);
	}
}