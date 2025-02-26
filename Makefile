CC=gcc
CFLAGS=-Wall -Wextra
LDFLAGS=-lm -llapack -lblas -llapacke
SRC=main.c memory_management.c i_o.c helper_functions.c

ifeq ($(OS),Windows_NT)
	LDFLAGS+=-L/mingw64/lib
	CFLAGS+=-I/mingw64/include
	RM=del /Q
	OUT=image_compressor.exe
else
	RM=rm -f
	OUT=image_compressor
endif

build:
	$(CC) $(CFLAGS) -o $(OUT) $(SRC) $(LDFLAGS)

clean:
	$(RM) *.o $(OUT)

.PHONY: clean