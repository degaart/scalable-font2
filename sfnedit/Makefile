TARGET = sfnedit
MINGWSDL = ../..

include ../libsfn/Makefile.env

SRCS = $(filter-out $(wildcard ui_*.c),$(wildcard *.c))

# SDL driver
ifeq ("$(USE_X11)","")
# MacOSX
ifneq ("$(wildcard /Library/Frameworks/SDL*)","")
SRCS += ui_sdl.c
CFLAGS += -D__MACOSX__=1
LIBS += -framework SDL
DRIVER = sdl
else
# Linux
ifneq ("$(wildcard /usr/local/include/SDL2/SDL.h)","")
SRCS += ui_sdl.c
CFLAGS += -I/usr/local/include/SDL2
LIBS += -L/usr/local/lib -lSDL2
DRIVER = sdl
else
ifneq ("$(wildcard /usr/include/SDL2/SDL.h)","")
SRCS += ui_sdl.c
CFLAGS += -I/usr/include/SDL2
LIBS += -lSDL2
DRIVER = sdl
else
# Windows MinGW
ifneq ("$(wildcard $(MINGWSDL)/i686-w64-mingw32/include/SDL2/SDL.h)","")
SRCS += ui_sdl.c
CFLAGS += -I$(MINGWSDL)/i686-w64-mingw32/include/SDL2
LIBDIRS = -static-libgcc -L$(MINGWSDL)/i686-w64-mingw32/lib -L$(MINGWSDL)/i686-w64-mingw32/bin
LIBS += -lSDL2
DRIVER = sdl
endif
endif
endif
endif
endif

# OS/Z driver
ifeq ("$(DRIVER)","")
ifneq ("$(wildcard ../../include/osZ/osZ.h)","")
SRCS += ui_osz.c
CFLAGS += -I../../include
LIBS += -lui
DRIVER = osz
endif
endif

# fallback X11 driver
ifeq ("$(DRIVER)","")
ifneq ("$(wildcard /opt/local/include/X11/Xlib.h)","")
SRCS += ui_x11.c
CFLAGS += -I/opt/local/include/X11
LIBS += -lX11 -L/opt/X11/lib -L/opt/local/lib
DRIVER = x11
else
ifneq ("$(wildcard /usr/local/lib/libX11*)","")
SRCS += ui_x11.c
CFLAGS += -I/usr/local/include/X11
LIBS += -L/usr/local/lib -L/usr/local/X11/lib -lX11
DRIVER = x11
else
ifneq ("$(wildcard /usr/lib/libX11*)","")
SRCS += ui_x11.c
CFLAGS += -I/usr/include/X11
LIBS += -lX11
DRIVER = x11
endif
endif
endif
endif

OBJS = $(SRCS:.c=.o)

all: configure $(TARGET)

configure:
ifeq ("$(DRIVER)","")
	@echo "No ui driver can be detected. Install libsdl-dev or libx11-dev."
	@false
endif

%: %.c icon.h lang.h
	$(CC) $(CFLAGS) $< -c $@

# include libsfn .o files directly so that we don't rely on 'ar' under mingw
$(TARGET): $(OBJS)
ifeq ($(wildcard ../libsfn/*.o),)
	@make -C ../libsfn all
endif
	$(CC) ../libsfn/*.o ../libsfn/potracelib/*.o $(EXTRA) $(OBJS) -o $(TARGET) $(LIBDIRS) $(LIBS)

clean:
	@rm $(TARGET) *.o 2>/dev/null || true
