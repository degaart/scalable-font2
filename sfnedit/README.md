Scalable Screen Font 2.0 Editor
===============================

This is a GUI font editor. For further details, read the [documentation](https://gitlab.com/bztsrc/scalable-font2/blob/master/docs/sfnedit.md).

Compilation
-----------

Naturally depends on `libsfn`. Compile that first, then to compile sfnedit, use
```sh
$ make
```

If despite SDL2 is detected, but you want to use X11, then
```sh
$ USE_X11=yes make
```

Dependencies
------------

For portability, it uses SDL2 (or X11 as fallback on platforms that has X11). By default, the installed library is detected
and sfnedit is linked against that dynamically. Experts might compile and link SDL statically info sfnedit, see below.

### Under Linux, BSD and all other POSIX systems

Standard GNU toolchain, "gcc" and "make".

You'll need the development version of SDL2, which can be installed by your distro's standard package management software
("apt-get install", "emerge", "pacman", "port", "brew", "pkg" etc.). It is usually called "libsdl-dev" or something similar.

### Under MacOSX

1. in a Terminal, run `xcode-select --install` and in the pop-up window click on "Install", this will give you "gcc" and "make" under MacOSX
2. download [SDL2-X.dmg](http://libsdl.org/download-2.0.php) under the section Development Libraries
3. open it with Finder, and drag'n'drop SDL2.framework to "/Library/Frameworks" (Finder might ask for your password)

### Under Windows

1. install [MinGW](https://osdn.net/projects/mingw/releases), this will give you "gcc" and "make" under Windows
2. download [SDL2-devel-X-mingw.tar.gz](http://libsdl.org/download-2.0.php) under the section Development Libraries
3. extract SDL2 into a directory under MinGW's home directory
4. open Makefile in Notepad, and edit MINGWSDL to the path where you've extracted the tarball, add the last SDL2-X part too

### Linking minimalistic SDL statically

On all platforms, you can also statically link SDL with sfnedit. For that, here are the required steps:

1. download [SDL2 source](http://libsdl.org/download-2.0.php)
2. unpack source repo to a directory
3. copy sdlconf.sh into that directory and in a terminal, run it (under Windows in MSYS terminal)
4. run `make` there, should create a (relatively) small libSDL.a with only the SDL_video subsystem
5. here, in this sfnedit folder, open the Makefile and depending on your OS, set LINUXSDL, MINGWSDL or MACOSXSDL to that directory
6. running sfnedit's `make` here will use that static version

Porting to other OSes
---------------------

This is pretty straight forward. You'll need to compile libsfn. As it contains all its dependencies, all you need for that
is an ANSI C compiler and a minimal libc. For sfnedit, you have three options (in descending order of complexity):
1. if you have already ported X11, then simply compile with `USE_X11=yes make` (porting X11 *is* hard).
2. if you have ported SDL, then simply link statically with that ported library (porting SDL is not easy, but not particularly hard either).
3. create a driver for your OS' native user interface (the simplest choice).

As for the latter, the required steps:
1. copy ui_dummy.c to ui_X.c, and implement the functions in the copy (not much, only 11 straightforward functions)
2. edit Makefile, and modify "DUMMY driver" block to use your ui_X.c and to include your OS header and ui library.

The code is written in a way that the user interface is completely separated, so all you need to do is provide wrappers
in your ui_X.c for manipulationg windows and getting events. Your windows should have a 32 bit ARGB pixel buffer. That's all.
