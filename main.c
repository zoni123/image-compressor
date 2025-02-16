#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_FILES 32
#define LINE_LEN 2048

enum {
	OK,
	INVALID_FILE_NUMBER,
	MEMORY_ALLOCATION_FAILED,
	NO_FILE,
	INVALID_TYPE,
	INVALID_PIXELS
} error_codes_t;

typedef struct {
	unsigned char r, g, b;
} pixel_t;

typedef struct {
	short int height, width, top;
	pixel_t **rgb_matrix;
} image_metadata_t;

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

void free_rgb_matrix(pixel_t ***rgb_matrix, const short int height)
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

void read_rgb_matrix(FILE *input, pixel_t **rgb_matrix, const image_metadata_t *mtd, const char format)
{
	int k = 0;
	for (short int i = 0; i < mtd->height; i++) {
		for (short int j = 0; j < mtd->width; j++) {
			if (format == 't') {
				if (fscanf(input, "%hhu%hhu%hhu", &rgb_matrix[i][j].r, &rgb_matrix[i][j].g, &rgb_matrix[i][j].b) == 3) {
					k++;
				}
			} else {
				if (fread(&rgb_matrix[i][j], sizeof(pixel_t), 1, input) == 1) {
					k++;
				}
			}
		}
	}
	if (k != mtd->height * mtd->width) {
		exit(INVALID_PIXELS);
	}
}

image_metadata_t read_image(char *filename, FILE *input, pixel_t ***rgb_matrix, const char format)
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
		if (rgb_matrix) {
			free_rgb_matrix(&rgb_matrix, img_mtd.height);
		}
	}
	for (int i = 0; i < argc - 1; i++) {
		fclose(files[i]);
	}
	return 0;
}