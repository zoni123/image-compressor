/* (c) 2025 Sebastian-Marian Badea - MIT License */
#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#define NUM_FILES 32
#define LINE_LEN 2048
#define min(a, b) ((a) < (b) ? (a) : (b))

typedef enum {
	OK,
	INVALID_FILE_NUMBER,
	MEMORY_ALLOCATION_FAILED,
	NO_FILE,
	INVALID_TYPE,
	INVALID_PIXELS,
	FAILED_SVD
} error_codes_t;

typedef enum {
	P3,
	P6,
	BMP
} image_format_t;

typedef struct {
	double r, g, b;
} pixel_t;

typedef struct {
	short int height, width, top, image_format;
} image_metadata_t;

#endif