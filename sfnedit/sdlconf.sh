#!/bin/sh

[ ! -f src/SDL.c ] && exit

./configure --enable-static --disable-shared --enable-assertions=release --enable-video --disable-render \
--disable-audio --disable-joystick --disable-haptic --disable-sensor --disable-power --disable-filesystem \
--disable-threads --disable-file --disable-jack --disable-diskaudio --disable-dummyaudio --disable-libsamplerate \
--disable-video-opengl --disable-video-opengles --disable-video-opengles1 --disable-video-opengles2 --disable-video-vulkan \
--disable-pthreads --disable-directx --disable-wasapi --disable-sdl-dlopen --disable-dbus --disable-ime --disable-ibus \
--disable-oss --disable-alsa --disable-pulseaudio --disable-esd --disable-arts --disable-nas --disable-sndio \
--disable-fusionsound --disable-libudev --disable-video-x11-xdbe --disable-video-x11-xinerama --disable-video-x11-xrandr \
--disable-video-x11-xinput --disable-video-x11-scrnsaver --disable-video-x11-xshape --disable-video-x11-vm \
--disable-video-vivante --disable-video-directfb --disable-video-dummy --disable-video-kmsdrm --disable-hidapi

# we must overwrite a header file in the source, there's no other way to compile a static SDL library
cat >src/dynapi/SDL_dynapi.h <<EOF
#ifndef SDL_dynapi_h_
#define SDL_dynapi_h_
#define SDL_DYNAMIC_API 0
#endif
EOF

# Makefile header, common part. Configure generated one that includes everything, even disabled subsystems...
cat >Makefile <<EOF
# Makefile to build the SDL library

INCLUDE = -I./include
CFLAGS  = -O2 \$(INCLUDE)
AR	= ar
RANLIB	= ranlib

TARGET  = libSDL.a
SOURCES = \\
	src/*.c \\
	src/atomic/*.c \\
	src/cpuinfo/*.c \\
	src/events/*.c \\
	src/file/*.c \\
	src/stdlib/*.c \\
	src/thread/*.c \\
	src/thread/generic/*.c \\
	src/timer/*.c \\
	src/render/*.c \\
	src/video/*.c \\
	src/video/yuv2rgb/*.c \\
EOF

# check in the newly generated SDL_config.h, which drivers we have and only include those
[ "`grep 'define SDL_VIDEO_DRIVER_X11 1' include/SDL_config.h`" != "" ] && cat >>Makefile <<EOF
	src/core/unix/*.c \\
	src/timer/unix/*.c \\
	src/video/x11/*.c \\
EOF

[ "`grep 'define SDL_VIDEO_DRIVER_WAYLAND 1' include/SDL_config.h`" != "" ] && cat >>Makefile <<EOF
	src/core/unix/*.c \\
	src/timer/unix/*.c \\
	src/video/wayland/*.c \\
EOF

[ "`grep 'define SDL_VIDEO_DRIVER_WINDOWS 1' include/SDL_config.h`" != "" ] && cat >>Makefile <<EOF
	src/core/windows/*.c \\
	src/loadso/windows/*.c \\
	src/timer/windows/*.c \\
	src/video/windows/*.c \\
EOF

[ "`grep 'define SDL_VIDEO_DRIVER_COCOA 1' include/SDL_config.h`" != "" ] && cat >>Makefile <<EOF
	src/core/unix/*.c \\
	src/timer/unix/*.c \\
	src/file/cocoa/*.m \\
	src/video/cocoa/*.m \\
EOF

# Makefile footer, common part
cat >>Makefile <<EOF

OBJECTS = \$(shell echo \$(SOURCES) | sed -e 's,\\.c,\\.o,g' | sed -e 's,\\.m,\\.o,g')

all: \$(TARGET)

\$(TARGET): \$(OBJECTS)
	\$(AR) crv \$@ \$^
	\$(RANLIB) \$@

clean:
	rm -f \$(TARGET) \$(OBJECTS)
EOF
