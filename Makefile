CC = gcc
CFLAGS = -Wall -O3 -std=c17 -D_GNU_SOURCE

PROJ=phf
LIB=lib$(PROJ).a
TEST=tests/$(PROJ).test

SRC= $(wildcard *.c)
OBJS= $(SRC:.c=.o)
DFILES= $(SRC:.c=.d)
HFILES= $(wildcard *.h)

.PHONY: all doc clean dist install

all: $(DFILES) lib test

$(DFILES): $(HFILES)


%.d:	%.c
	$(CC) -MM -MG $(CFLAGS) $< > $@

lib: $(LIB)

$(LIB): $(OBJS)
	ar rcsu $@ $^

$(TEST): $(LIB)
	make -C tests

doc:
	mkdir docs
	doxygen

dist clean:
	rm -fR $(OBJS) $(DFILES) $(LIB) *.test semantic.cache* *.tmp *.tmp~ docs
	make -C tests clean


ifeq (,$(findstring $(MAKECMDGOALS),dist clean depend doc docs))
-include $(DFILES)
endif
