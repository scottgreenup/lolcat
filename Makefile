
LOLCAT_SRC ?= lolcat.c
CFLAGS ?= -std=c11 -Wall

DESTDIR ?= /usr/local/bin


.PHONY: all
all: lolcat

.PHONY: lolcat
lolcat: $(LOLCAT_SRC)
	gcc $(CFLAGS) -o $@ $^

.PHONY: clean
clean:
	rm -f lolcat
