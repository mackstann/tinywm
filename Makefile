X11=/usr/X11R6
CCOPTS=-Os -pedantic -Wall

all:
	$(CC) $(CCOPTS) -I$(X11)/include -L$(X11)/lib -lX11 -o tinywm tinywm.c

clean:
	rm -f tinywm

