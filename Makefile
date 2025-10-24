build:
	gcc src/main.c -o mand.exe -g -std=c99 -Wall -Wextra -pedantic -Wmissing-prototypes -Wstrict-prototypes -Werror
gtk:
	gcc src/gtk.c `pkg-config --cflags --libs gtk+-3.0` -o gtk.exe -g -Wall -Wextra -pedantic -Wmissing-prototypes -Wstrict-prototypes
