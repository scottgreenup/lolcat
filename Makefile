
LOLCAT_SRC ?= lolcat.cc
CFLAGS ?= -std=c++17 -Wall -O3

.PHONY: all
all: lolcat

.PHONY: lolcat
lolcat: $(LOLCAT_SRC)
	g++ $(CFLAGS) -o $@ $^

.PHONY: clean
clean:
	rm -f lolcat
