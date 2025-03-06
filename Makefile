CC=gcc
CFLAGS=-Wall -Wextra
LDFLAGS=-lm -llapack -lblas -llapacke
SRC=src
SRCS=$(wildcard $(SRC)/*.c)

ifeq ($(OS),Windows_NT)
	LDFLAGS+=-L/mingw64/lib
	CFLAGS+=-I/mingw64/include
	RM=del /Q
	OUT=image_compressor.exe
else
	RM=rm -f
	OUT=image_compressor
endif

build: $(SRCS)
	$(CC) $(CFLAGS) -o $(OUT) $(SRCS) $(LDFLAGS)

clean:
	$(RM) $(OUT)

.PHONY: clean