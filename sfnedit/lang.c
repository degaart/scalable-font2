/*
 * sfnedit/lang.c
 *
 * Copyright (C) 2019 bzt (bztsrc@gitlab)
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
 * @brief Multilanguage support
 *
 */

#include "lang.h"

/**
 * Translations
 */
char *dict[NUMLANGS][NUMTEXTS + 1] = {
    {
        "en",

        "memory allocation error",
        "unable to open display",
        "unable to load font",
        "unable to save font",
        "bad glyph size",

        "Measuring bounding box",
        "Querying outlines",
        "Querying all kerning combinations",
        "Quantizing image",
        "Reading file",
        "Reading bitmap",
        "Reading tall pixel map",
        "Reading wide pixel map",
        "Generating fragments",
        "Compressing fragments",
        "Serializing fragments",
        "Writing character map",
        "Writing file",
        "Rasterizing",
        "Vectorizing",
        "Copying to clipboard",

        "Help:",
        "If anytime you feel lost, press [F1]. UNICODE database:",
        "Permission is hereby granted, free of charge, to any person",
        "obtaining a copy of this software and associated documentation",
        "files (the \"Software\"), to deal in the Software without",
        "restriction, including without limitation the rights to use, copy,",
        "modify, merge, publish, distribute, sublicense, and/or sell copies",
        "of the Software, and to permit persons to whom the Software is",
        "furnished to do so, subject to the following conditions:",
        "",
        "The above copyright notice and this permission notice shall be",
        "included in all copies or substantial portions of the Software.",

        "About",
        "Load Font",
        "Save Font",
        "Properties",
        "Blocks coverage",
        "Characters",
        "Measures",
        "Layers",
        "Kerning",
        "Color Picker",

        "Name",
        "Size",
        "Modified",
        "Range:",
        "replace glyphs",
        "Import",
        "Save",
        "Save modified font?",
        "Drop all glyphs and start anew?",
        "No",
        "Yes",
        "Sunday",
        "Monday",
        "Tuesday",
        "Wednesday",
        "Thursday",
        "Friday",
        "Saturday",
        "Yesterday",
        "Just now",
        "%d minutes ago",
        "%d hours ago",
        "an hour ago",

        "Type",
        "Unique name",
        "Family name",
        "Subfamily",
        "Font revision",
        "Manufacturer",
        "License",
        "Serif",
        "SansSerif",
        "Decorative",
        "Monospace",
        "Handwriting",
        "Bold",
        "Italic",
        "usrdef1",
        "usrdef2",

        "Show uncovered blocks",
        "Coverage",
        "UNICODE Block",

        "Vectorize font",
        "Unable to vectorize, compiled without potrace",
        "Rasterize font at given size",
        "Zoom out, smaller glyphs",
        "Zoom in, larger glyphs",
        "Copy and delete glyphs",
        "Copy glyphs",
        "Paste glyphs",
        "Delete glyphs",
        "Search results",
        "undefined by UNICODE",
        "SSFN LIGATURE",
        "Selected",

        "UTF-8 copied to clipboard",
        "Glyph's width",
        "Glyph's height",
        "Baseline position (global)",
        "Underline position (global)",
        "Horizontal overlap",
        "Horizontal advance",
        "Vertical advance",
        "Right-to-Left advance",
        "Horizontal advance",
        "Left-to-Right advance",
        "Reposition glyph",
        "Make italic",
        "Make unitalic",
        "Flip horizontally",
        "Flip vertically",
        "Recalculate dimensions",
        "Drop all layers",

        "Zoom out layer",
        "Zoom in layer",
        "Copy and delete layer",
        "Copy layer",
        "Paste layer",
        "Add a new vector layer",
        "Add a new bitmap layer",
        "Add a new pixel map layer",
        "Change foreground color",
        "Erase to transparency",
        "Drop current layer",

        "Delete kerning pair",

        "OK",

        "There's one main window per font with it's properties and character table.",
        "You can open one glyph window per character.",
        "Keys available all the time\none\ntwo",
        "load font",
        "save font",
        "font props",
        "ranges",
        "glyphs",
        "measures",
        "layers",
        "kerning",
        "colorpick"
    }
};
