
LOLCAT_SRC ?= lolcat.c
CFLAGS ?= -std=c11 -Wall -g

DESTDIR ?= /usr/local/bin

all: lolcat-static

.PHONY: install clean static

static: lolcat-static

lolcat-static: lolcat.c
	gcc -c $(CFLAGS) -I$(MUSLDIR)/include -o lolcat.o $<
	gcc -s -nostartfiles -nodefaultlibs -nostdinc -static -ffunction-sections -fdata-sections -Wl,--gc-sections -o $@ lolcat.o $(MUSLDIR)/lib/crt1.o $(MUSLDIR)/lib/libc.a

lolcat: $(LOLCAT_SRC)
	gcc $(CFLAGS) -o $@ $^

install: lolcat-static
	install lolcat-static $(DESTDIR)/lolcat

clean:
	rm -f lolcat lolcat-static.o lolcat-static
