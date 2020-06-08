Scalable Screen Font 2.0 Library API
====================================

Note: this is a supporting library to manage SSFN font files. It is NOT needed by normal applications
that want to render text only. It is needed for applications that want to modify or convert SSFN fonts.
The renderer's function are all prefixed by `ssfn_` (referring to the format), while the supporting
library is prefixed by `sfn_` (referring to the file extention).

The library's philosophy is simple: there's a function that reads fonts from files into a clean and
easy to use in-memory representation. Then there's another that compresses and saves fonts from that
in-memory representation into files. The application is allowed to modify and mess around with the
in-memory representation in any way they seem fit, although a few helper functions are also provided
to do that and make life easier.

Thread Safety
-------------

Unlike the renderer, the supporting library was never intended to be thread safe. Therefore, it is
operating on a single global context. There are also some other global variables that changes the
behaviour, they are listed below next to the functions they influence.

Variable Types
--------------

| typedef        | Description                                               |
| -------------- | --------------------------------------------------------- |
| `sfnctx_t`     | the main `ctx` structure that represents a font in memory |
| `sfnglyph_t`   | structure to represent a glyph. Each character has one    |
| `sfnlayer_t`   | a glyph consist of several layers                         |
| `sfncont_t`    | a contour command, paths consist of a series of commands  |
| `sfnkern_t`    | a kerning relation, with horizontal and vertical offsets  |

As you interface with this library mostly by using its structures directly, here comes a detailed
description of each field. There's one global `ctx` variable, with the following structure:

| `sfnctx_t`   | Description                                                                      |
| ------------ | -------------------------------------------------------------------------------- |
| family       | numeric family type, `SSFN_FAMILY_SERIF` .. `SSFN_FAMILY_HAND`                   |
| style        | OR'd style flags, `SSFN_STYLE_BOLD`, `SSFN_STYLE_ITALIC` etc.                    |
| width        | font width in pixels (8 .. 255)                                                  |
| height       | font height in pixels (8 .. 255)                                                 |
| baseline     | baseline (ascender) of the font (see Font Metrics)                               |
| underline    | underline vertical position in pixels                                            |
| name         | zero terminated UTF-8 string, font's unique name                                 |
| familyname   | zero terminated UTF-8 string, font's family name (like Helvetica, Arial etc.)    |
| subname      | zero terminated UTF-8 string, font's sub-family name (like Regular, Medium etc.) |
| revision     | zero terminated UTF-8 string, font's revision (or version)                       |
| manufacturer | zero terminated UTF-8 string, font's manufacturer (designer, creator, foundry)   |
| license      | zero terminated UTF-8 string, font's license                                     |
| glyphs\[]    | array of `sfnglyph_t`, one for each of the 0x110000 characters                   |
| numcpal      | number of color palette entries (up to 253)                                      |
| cpal\[]      | color palette, each color ARGB (blue is the least significant)                   |
| ligatures\[] | array of zero terminated UTF-8 strings, for characters U+F000 - U+F8FF           |
| numskip      | number of UNICODE code points to skip on load                                    |
| skip\[]      | UNICODE code points to skip                                                      |

There are some more properties, used temporarily by the routines, simply don't care about them. One
glyph record looks like this:

| `sfnglyph_t` | Description                                                                      |
| ------------ | -------------------------------------------------------------------------------- |
| width        | glyph's width in pixels (0 .. 255)                                               |
| height       | glyph's height in pixels (0 .. 255)                                              |
| ovl_x        | glyph's x overlay (left margin) in pixels (0 .. 63)                              |
| adv_x        | glyph's horizontal advance in pixels (-128 .. 127)                               |
| adv_y        | glyph's vertical advance in pixels (-128 .. 127)                                 |
| numlayers    | number of glyph's layers (0 .. 255)                                              |
| layers\[]    | array of layers in `sfnlayer_t` records                                          |
| numkern      | number of kerning pairs for this character                                       |
| kern\[]      | kerning relations in `sfnkern_t` records                                         |
| rtl          | 1 if the character is used in a right-to-left scripting system, 0 otherwise      |

There are some more properties, used temporarily by the routines. The kerning relation is stored
simply, the previous character is the one with the `sfnglyph_t` record. The next character in the
relation and the relative offsets are stored in:

| `sfnkern_t`  | Description                                                                      |
| ------------ | -------------------------------------------------------------------------------- |
| n            | next characters UNICODE code point (0 .. 0x10FFFF)                               |
| x            | horizontal advance difference in pixels (-128 .. 127), add to adv_x              |
| y            | vertical advance difference in pixels (-128 .. 127), add to adv_y                |

Each glyph consist of layers. One layer is stored in the following structure:

| `sfnlayer_t` | Description                                                                      |
| ------------ | -------------------------------------------------------------------------------- |
| type         | the layer's type, `SSFN_FRAG_CONTOUR`, `SSFN_FRAG_BITMAP`, `SSFN_FRAG_PIXMAP`    |
| color        | color index into the palette, 0xFE (foreground) by default                       |
| len          | length of the data                                                               |
| data         | pointer to layer data                                                            |

Now how data represents the layer depends on the layer's type. For bitmaps and pixmaps, that is
simply width\*height color indeces, one for each pixel. 0xFF encodes background, 0xFE foreground
(for both bitmaps and pixmaps), and the rest is an index to the color palette (pixmaps only). With
0xFE, the actual color is taken from the layer's color field (which in turn could also be 0xFE).

When the layer's type is a contour, then data is an array of `sfncont_t` contour commands, and `len`
means the number of elements in the contour commands array (or with other words, length of the path).

| `sfncont_t`  | Description                                                                      |
| ------------ | -------------------------------------------------------------------------------- |
| type         | contour's type, `SSFN_CONTOUR_MOVE`, `SSFN_CONTOUR_LINE`, `SSFN_CONTOUR_QUAD`, etc. |
| px           | next point's x coordinate in path                                                |
| py           | next point's y coordinate in path                                                |
| c1x          | for `SSFN_CONTOUR_QUAD` and `SSFN_CONTOUR_CUBIC`, the first control point's x    |
| c1y          | for `SSFN_CONTOUR_QUAD` and `SSFN_CONTOUR_CUBIC`, the first control point's y    |
| c2x          | for `SSFN_CONTOUR_CUBIC`, the second control point's x                           |
| c2y          | for `SSFN_CONTOUR_CUBIC`, the second control point's y                           |

There are some more technical structures used only during serialization, but basically that's all.

Constructor and Destructor
--------------------------

## Initialization

```c
void sfn_init(sfnprogressbar_t pb);
```

Initializes the global `ctx` struct to store a font in memory.

#### Parameters

| Parameter | Description                       |
| --------- | --------------------------------- |
| pb        | progress bar callback function    |

The definition of the progress bar callback is as follows:
```c
typedef void (*sfnprogressbar_t)(int step, int numstep, int curr, int total, int msg);
```
This callback will be called during load, save and other time consuming tasks. If `numstep` is zero,
then there's only one step in the process, otherwise each step represents a separate progress bar
with different `total` values. Message is passed as a numeric code (see `PBAR_x` defines in libsfn.h),
so that the application can print translated versions easily.

## Free the Context

```c
void sfn_free();
```

Frees the global `ctx` context by freeing it's internal buffers.

Loading Fonts
-------------

```c
int sfn_load(char *filename, int dump);
```

Loads a font into `ctx`. Unlike in the renderer, there can be only one font in the global context. This
function also uses some global variables that influence its workings. Font file can be gzipped, and in
SSFN, ASC, BDF, PCF, PSF, FNT, TGA, PNG, SFD, TTF, OTF, PST, etc. format (anything that libsfn was compiled
support for; vector fonts require `freetype2`, which is by default compiled statically in, and most of the
formats are supported without dependency).

#### Parameters

| Parameter | Description                                                     |
| --------- | --------------------------------------------------------------- |
| filename  | font file's name on the disk                                    |
| dump      | with 0, loads the font, otherwise validates and dumps to stdout |

| Globals   | Description                                                     |
| --------- | --------------------------------------------------------------- |
| pbar      | progress bar callback function                                  |
| rs        | range start (defaults to 0)                                     |
| re        | range end (defaults to 0x10FFFF)                                |
| replace   | replace already existing glyphs from the file                   |
| quiet     | when 1, don't print font errors to stdout                       |

This function reads glyphs for the range `rs` to `re` from the file, skipping characters in `ctx.skip` (see
`sfn_skipadd()` below). If a glyph is already exists in `ctx.glyphs`, then it is skipped too, unless
`replace` is 1. For pixel map fonts, `rs` and `re` specifies how many glyphs the image has, so implicitly
the loaded glyph's dimensions.

#### Return value

Non-zero on success.

Saving Fonts
------------

```c
int sfn_save(char *filename, int ascii, int compress);
```

Saves an in-memory font into a file in SSFN or ASC format. It is recommended to call `sfn_sanitize()` beforehand.

#### Parameters

| Parameter | Description                                                     |
| --------- | --------------------------------------------------------------- |
| filename  | font file's name on the disk                                    |
| ascii     | use the [ASCII](https://gitlab.com/bztsrc/scalable-font2/blob/master/docs/asc_format.md) format |
| compress  | use additional gzip deflate compression                         |

| Globals   | Description                                                     |
| --------- | --------------------------------------------------------------- |
| pbar      | progress bar callback function                                  |
| hinting   | save hinting grid to the output (defaults to no)                |

#### Return value

Non-zero on success.

Coverage Reports
----------------

```c
void sfn_coverage();
```

Prints out UNICODE blocks coverage report to the stdout.

Helper Functions to Manipulate Fonts
------------------------------------

### Skip Characters

```c
void sfn_skipadd(int unicode);
```

Unfortunatelly many fonts has bad glyphs. You can specify a list of UNICODE code points to skip on load with
this function, you can call it repeatedly.

### Consistency Checks

```c
void sfn_sanitize();
```

Enforces internal self-consistency on the in-memory font. It worth calling this function after `sfn_load` and
before `sfn_save`.

### Vectorize Bitmap Glyphs

```c
void sfn_vectorize();
```

Vectorize all bitmap layers into contour paths.

#### Parameters

| Globals   | Description                                                     |
| --------- | --------------------------------------------------------------- |
| pbar      | progress bar callback function                                  |

### Rasterize Vector Glyphs

```c
void sfn_rasterize(int size);
```

Rasterize all contour paths into bitmap glyphs at the given `size`.

#### Parameters

| Parameter | Description                                                     |
| --------- | --------------------------------------------------------------- |
| size      | resulting bitmap's height                                       |

| Globals   | Description                                                     |
| --------- | --------------------------------------------------------------- |
| pbar      | progress bar callback function                                  |

### Modify Meta Data

```c
void sfn_setfamilytype(int t);
```

Modify font's type property.

#### Parameters

| Parameter | Description                                                     |
| --------- | --------------------------------------------------------------- |
| t         | one of `SSFN_FAMILY_x` defines                                  |

```c
void sfn_setstr(char **s, char *n, int len);
```

Set a string property in the font. Use with `ctx.name`, `ctx.license` etc. and `ctx.ligatures`.

#### Parameters

| Parameter | Description                                                     |
| --------- | --------------------------------------------------------------- |
| s         | pointer to the string pointer                                   |
| n         | pointer to the new UTF-8 string (zero terminated if `len` is 0) |
| len       | 0 or the new string's length                                    |

### Get Color Index

```c
unsigned char sfn_cpaladd(int r, int g, int b, int a);
```

Returns the color index for a particular Alpha, Red, Green, Blue value. Adds it to the color palette
if necessary, and does minimalistic color interpolation.

### Add a Character

```c
int sfn_charadd(int unicode, int w, int h, int ax, int ay, int ox);
```

Adds a character to the font with no layers (not all characters has glyphs, think about space).

#### Parameters

| Parameter | Description                                                     |
| --------- | --------------------------------------------------------------- |
| unicode   | code point                                                      |
| w         | glyph's width                                                   |
| h         | glyph's height                                                  |
| ax        | advance x                                                       |
| ay        | advance y                                                       |
| ox        | overlap x                                                       |

| Globals   | Description                                                     |
| --------- | --------------------------------------------------------------- |
| rs        | range start (defaults to 0)                                     |
| re        | range end (defaults to 0x10FFFF)                                |
| replace   | replace already existing glyphs from the file                   |

This function adds empty glyphs if in the range `rs` to `re`, skipping characters in `ctx.skip` (see
`sfn_skipadd()` above). If a glyph is already exists in `ctx.glyphs`, then it is skipped too, unless
`replace` is 1.

#### Return value

Non-zero on success.

### Add a Layer (Empty Path) to Character

```c
sfnlayer_t *sfn_layeradd(int unicode, int t, int x, int y, int w, int h, int c, unsigned char *data);
```

#### Parameters

| Parameter | Description                                                     |
| --------- | --------------------------------------------------------------- |
| unicode   | code point                                                      |
| t         | one of `SSFN_FRAG_x` defines                                    |
| x         | layer's x position                                              |
| y         | layer's y position                                              |
| w         | layer's width                                                   |
| h         | layer's height                                                  |
| c         | layer's color index (returned by `sfn_cpaladd()` or 0xFE)       |
| data      | pointer to layer data or NULL                                   |

For bitmaps and pixmaps, `data` is an array of color indeces, but for bitmaps only 0xFE (foreground)
and 0xFF (background) allowed. For contour layers (when `t` is `SSFN_FRAG_CONTOUR`), `data` must be
NULL, and several `sfn_contadd()` calls must be made to fill up the layer with a path.

#### Return value

Pointer to the newly allocated layer or NULL on error.

### Delete a Layer from a Character

```c
void sfn_layerdel(int unicode, int idx);
```

Removes a glyph layer from a character.

#### Parameters

| Parameter | Description                                                     |
| --------- | --------------------------------------------------------------- |
| unicode   | code point                                                      |
| idx       | layer index                                                     |

### Add Contour Command to Path

```c
int sfn_contadd(sfnlayer_t *lyr, int t, int px, int py, int c1x, int c1y, int c2x, int c2y);
```

Add a contour to a path. Each path MUST start with a "move to" command, and there can be only one "move to"
per layer. Admittedly this is not checked nor enforced, so be careful!

#### Parameters

| Parameter | Description                                                                      |
| --------- | -------------------------------------------------------------------------------- |
| lyr       | pointer to a layer, returned by `sfn_layeradd()`                                 |
| t         | one of `SSFN_CONTOUR_x` defines                                                  |
| px        | next point's x coordinate in path                                                |
| py        | next point's y coordinate in path                                                |
| c1x       | for `SSFN_CONTOUR_QUAD` and `SSFN_CONTOUR_CUBIC`, the first control point's x    |
| c1y       | for `SSFN_CONTOUR_QUAD` and `SSFN_CONTOUR_CUBIC`, the first control point's y    |
| c2x       | for `SSFN_CONTOUR_CUBIC`, the second control point's x                           |
| c2y       | for `SSFN_CONTOUR_CUBIC`, the second control point's y                           |

#### Return value

Non-zero on success.

### Add Kerning Relation to a Character

```c
int sfn_kernadd(int unicode, int next, int x, int y);
```

Adds relative offsets to advances.

#### Parameters

| Parameter | Description                                                     |
| --------- | --------------------------------------------------------------- |
| unicode   | the previous code point                                         |
| next      | the next code point                                             |
| x         | horizontal relative offset                                      |
| y         | vertical relative offset                                        |

#### Return value

Non-zero on success.

### Generate Hinting Grid for a Character

```c
void sfn_hintgen(int unicode);
```

Recalculates hinting grid for a character by measuring vertical and horizontal lines
in the contour path.

#### Parameters

| Parameter | Description                                                     |
| --------- | --------------------------------------------------------------- |
| unicode   | code point                                                      |

#### Return value

Updated `ctx.glyphs[unicode].hintv[]` and `ctx.glyphs[unicode].hinth[]` arrays.

### Rasterize a Character or Layer

```c
int sfn_glyph(int size, int unicode, int layer, ssfn_glyph_t *g);
```

Rasterizes the specified layer or, if layer is -1 all layers.

#### Parameters

| Parameter | Description                                                     |
| --------- | --------------------------------------------------------------- |
| size      | size to rasterize at                                            |
| unicode   | code point                                                      |
| layer     | layer to rasterize, or -1 for all                               |
| g         | pointer to a glyph structure to fill                            |

#### Return value

Non-zero on success, and `g` filled up with a rasterized glyph or layer.

### Delete a Character

```c
void sfn_chardel(int unicode);
```

Removes a character with all of its glyph layers and kerning info from memory.

#### Parameters

| Parameter | Description                                                     |
| --------- | --------------------------------------------------------------- |
| unicode   | code point                                                      |
