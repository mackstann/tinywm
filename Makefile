X11=/usr/X11R6

all:
	$(CC) -I$(X11)/include -L$(X11)/lib -lX11 -o tinywm tinywm.c

clean:
	rm -f tinywm

