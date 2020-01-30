all: first

first: first.c
	gcc -Wall -Werror -lm -fsanitize=address -std=c11 first.c -o first
clean:
	rm -f first
