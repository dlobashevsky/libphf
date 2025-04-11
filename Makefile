CC = gcc
CFLAGS = -Wall -O3 -std=c17 -D_GNU_SOURCE

all: test

test: phf.o test_phf.o
	$(CC) $(CFLAGS) -o test phf.o test_phf.o

clean:
	rm -f *.o test
