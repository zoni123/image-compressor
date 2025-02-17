#ifndef DEFINITIONS_H
#define DEFINITIONS_H

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
} image_metadata_t;

#endif