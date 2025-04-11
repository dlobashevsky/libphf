CC=gcc

CFLAGS = -Wall -O3 -std=c17 -D_GNU_SOURCE -I.

PROJ=phf
LIB=../lib$(PROJ).a
TEST=$(PROJ).test

SRC= $(wildcard *.c)
OBJS= $(SRC:.c=.o)

.PHONY: all test clean dist

all: test

$(TEST): $(LIB)
	$(CC) $(CFLAGS) -L.. -l$(PROG) -o $@

dist clean:
	rm -fR *.o $(TEST)

