EXEC=run
srcdir=

SHELL=/bin/sh
CC=gcc -pipe -mtune=native -march=native
OFLAGS=-O2
CFLAGS+=-std=gnu90 -Wall
LDFLAGS=-lc
CDEBUG=-g -p
DFLAGS=$(CFLAGS) -M

ifdef srcdir
VPATH=$(srcdir)
SRCS=$(wildcard $(srcdir)/*.c)
HDRS=$(wildcard $(srcdir)/*.h)
CFLAGS+=-I. -I$(srcdir)
else
SRCS=$(wildcard *.c)
HDRS=$(wildcard *.h)
endif
OBJS=$(SRCS:.c=.o)
DEPS=$(SRCS:.c=.d)
ASMS=$(SRCS:.c=.s)

ifeq ($(MAKECMDGOALS), real_debug)
CFLAGS+=$(CDEBUG)
else
CFLAGS+=$(OFLAGS)
endif

ifneq ($(MAKECMDGOALS), clean)
-include $(DEPS)
endif

.DEFAULT_GOAL=all
.PHONY: all
all: $(DEPS) $(EXEC)
	@echo done

.PHONY: clean
clean:
	-rm -f $(OBJS) $(ASMS) $(DEPS) $(HDRS:.h=.h.gch) $(EXEC) *.out
	@echo done

.PHONY: debug
debug:
	make clean
	make real_debug
	@echo done

.PHONY: real_debug
real_debug: $(DEPS) $(EXEC)

.PHONY: asm
asm: $(DEPS) $(ASMS)
	@echo done

.PHONY: depend
depend: $(DEPS)
	@echo done

.PHONY: headers
headers: $(HDRS:.h=.h.gch)
	@echo done

.PHONY: dox
dox: Doxyfile
	doxygen Doxyfile

$(EXEC): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

%.s: %.c
	$(CC) -S -g $(CFLAGS) -o $@ $<

%.d: %.c
	$(CC) $(DFLAGS) $< >$*.d

%.h.gch: %.h
	$(CC) -c $(CFLAGS) -o $@ $<

Doxyfile:
	doxygen -g
