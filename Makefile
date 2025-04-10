CC = gcc
CFLAGS = -Wall -O3 -std=c17

all: test

test: phf.o test_phf.o
	$(CC) $(CFLAGS) -o test phf.o test_phf.o

clean:
	rm -f *.o test
