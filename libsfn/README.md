Scalable Screen Font 2.0 Utility Library
========================================

This is a library to manipulate SSFN files as well as import other formats. For the latter it has some dependencies.
This library is only used by the converter and the editor, and NOT needed by the renderer. The library relies on the
renderer, and not the other way around. See [libsfn's API](https://gitlab.com/bztsrc/scalable-font2/blob/master/docs/libsfn.md)
for more details.

Heavily stripped down versions of the required libraries are linked statically by default. These include:

- potrace to convert bitmap fonts to scalable vector fonts
- libimagequant to quantize truecolor pixel fonts
- freetype2 to import TTF / OTF / WOFF / PS Type 1 / PS Type 42 etc.

Other dependencies are linked dynamically, and are automatically detected by the build system.

- zlib for compressing fonts on-the-fly (uncompression is working regardless)
- libm both potrace and libimagequant depends on it (mostly for sqrt() and pow())

Compilation
-----------

To generate the static library, run
```
$ make libsfn.a
```

If you rather have dynamically linked dependencies (libimagequant and freetype2), then
```
$ USE_DYNDEPS=1 make libsfn.a
```
I would recommend static dependencies under Windows, and dynamic under Linux (and other POSIX systems with
decent package management for shared libraries).

To exclude importing foreign font formats alltogether and support just SSFN and ASC (no need for libimagequant
nor freetype2), then use
```
$ USE_NOFOREIGN=1 make libsfn.a
```

Note: to avoid .dll.a linking issues under MinGW, both sfnconv and sfnedit directly links with the .o object
files of libsfn. K.I.S.S.! :-)
