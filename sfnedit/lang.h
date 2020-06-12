/*
 * sfnedit/lang.h
 *
 * Copyright (C) 2020 bzt (bztsrc@gitlab)
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * @brief Multilanguage definitions
 *
 */

enum {
    /* error messages */
    ERR_MEM = 0,
    ERR_DISPLAY,
    ERR_LOAD,
    ERR_SAVE,
    ERR_SIZE,
    /* status bar */
    STAT_MEASURE,
    STAT_OUTLINE,
    STAT_GETKERN,
    STAT_QUANT,
    STAT_RDFILE,
    STAT_BITMAP,
    STAT_TALLPIX,
    STAT_WIDEPIX,
    STAT_GENFRAG,
    STAT_COMPFRAG,
    STAT_SERFRAG,
    STAT_WRTCHARS,
    STAT_WRTFILE,
    STAT_RASTERIZE,
    STAT_VECTORIZE,
    STAT_COPY,
    /* copyright text */
    HELPSTR,
    CPYRGT_0,
    CPYRGT_1,
    CPYRGT_2,
    CPYRGT_3,
    CPYRGT_4,
    CPYRGT_5,
    CPYRGT_6,
    CPYRGT_7,
    CPYRGT_8,
    CPYRGT_9,
    /* tools */
    MTOOL_ABOUT,
    MTOOL_LOAD,
    MTOOL_SAVE,
    MTOOL_PROPS,
    MTOOL_RANGES,
    MTOOL_GLYPHS,
    GTOOL_MEASURES,
    GTOOL_LAYERS,
    GTOOL_KERN,
    /* file operations */
    FILEOP_NAME,
    FILEOP_SIZE,
    FILEOP_TIME,
    FILEOP_RANGE,
    FILEOP_REPLACE,
    FILEOP_IMPORT,
    FILEOP_SAVE,
    FILEOP_DOSAVE,
    FILEOP_NEW,
    FILEOP_NO,
    FILEOP_YES,
    FILEOP_WDAY0, FILEOP_WDAY1, FILEOP_WDAY2, FILEOP_WDAY3, FILEOP_WDAY4, FILEOP_WDAY5, FILEOP_WDAY6,
    FILEOP_YESTERDAY,
    FILEOP_NOW,
    FILEOP_MSAGO,
    FILEOP_HSAGO,
    FILEOP_HAGO,
    /* properties */
    PROP_TYPE,
    PROP_NAME,
    PROP_FAMILY,
    PROP_SUBFAM,
    PROP_REVISION,
    PROP_MANUFACTURER,
    PROP_LICENSE,
    PROP_SERIF,
    PROP_SANS,
    PROP_DECOR,
    PROP_MONO,
    PROP_HAND,
    PROP_BOLD,
    PROP_ITALIC,
    PROP_USRDEF1,
    PROP_USRDEF2,
    /* ranges */
    RANGES_SHOWALL,
    RANGES_COVERAGE,
    RANGES_NAME,
    /* glyphs */
    GLYPHS_VECTORIZE,
    GLYPHS_NOVECTORIZE,
    GLYPHS_RASTERIZE,
    GLYPHS_ZOOMOUT,
    GLYPHS_ZOOMIN,
    GLYPHS_CUT,
    GLYPHS_COPY,
    GLYPHS_PASTE,
    GLYPHS_DELETE,
    GLYPHS_RESULTS,
    GLYPHS_UNDEF,
    GLYPHS_LIGATURE,
    GLYPHS_SELECT,
    /* coords */
    COORDS_CLIPBRD,
    COORDS_WIDTH,
    COORDS_HEIGHT,
    COORDS_BASELINE,
    COORDS_UNDERLINE,
    COORDS_OVERLAP,
    COORDS_HADV,
    COORDS_VADV,
    COORDS_RTL,
    COORDS_HORIZ,
    COORDS_LTR,
    COORDS_REPOS,
    COORDS_ITALIC,
    COORDS_UNITALIC,
    COORDS_HFLIP,
    COORDS_VFLIP,
    COORDS_RECALC,
    COORDS_DELETE,
    /* layers */
    LAYERS_ZOOMOUT,
    LAYERS_ZOOMIN,
    LAYERS_CUT,
    LAYERS_COPY,
    LAYERS_PASTE,
    LAYERS_VECTOR,
    LAYERS_BITMAP,
    LAYERS_PIXMAP,
    LAYERS_FOREGROUND,
    LAYERS_BACKGROUND,
    LAYERS_DELETE,
    /* kerning */
    KERN_DELETE,
    /* colors */
    COLORS_SET,
    /* must be the last */
    NUMTEXTS
};

#define NUMLANGS         1

extern char *dict[NUMLANGS][NUMTEXTS + 1], **lang;

