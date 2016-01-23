CC=gcc
LDFLAGS=-lm -lgd
CFLAGS=-Wall -g
DEST=windf-$(shell svnversion | tr -d 'M:')

all: windarr windp

windarr: windarr.o windf.o

windarr.o: windarr.c

windf_test: windf_test.o windf.o

windf_test.o: windf_test.c

windf.o: windf.c

windp.o: windp.c

windp: windp.o

clean:
	rm -f windarr windp windf_test *.o

dist:
	rm -rf $(DEST)
	mkdir $(DEST)
	cp windf.h windarr.c windf.c windf_test.c windp.c Makefile $(DEST)
	tar cvfj $(DEST).tbz2 $(DEST)

.PHONY: clean dist

