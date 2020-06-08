Scalable Screen Font 2.0
========================

This is a portable, single ANSI C/C++ header file, a scalable bitmap and vector font renderer. It has only memory
related libc dependency and it does not use floating point numbers. It's extremely small (ca. 28 kilobytes) and it is
very easy on memory, perfect for embedded systems and hobby OS kernels. It was a hobby project for me, so donations
and contributions would be much appreciated if it turns out to be useful to you.

 - [ssfn.h](https://gitlab.com/bztsrc/scalable-font2/blob/master/ssfn.h) the SSFN renderer itself
 - [sfnconv](https://gitlab.com/bztsrc/scalable-font2/tree/master/sfnconv) a command line SSFN converter tool
 - [sfnedit](https://gitlab.com/bztsrc/scalable-font2/tree/master/sfnedit) multiplatform SSFN font converter and editor with a GUI (WiP)
 - [sfntest](https://gitlab.com/bztsrc/scalable-font2/tree/master/sfntest) test applications and [API](https://gitlab.com/bztsrc/scalable-font2/blob/master/docs/API.md) usage examples

<img alt="Scalable Screen Font Features" src="https://gitlab.com/bztsrc/scalable-font2/raw/master/features.png">

SSFN renderer does not use existing font formats directly (because most formats are inefficient or just insane),
so you first have to compress those into [SSFN](https://gitlab.com/bztsrc/scalable-font2/blob/master/docs/sfnconv.md).
There's a small ANSI C utility and also a GUI editor to do that. They support importing

 - OpenType (.otf, .ttf, .sfnt, .cff),
 - TrueType (.ttf),
 - PS Type1 (.pfa, .pfb),
 - PS Type42 (.pfa, .pfb),
 - Webfonts (.woff, .woff2),
 - FontForge's SplineFontDB (.sfd, both vector and bitmap fonts),
 - X11 Bitmap Distribution Format (.bdf),
 - X11 Portable Compiled Font (.pcf),
 - Linux Console fonts (.psf, .psfu),
 - Windows Console Fonts (.fon, .fnt),
 - GNU unifont (.hex),
 - Portable Network Graphics and TARGA (.png, .tga),
 - ...and others! Vectorizing bitmap fonts and rasterizing vector fonts also possible.

Using SSFN means your fonts will require less space, and also the renderer can work a lot faster than other renderer
libraries. Check out [comparition](https://gitlab.com/bztsrc/scalable-font2/blob/master/docs/compare.md) with other font
formats (measurements for the feature demo above can be found at the bottom of that page).

Example Code
------------

The SSFN renderer comes in two flavours: there's the normal renderer with a few functions and libc dependency, and a
specialized renderer for OS kernel consoles with just one function and no dependencies at all. (Just for the records,
with a single define you can configure the normal render for dependency-free static memory management too, although
that mode has certain limitiations.)

### Simple Renderer

The header contains everything, no additional linking required!

```c
#define SSFN_CONSOLEBITMAP_HICOLOR          /* use the special renderer for hicolor packed pixels */
#include <ssfn.h>

/* set up context by global variables */
ssfn_src = &_binary_console_sfn_start;      /* the bitmap font to use */

ssfn_dst.ptr = 0xE0000000;                  /* address of the linear frame buffer */
ssfn_dst.w = 1024;                          /* width */
ssfn_dst.h = 768;                           /* height */
ssfn_dst.p = 2048;                          /* bytes per line */
ssfn_dst.x = ssfn_dst.y = 0;                /* pen position */
ssfn_dst.fg = 0xFFFF;                       /* foreground color */

/* render one glyph for UNICODE code point 'A', directly to the screen and then adjust pen position */
ssfn_putc(0x41);
```

As you see this renderer implementation is very simple, extremely small (less than 2k). It can only render
unscaled bitmap fonts. It does not allocate memory nor need libc, so it can't scale, but it can handle
proportional fonts (like 8x16 for Latin letters, and 16x16 for CJK ideograms). Therefore you can implement
a true UNICODE console with this renderer.

IMPORTANT NOTE: unlike the normal renderer, this one does not handle gzip compressed fonts. Always pass an
inflated font in `ssfn_src`.

### Normal Renderer

Very easy to use, here's an example without error handling:

```c
#define SSFN_IMPLEMENTATION                         /* use the normal renderer implementation */
#include <ssfn.h>

ssfn_t ctx = { 0 };                                 /* the renderer context */
ssfn_buf_t buf = {                                  /* the destination pixel buffer */
    .ptr = sdlsurface->pixels,                      /* address of the buffer */
    .w = sdlsurface->w,                             /* width */
    .h = sdlsurface->h,                             /* height */
    .p = sdlsurface->pitch,                         /* bytes per line */
    .x = 100,                                       /* pen position */
    .y = 100,
    .fg = 0xFF808080                                /* foreground color */
};

/* add one or more fonts to the context. Fonts must be already in memory */
ssfn_load(&ctx, &_binary_times_sfn_start);          /* you can add different styles... */
ssfn_load(&ctx, &_binary_timesbold_sfn_start);
ssfn_load(&ctx, &_binary_timesitalic_sfn_start);
ssfn_load(&ctx, &_binary_emoji_sfn_start);          /* ...or different UNICODE ranges */
ssfn_load(&ctx, &_binary_cjk_sfn_start);

/* select the typeface to use */
ssfn_select(&ctx,
    SSFN_FAMILY_SERIF, NULL,                        /* family */
    SSFN_STYLE_REGULAR | SSFN_STYLE_UNDERLINE,      /* style */
    64                                              /* size */
);

/* rasterize the first glyph in an UTF-8 string into a 32 bit packed pixel buffer */
ssfn_render(&ctx, &buf, "A");

/* free resources */
ssfn_free(&ctx);                                    /* free the renderer context's internal buffers */
```

There's more, you can use the C++ wrapper class, you can select font by it's name and you can also query the
bounding box for example, and `ssfn_text` will render entire strings into newly allocated pixel buffers,
read the [API reference](https://gitlab.com/bztsrc/scalable-font2/blob/master/docs/API.md).

As with the simple renderer, the header contains everything, no additional linking required! Gzip uncompressor
also included, no need to link with zlib!

License
-------

Both the renderers, the converter and editor utilities are licensed under the terms of MIT license in the hope
that they will be useful.

IMPORTANT NOTE: although the file format is licensed under MIT, it is possible that the font stored in a SSFN
file is NOT! Always consult the license field in the font's header! See `sfnconv -d`.

Dependencies
------------

The simple renderer calls no functions at all and therefore has no dependencies whatsoever (not even libc
nor compiler built-ins). Absolutely nothing save for it's two global variables.

As for the normal renderer, with a define you can also compile it dependency-free (with some limitations)
but all dependencies are provided as built-ins by gcc or by libc:
 - `realloc()` and `free()` from libc (stdlib.h)
 - `memcmp()` and `memset()` from libc (string.h)

The scalable font converter is built on the **freetype2** library to read vector font files (compiled
statically into libsfn by default). The bitmap font converter has no dependencies. Libsfn can be built
optionally with **zlib** to write gzip deflate compressed files on-the-fly (read is supported without
zlib). Font vectorization needs **libm** (for sqrt() and pow() calls in potrace).

The editor uses SDL2 with an X11 fallback, and naturally depends on libsfn.

The test applications use SDL2 to create a window and display the rendered texts.

Changes to SSFN 1.0
-------------------

SSFN 2.0 uses a conceptually different method to rasterize and scale glyphs. SSFN 2.0 does not only support mixed
glyphs in a font (like SSFN 1.0), but it can also mix bitmap, pixmap and vector layers within one glyph, and the
result is much more readable in small rendering sizes. To do that, it only supports 32 bit packed pixel buffers,
but takes care of the alpha-blending.

### Disadvantages

- the renderer uses considerably more memory than SSFN 1.0 (~64k vs. ~24k, and with glyph cache enabled probably megabytes).
- it does not return the outline nor the rasterized glyph any more.

### Advantages

- simpler API, fewer functions (load + select + render + free).
- high quality, anti-aliased rendering directly to 32 bit pixel buffers.
- faster rendering with less malloc calls and internal glyph caching.
- smaller, more compact font files.
- transparent uncompression of gzipped fonts.
- support for non-standardized and user-defined ligatures.
- support for color glyphs.
- separate libsfn library to manipulate font files.

Known Bugs
----------

Nothing that I know of in the renderer. But no programmer can test their own code properly. I've ran the SSFN renderer
through valgrind with many different font files with all the tests without problems. If you find a bug, please
use the [Issue Tracker](https://gitlab.com/bztsrc/scalable-font2/issues) on gitlab and let me know.

With the converter, there's the neverending issue with querying the advance offsets correctly from FreeType2. You might
need to adjust some manually.

Loading kerning information from SplineFontDB .sfd files is not implemented as of yet.

Hinting is supported by the format, but not implementeted by the renderer (libsfn supports loading and saving the hinting
grids). With the new bilinear interpolation scaler I'm not sure it's needed, we'll see how testing goes.

Contributors
------------

I'd like to say thanks to @mrjbom, who tested the library throughfully and pointed out many usability issues and
by that helped me a lot to improve this code.

Authors
-------

- SSFN format, converter, editor and renderers: bzt
- STBI (PD, original zlib decoder): Sean Barrett
- potrace (GPL): Peter Selinger
- libimagequant (GPL): Jef Poskanzer, Greg Roelofs, Kornel Lesiński
- freetype2 (GPL): David Turner, Robert Wilhelm, Werner Lemberg
