CFLAGS = -g -O2 -Wall ${DP} -DGTK_DISABLE_DEPRECATED=1 -DGDK_DISABLE_DEPRECATED=1 -DG_DISABLE_DEPRECATED=1 `pkg-config --cflags gtk+-3.0`
LDLIBS = `pkg-config --libs gtk+-3.0`
LDFLAGS = -g

all : main

main : puzzle.o
	$(CC) $(LDFLAGS) -o puzzle puzzle.o $(LDLIBS)

clean :
	rm -f puzzle puzzle.o

copy:
	scp puzzle.c eva:MIS/GUX/proj2/puzzle.c
	scp Makefile eva:MIS/GUX/proj2/Makefile
