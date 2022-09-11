SRCS = $(wildcard *.c)
BINS = $(SRCS:.c=) sfntest7
CFLAGS = -ansi -pedantic -Wall -Wextra -I/usr/include/SDL2 -I../ -O3
LIBS = -lSDL2
ifneq ("$(wildcard /usr/include/zlib.h)","")
CFLAGS += -DHAS_ZLIB=1
LIBS += -lz
endif

# workaround a gcc 12 bug: it compains about _ssfn_zexpand, but the checker is just wrong.
# TODO: remove this when gcc gets fixed.
CFLAGS+=-Wno-use-after-free

all: $(BINS) size

testC++: sfntest2.c
	g++ $(CFLAGS:-ansi=-std=c++11) $< -o $@ $(LIBS)

sfntest7: sfntest7.cpp
	g++ $(CFLAGS:-ansi=-std=c++11) $< -o $@ $(LIBS)

sfntest6: sfntest6.c ../ssfn.h
	gcc $(CFLAGS) $< -o $@
	valgrind -s --leak-check=full --show-leak-kinds=all ./$@

%: %.c ../ssfn.h
	gcc $(CFLAGS) $< -o $@ $(LIBS)

size: sfntest1 sfntest2
	@./size.sh

clean:
	@rm $(BINS) sfntest7 testC++ *.o buffer.tga 2>/dev/null || true
