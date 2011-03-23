CC=gcc
LDFLAGS=-lm -lgd
CFLAGS=-Wall

windarr: windarr.o windf.o

windarr.o: windarr.c

windf_test: windf_test.o windf.o

windf_test.o: windf_test.c

windf.o: windf.c

clean:
	rm -f windarr windf_test *.o

.PHONY: clean

