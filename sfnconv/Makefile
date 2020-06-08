TARGET = sfnconv

include ../libsfn/Makefile.env

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

%: %.c
	$(CC) $(CFLAGS) $< -c $@

$(TARGET): $(OBJS)
ifeq ($(wildcard ../libsfn/*.o),)
	@make -C ../libsfn all
endif
	$(CC) ../libsfn/*.o ../libsfn/potracelib/*.o $(EXTRA) $(OBJS) $(LIBS) -o $(TARGET)

clean:
	@rm $(TARGET) *.o 2>/dev/null || true
