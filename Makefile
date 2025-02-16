build:
	gcc -Wall -Wextra -o image_compressor main.c
clean:
	rm -f *.o image_compressor
.PHONY: clean