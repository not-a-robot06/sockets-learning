ifneq (, $(shell which clang))
	CC:=clang
else ifneq (, $(shell which gcc))
	CC:=gcc
else ifneq (, $(shell which cc))
	CC:=cc
else
	$(error "No C compiler found (!)")
endif

CFLAGS:=-Wall -Wextra -Wpedantic -std=c99 -D_POSIX_C_SOURCE=200112L -g3
LDFLAGS:=
DIRS:=bin
BINS:=bin/test

all: dirs $(BINS)

bin/%: src/%.c
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@

dirs: $(DIRS)
$(DIRS):
	@mkdir -p $(DIRS)

clean:
	rm bin/*
