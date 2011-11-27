PREFIX?=/usr/X11R6
CFLAGS?=-Os -pedantic -Wall

all:
	$(CC) $(CFLAGS) -I$(PREFIX)/include tinywm.c -L$(PREFIX)/lib -lX11 -o tinywm

clean:
	rm -f tinywm

