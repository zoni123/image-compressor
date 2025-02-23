CFLAGS=-Wall -Wextra -I/mingw64/include
LDFLAGS=-L/mingw64/lib -lm -llapack -lblas -llapacke

build:
	gcc $(CFLAGS) -o image_compressor main.c memory_management.c i_o.c $(LDFLAGS)

clean:
	rm -f *.o image_compressor

.PHONY: clean