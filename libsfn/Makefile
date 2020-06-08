include Makefile.env

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

ifeq ($(USE_NOFOREIGN),)
ifeq ($(USE_DYNDEPS),)
DEPS = imagequant freetype
endif
endif

all: potrace $(DEPS) $(OBJS)

unicode.h:
	php unicode.php

potrace:
	@make -C potracelib all

imagequant:
	@make -C libimagequant all

freetype:
ifeq ($(wildcard freetype2/objs/*.h),)
	@make -C freetype2 setup ansi
endif
	@make -C freetype2 all

%: %.c unicode.h
	$(CC) $(CFLAGS) $< -c $@

libsfn.a: $(OBJS)
	@rm $@ 2>/dev/null || true
	@ar -frsv potracelib/*.o $(EXTRA) $@ $^

clean:
	@rm *.a *.txt *.o 2>/dev/null || true

distclean: clean
	@make -C potracelib clean || true
	@make -C libimagequant clean || true
	@make -C freetype2 distclean || true
