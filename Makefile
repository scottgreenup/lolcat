
LOLCAT_SRC ?= lolcat.c
CFLAGS ?= -std=c11 -Wall

DESTDIR ?= /usr/local/bin

all: lolcat

.PHONY: install clean

lolcat: $(LOLCAT_SRC)
	gcc $(CFLAGS) -o $@ $^

clean:
	rm -f lolcat
