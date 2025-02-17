build:
	gcc -Wall -Wextra -o image_compressor main.c memory_management.c
clean:
	rm -f *.o image_compressor
.PHONY: clean