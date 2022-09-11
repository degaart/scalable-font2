Scalable Screen Font 2.0 Comparisons
====================================

[[_TOC_]]

Feature Comparison to Other Font Formats
----------------------------------------

| Feature                | SSFN | ASC  | SFD  | TTF  | OTF  | PST1 | PSF2 | BDF  | PNG  |
| ---------------------- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- |
| Binary file            | ✔Yes | ✗No  | ✗No  | ✔Yes | ✔Yes | ✗No  | ✔Yes | ✗No  | ✔Yes |
| Varying width glyphs   | ✔Yes | ✔Yes | ✔Yes | ✔Yes | ✔Yes | ✔Yes | ✗No  | ✔Yes | ✗No  |
| Bitmap fonts           | ✔Yes | ✔Yes | ✔Yes | ✗No  | ✔Yes | ✗No  | ✔Yes | ✔Yes | ✗No  |
| Pixmap fonts           | ✔Yes | ✔Yes | ✗No  | ✗No  | ✗No  | ✗No  | ✗No  | ✗No  | ✔Yes |
| Scalable fonts         | ✔Yes | ✔Yes | ✔Yes | ✔Yes | ✔Yes | ✔Yes | ✗No  | ✗No  | ✗No  |
| Quadratic curves       | ✔Yes | ✔Yes | ✗No  | ✔Yes | ✗No  | ✗No  | ✗No  | ✗No  | ✗No  |
| Cubic Bezier curves    | ✔Yes | ✔Yes | ✔Yes | ✗No  | ✔Yes | ✔Yes | ✗No  | ✗No  | ✗No  |
| Kerning information    | ✔Yes | ✔Yes | ✔Yes | ✔Yes | ✔Yes | ✔Yes | ✗No  | ✗No  | ✗No  |
| Kerning groups         | ✔Yes | ✗No  | ✔Yes | ✗No  | ✔Yes | ✗No  | ✗No  | ✗No  | ✗No  |
| Glyph Compression      | ✔Yes | ✗No  | ✗No  | ✗No  | ✗No  | ✗No  | ✗No  | ✗No  | ✔Yes |
| CLI converter          | ✔Yes | ✔Yes | ✗No  | ✗No  | ✗No  | ✔Yes | ✔Yes | ✔Yes | ✔Yes |
| Requires SO library(1) | ✗No  | ✗No  | ✔Yes | ✔Yes | ✔Yes | ✔Yes | ✗No  | ✗No  | ✗No  |
| Codepage nightmare(2)  | ✗No  | ✗No  | ✗No  | ✔Yes | ✔Yes | ✔Yes | ✔Yes | ✔Yes | ✗No  |

Note(1): SSFN has a very small, simple to use renderer in an ANSI C header file. Others need the
freetype2 library, but PSF2 and BDF are simple enough to parse them manually.

Note(2): SSFN excusively uses UNICODE ISO-10646 standard as it's only character mapping (which is
usually encoded with UTF-8).

Legend:
 - SSFN: [Scalable Screen Font Binary Format](https://gitlab.com/bztsrc/scalable-font2/blob/master/docs/sfn_format.md)
 - ASC: [Scalable Screen Font ASCII Format](https://gitlab.com/bztsrc/scalable-font2/blob/master/docs/asc_format.md)
 - SFD: FontForge's SplineFontDB
 - TTF: TrueType Font
 - OTF: OpenType Font
 - PST1: PostScript Type 1
 - PSF2: PC Screen Format with UNICODE table (Linux Console)
 - BDF: X11 Bitmap Distribution Format
 - PNG: PNG or Truevision TARGA, used by SSFN to import pixel fonts

Compression Ratio
-----------------

The fonts can be gzipped, the normal renderer can uncompress them transparently. The SSFN compression is based
on the intimate knowledge of the glyphs and geometrical transformations, not on byte occurances, therefore SSFN
fonts can be further compressed with RFC1950 deflate efficiently.

### Vector Glyphs

Because vector font formats are just crazy, you can easily gain 60% - 80% extra space by
[converting them into SSFN](https://gitlab.com/bztsrc/scalable-font2/blob/master/docs/sfnconv.md).

| File              | Size  | Ratio   | Format    |
| ----------------- | ----: | ------: | --------- |
| [FreeSerif.otf](https://www.gnu.org/software/freefont/)     | 2047K | 100.00% | OpenType  |
| FreeSerif.sfn     |  840K |  41.04% | SSFN      |
| FreeSerif.sfn.gz  |  595K |  29.08% | SSFN+zlib |
| UbuntuBold.ttf    |  333K | 100.00% | Truetype  |
| UbuntuBold.sfn    |   68K |  20.63% | SSFN      |
| UbuntuBold.sfn.gz |   40K |  11.99% | SSFN+zlib |
| [Vera.ttf](https://www.gnome.org/fonts/)          |   65K | 100.00% | Truetype  |
| Vera.sfn          |   18K |  27.58% | SSFN      |
| Vera.sfn.gz       |   12K |  19.61% | SSFN+zlib |

### Bitmap Glyphs

Compression depends mostly on the glyph bitmaps, but there's a good chance you can compress those too
if you have many similar glyphs as SSFN uses an effective deduplication algorithm.

| File              | Size  | Ratio   | Format    |
| ----------------- | ----: | ------: | --------- |
| [u_vga16.bdf](http://www.inp.nsk.su/~bolkhov/files/fonts/univga/)       |  386K | 100.00% | BDF       |
| u_vga16.sfn       |   59K |  15.31% | SSFN      |
| u_vga16.sfn.gz    |   22K |   5.81% | SSFN+zlib |
| [unifont8.psf](http://unifoundry.com/unifont/index.html)      |   70K | 100.00% | PSF2      |
| unifont8.sfn      |   75K | 107.28% | SSFN      |
| unifont8.sfn.gz   |   26K |  37.07% | SSFN+zlib |
| unifont16.psf     | 1125K | 100.00% | PSF2      |
| unifont16.sfn     | 1249K | 110.98% | SSFN      |
| unifont16.sfn.gz  |  526K |  46.71% | SSFN+zlib |
| unifont.sfn*      | 1325K | 110.75% | SSFN      |
| unifont.sfn.gz    |  554K |  46.31% | SSFN+zlib |
| unicode.pf2\*\*   | 2398K | 100.00% | GRUB's    |
| unicode.sfn       | 1945K |  81.11% | SSFN      |
| unicode.sfn.gz    |  848K |  35.39% | SSFN+zlib |

* - PSF2 contains either the 8x16 or the 16x16 glyphs, because it simply can't handle different glyph widths
of GNU unifont. SSFN contains per glyph metrics, so without zlib it requires about 10% more storage space than
PSF2, but it can store both 8x16 and 16x16 glyphs in a single font file.

\*\* - GRUB's PFF2 contains per glyph metrics. The test file is included in every standard GRUB installation.

(Also note that in the "fonts" directory you can find an unifont.sfn.gz which was converted from the BDF with
more meta information, and therefore is 817k in size.)

### Pixmap Glyphs

Same as bitmaps, all redundant blocks removed, plus fragments are RLE compressed. Images are quantized
to 254 individual colors, hence the great advantage over truecolor PNG.

| File              | Size  | Ratio   | Format    |
| ----------------- | ----: | ------: | --------- |
| emoji.png         |   29K | 100.00% | PNG       |
| emoji.sfn         |   16K |  55.56% | SSFN      |
| emoji.sfn.gz      |    9K |  30.44% | SSFN+zlib |

Code Size and Performance
-------------------------

I've compiled the [sfndemo](https://gitlab.com/bztsrc/scalable-font2/blob/master/sfntest/sfndemo.c) app in
the tests directory using gcc 9.3.0 and run it on an Intel(R) Core(TM) i5-3317U CPU @ 1.70GHz (running at
800 Mhz, 3394 bogomips). I encourage you to repeat these tests on your own computer. A few notes: file load
time includes the time took to load from disk and also to uncompress gzipped fonts. This sfndemo does not
use the internal glyph cache, so it rasterized each glyph every time they occured.

Here are the results:

### Unoptimized Code (-O0)

```
Simple renderer: 1521 bytes
Normal renderer: 29389 bytes
File load time:      0.042081 sec
Character lookup:    0.001375 sec
Rasterization:       0.125243 sec
Blitting:            0.123046 sec
Kerning:             0.000475 sec
```
Finished in **0.3 sec**.

### With Optimization (-O3)

```
Simple renderer: 1355 bytes
Normal renderer: 29852 bytes
File load time:      0.044324 sec
Character lookup:    0.001025 sec
Rasterization:       0.037352 sec
Blitting:            0.066342 sec
Kerning:             0.000366 sec
```
Finished in **0.15 sec**.

**Update 2022**: I've repeated the same test on the same machine but with the latest kernel and the latest gcc 12.2.0:

### Unoptimized Code (-O0)

```
Simple renderer: 1429 bytes
Normal renderer: 29728 bytes
File load time:      0.084566 sec
Character lookup:    0.002340 sec
Rasterization:       0.194156 sec
Blitting:            0.198645 sec
Kerning:             0.000841 sec
Raster/blit diff:    0.004489 sec
```
Finished in **0.46 sec**.

### With Optimization (-O3)

```
Simple renderer: 1173 bytes
Normal renderer: 30721 bytes
File load time:      0.077983 sec
Character lookup:    0.001618 sec
Rasterization:       0.062947 sec
Blitting:            0.111094 sec
Kerning:             0.000588 sec
Raster/blit diff:    0.048147 sec
```
Finished in **0.24 sec**.

With this gcc 12 version, almost every block become considerably slower for some reason (the source code itself hasn't changed).
The optimized version of rasterization is about fine, but the blitter speed up isn't as much as with the old gcc 9 version. I
might take a look at that and do manual optimization where the new gcc optimizer failed, if/when I have the time.

What's interesting, that the [file load time](../sfntest/sfndemo.c#L45) (which includes zlib decompression and totally independent
to the SSFN library) too takes about twice the time than with the gcc 9 compiled version (and zlib's source definitely hasn't
changed in the meantime). Also gcc 12 incorrectly throws a warning about `ssfn.h:605:31: warning: pointer may be used after ‘realloc’`.
Yeah, I can assure you, that's totally not the case, gcc is just wrong. BTW this function (_ssfn_zexpand) comes from stb's code,
and stbi__zexpand is working perfectly in stb_image too, using exactly the same pointer arithmetic.
