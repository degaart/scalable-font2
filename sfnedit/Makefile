# --- set these according to your configuration ---
LINUXSDL = /usr/include/SDL2
MINGWSDL = ../..
MACSDL = /Library/Frameworks/SDL2.framework
# -------------------------------------------------

TARGET = sfnedit
include ../libsfn/Makefile.env

SRCS = $(filter-out $(wildcard ui_*.c),$(wildcard *.c))

# SDL driver
ifeq ("$(USE_X11)","")
# MacOSX
ifneq ("$(wildcard $(MACSDL))","")
SRCS += ui_sdl.c
CFLAGS += -D__MACOSX__=1 -I$(MACSDL)/Headers -I$(MACSDL)/Versions/A/Headers
LIBDIRS = -F$(MACSDL:/SDL2.framework=)
LIBS += -framework SDL2
PLAT = intel-macosx
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
LIBDIRS = -mwindows -static-libgcc -L$(MINGWSDL)/i686-w64-mingw32/lib -L$(MINGWSDL)/i686-w64-mingw32/bin
LIBS += -lSDL2
EXTRA += resource.o
PLAT = i686-win
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
PLAT = x86_64-osz
DRIVER = osz
endif
endif

# fallback X11 driver
ifeq ("$(DRIVER)","")
ifneq ("$(wildcard /opt/local/include/X11/Xlib.h)","")
SRCS += ui_x11.c
CFLAGS += -I/opt/local/include/X11
LIBS +=-L/opt/X11/lib -L/opt/local/lib -lX11
ifneq ("$(wildcard /opt/X11/lib/libXmu*)","")
CFLAGS += -DHAS_XMU=1
LIBS += -lXmu
endif
DRIVER = x11
else
ifneq ("$(wildcard /usr/local/lib/libX11*)","")
SRCS += ui_x11.c
CFLAGS += -I/usr/local/include/X11
LIBS += -L/usr/local/lib -L/usr/local/X11/lib -lX11
ifneq ("$(wildcard /usr/local/X11/lib/libXmu*)","")
CFLAGS += -DHAS_XMU=1
LIBS += -lXmu
endif
DRIVER = x11
else
ifneq ("$(wildcard /usr/lib/libX11*)","")
SRCS += ui_x11.c
CFLAGS += -I/usr/include/X11
LIBS += -lX11
ifneq ("$(wildcard /usr/lib/libX1mu*)","")
CFLAGS += -DHAS_XMU=1
LIBS += -lXmu
endif
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

resource.o: misc/resource.rc
	@windres misc/resource.rc -o resource.o

%: %.c icon.h lang.h
	$(CC) $(CFLAGS) $< -c $@

# include libsfn .o files directly so that we don't rely on 'ar' under mingw
$(TARGET): $(OBJS)
ifeq ($(wildcard ../libsfn/*.o),)
	@make -C ../libsfn all
endif
	$(CC) ../libsfn/*.o ../libsfn/potracelib/*.o $(EXTRA) $(OBJS) -o $(TARGET) $(LIBDIRS) $(LIBS)

### packaging stuff ###
zip: $(TARGET)
ifeq ("$(PLAT)","i686-win")
	@mkdir SSFN
	@cp ../ssfn.h SSFN
	@cp $(TARGET) SSFN
ifneq ("$(wildcard ../sfnconv/sfnconv)","")
	@cp ../sfnconv/sfnconv SSFN
endif
	@cp $(MINGWSDL)/i686-w64-mingw32/bin/SDL2.dll SSFN/SDL2.dll
	@rm ../ssfn_2.0.0-$(PLAT).zip 2>/dev/null || true
	zip -r -9 ../ssfn_2.0.0-$(PLAT).zip SSFN
	@rm -rf SSFN
else
ifeq ("$(PLAT)","intel-macosx")
	@mkdir SFNEdit.app SFNEdit.app/Contents SFNEdit.app/Contents/MacOS SFNEdit.app/Contents/Resources SFNEdit.app/Contents/Frameworks SFNEdit.app/Contents/Frameworks/SSFN.framework SFNEdit.app/Contents/Frameworks/SSFN.framework/Headers
	@cp $(TARGET) SFNEdit.app/Contents/MacOS
ifneq ("$(wildcard ../sfnconv/sfnconv)","")
	@cp ../sfnconv/sfnconv SFNEdit.app/Contents/MacOS
endif
	@cp ../ssfn.h SFNEdit.app/Contents/Frameworks/SSFN.framework/Headers
	@cp -r $(MACSDL) SFNEdit.app/Contents/Frameworks
	@cp misc/Info.plist SFNEdit.app/Contents
	@cp misc/sfnedit.icns SFNEdit.app/Contents/Resources
	@rm ../ssfn_2.0.0-$(PLAT).zip 2>/dev/null || true
	zip -r -9 ../ssfn_2.0.0-$(PLAT).zip SFNEdit.app
	@rm -rf SFNEdit.app
else
	@mkdir bin include
	@cp ../ssfn.h include
	@cp $(TARGET) bin
ifneq ("$(wildcard ../sfnconv/sfnconv)","")
	@cp ../sfnconv/sfnconv bin
endif
	@rm ../ssfn_2.0.0-$(PLAT).zip 2>/dev/null || true
	zip -r -9 ../ssfn_2.0.0-$(PLAT).zip bin include
	@rm -rf bin include
endif
endif

deb: $(TARGET)
	@mkdir DEBIAN usr usr/bin usr/include usr/share usr/share/applications usr/share/icons usr/share/icons/hicolor usr/share/icons/hicolor/16x16 usr/share/icons/hicolor/16x16/apps usr/share/icons/hicolor/32x32 usr/share/icons/hicolor/32x32/apps usr/share/icons/hicolor/64x64 usr/share/icons/hicolor/64x64/apps usr/share/icons/hicolor/128x128 usr/share/icons/hicolor/128x128/apps
	@cp $(TARGET) usr/bin
ifneq ("$(wildcard ../sfnconv/sfnconv)","")
	@cp ../sfnconv/sfnconv usr/bin
endif
	@cp ../ssfn.h usr/include
	@cp misc/sfnedit.desktop usr/share/applications
	@cp misc/icon128.png usr/share/icons/hicolor/128x128/apps/sfnedit.png
	@cp misc/icon64.png usr/share/icons/hicolor/64x64/apps/sfnedit.png
	@cp misc/icon32.png usr/share/icons/hicolor/32x32/apps/sfnedit.png
	@cp misc/icon16.png usr/share/icons/hicolor/16x16/apps/sfnedit.png
	@cat misc/deb_control | sed s/SIZE/`du -s usr|cut -f 1`/g >DEBIAN/control
	@md5sum `find usr -type f` >DEBIAN/md5sums
	@cp ../LICENSE DEBIAN/copyright
	@echo "2.0" >debian-binary
	@tar -czvf data.tar.gz usr
	@tar -C DEBIAN -czvf control.tar.gz control copyright md5sums
	@ar r ../ssfn_2.0.0-amd64.deb debian-binary control.tar.gz data.tar.gz
	@rm -rf debian-binary control.tar.gz data.tar.gz DEBIAN usr

clean:
	@rm $(TARGET) *.o 2>/dev/null || true

