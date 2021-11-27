SRCS = $(wildcard *.c)
BINS = $(SRCS:.c=)
CFLAGS = -ansi -pedantic -Wall -Wextra -I/usr/include/SDL2 -I../ -O3
LIBS = -lSDL2
ifneq ("$(wildcard /usr/include/zlib.h)","")
CFLAGS += -DHAS_ZLIB=1
LIBS += -lz
endif

all: $(BINS) size

testC++: sfntest2.c
	g++ $(CFLAGS:-ansi=-std=c++11) $< -o $@ $(LIBS)

sfntest6: sfntest6.c ../ssfn.h
	gcc $(CFLAGS) $< -o $@
	valgrind -s --leak-check=full --show-leak-kinds=all ./$@

%: %.c ../ssfn.h
	gcc $(CFLAGS) $< -o $@ $(LIBS)

size: sfntest1 sfntest2
	@./size.sh

clean:
	@rm $(BINS) testC++ *.o 2>/dev/null || true
