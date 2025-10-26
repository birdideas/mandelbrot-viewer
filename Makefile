build:
	gcc src/main.c -o mand.exe `pkg-config --cflags --libs gtk4` -g -Wall -Wextra -pedantic -Wstrict-prototypes -Werror -O3 -lm
