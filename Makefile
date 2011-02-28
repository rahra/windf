CC=gcc
LDFLAGS=-lm -lgd
CFLAGS=-Wall

windf_test: windf_test.o windf.o

windf_test.o: windf_test.c

windf.o: windf.c

clean:
	rm -f windf_test *.o

.PHONY: clean

