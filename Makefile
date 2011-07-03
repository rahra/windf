CC=gcc
LDFLAGS=-lm -lgd
CFLAGS=-Wall -g

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

.PHONY: clean

