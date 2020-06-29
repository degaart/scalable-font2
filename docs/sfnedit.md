Scalable Screen Font 2.0 Editor
===============================

This is a font converter and editor with a GUI for [SSFN](https://gitlab.com/bztsrc/scalable-font2/blob/master/docs/sfn_format.md)
font files.

Command Line
------------

Has only optional arguments.
```sh
./sfnedit [-l <language>] [-t <theme>] [font file]

-l <language>   override autodetected language, uses two letter codes, like "en", "hu", "es", "de" etc.
-t <theme>      loads a theme from a GIMP palette file
<font file>     loads the font on application start
```

THe font can be virtually in any font format out there (.ttf, otf, .pfa, .pfb, .sfd, .psf, .bdf, .pcf etc.)

The editor is multilingual, and autodetects the language from the environment. If you want to use
a different language than your OS' default, you can specify it in the LANG environment variable:
```sh
$ LANG=hu ./sfnedit
```
Or use the `-l` flag.

User Interface
--------------

<img src='https://gitlab.com/bztsrc/scalable-font2/raw/master/docs/sfnedit.png'>

All windows are splited vertically into a tab navigation bar (on the top), and a
tab area (the rest, bigger part of the window). By clicking on the tab icon in the
navigation bar, you can switch the tab area.

These keyboard shortcuts work in all windows and in all tabs.

| Key Combination | Description |
| --------------- | ----------- |
| <kbd>F1</kbd>   | Context dependent built-in help |
| <kbd>Esc</kbd>  | Close the window |
| <kbd>Ctrl</kbd>+<kbd>s</kbd> | Quick Save the font |
| <kbd>Shift</kbd>+<kbd>Ctrl</kbd>+<kbd>s</kbd> | Save As |
| <kbd>Tab</kbd>  | Switch to next field |
| <kbd>Shift</kbd>+<kbd>Tab</kbd>  | Switch to previous field |

Main Window
-----------

There's only one main window per font. If you want to edit more fonts in paralell,
then you have to start more instances of `sfnedit`. If you close this window, then
all windows will be closed and the application quits. If there are unsaved modifications
on the font, then you will be asked if you want to save them.

## About

<img src='https://gitlab.com/bztsrc/scalable-font2/raw/master/docs/sfnedit1.png'>

Shows the About page for `sfnedit`, with a clickable link to the rpository and the copyright.
Also shows the UNICODE name database's date and GNU unifont's version that were built into `sfnedit`.

## Load Font

<img src='https://gitlab.com/bztsrc/scalable-font2/raw/master/docs/sfnedit2.png'>

Here you can import glyphs from fonts. Normally only empty slots are loaded, unless you check
`replace glyphs`. You can also limit the import to a certain range.

## Save Font

<img src='https://gitlab.com/bztsrc/scalable-font2/raw/master/docs/sfnedit3.png'>

Similar to load, but for saving the font. You can specify the format (if you check ASC then
the font will be saved in [ASC](https://gitlab.com/bztsrc/scalable-font2/blob/master/docs/asc_format.md)
otherwise in [SSFN](https://gitlab.com/bztsrc/scalable-font2/blob/master/docs/sfn_format.md)).

## Properties Tab

<img src='https://gitlab.com/bztsrc/scalable-font2/raw/master/docs/sfnedit4.png'>

This tab shows the properties of the font.

## Blocks Coverage Tab

<img src='https://gitlab.com/bztsrc/scalable-font2/raw/master/docs/sfnedit5.png'>

This tab gives an overview of which UNICODE character blocks have glyph definitions
in the font.

By clicking "Show uncovered blocks" then blocks with absolutely no glyphs will
be shown too. It is possible to have more than 100% coverage if glyphs are defined for
code points that are undefined by UNICODE Inc.

You can search for UNICODE block names (both normal and short versions).

## Characters Tab

<img src='https://gitlab.com/bztsrc/scalable-font2/raw/master/docs/sfnedit6.png'>

Here you can see the characters for which the font has glyphs. If you have came here by
clicking on a UNICODE block, then that block will be selected for you. You can also make
selections by left clicking and moving the mouse. Simple left click will open the glyph
window (see below). Light background with a dark, low-res bitmap glyph representation means
the code point is specified by UNICODE, but has no glyph in the font. Dark background with
light glyphs are generated from the font, and are real-time representations of all
modifications you may have done on the font.

This tab has its own toolbar.

<img src='https://gitlab.com/bztsrc/scalable-font2/raw/master/docs/sfnedit7.png'>

These are for: vectorization (converting bitmap font into scalable one), rasterization
(the opposite, converting scalable font into a bitmap font at a given size), zoom in and out,
cut, copy and paste selected range, and finally, drop selected range.

| Key Combination | Description |
| --------------- | ----------- |
| <kbd>Down</kbd> / <kbd>Up</kbd>   | Scroll the list |
| <kbd>Left</kbd> / <kbd>Right</kbd> | Select a glyph variant table |
| <kbd>Shift</kbd> + left click and move | Select a range |
| <kbd>Ctrl</kbd> + <kbd>c</kbd> | Copy the selection to clipboard |
| <kbd>Ctrl</kbd> + <kbd>x</kbd> | Cut out the selection and store it to clipboard |
| <kbd>Ctrl</kbd> + <kbd>p</kbd> | Paste a range from clipboard started at selection |
| left click    | Open glyph window for the character |
| any other key | Start a search |

You can search for a UNICODE code point name, for an UTF-8 character, or with a "U+"
prefix and hex number for a particular code point.

Glyph Windows
-------------

You can have as many of these windows as you want, but only one per character. In this
window you can edit and modify the properties of a glyph.

## Metrics Tab

<img src='https://gitlab.com/bztsrc/scalable-font2/raw/master/docs/sfnedit8.png'>

In this tab you can set the glyph's width and height; modify the advances, overlap and other
metrics. You can also set the global baseline and underline here.

This tab also shows the UNICODE code point, as well as the UTF-8 and decimal representation
of the character. If you click on this info, then the UTF-8 version of the character will
be placed on the clipboard, you'll be able to paste it into another application, like into
a text editor.

Quite a few transformation utilities are availalbe, like rotating (moving) the glyph,
flipping vertically and horizontally, make it italic or non-italic, recalculate the bounding
box etc.

On the left you can see the overall glyph representation with all the metrics as guide lines.

## Layers Tab

<img src='https://gitlab.com/bztsrc/scalable-font2/raw/master/docs/sfnedit9.png'>

This is the main editing tab. This also has its own toolbar, the usual zooming and
cut, copy and paste icons. On the right, you can see three icons for adding new layers:
vector contour layer, bitmap layer and pixel map layer.

Below that is the list of current layers. Only one layer can be active at a time, and it
is highlighted in the main editor area.

At the bottom there are again three icons: layer's color (this means pixel color for pixel
layers), color picker (only for pixel layers), and drop layer.

On the left, in the big part of the window, is the layer editor area.

| Key Combination | Description |
| --------------- | ----------- |
| <kbd>h</kbd>    | Flip layer horizontally |
| <kbd>v</kbd>    | Flip layer vertically |
| <kbd>Ctrl</kbd> + <kbd>c</kbd> | Copy the active layer to clipboard |
| <kbd>Ctrl</kbd> + <kbd>x</kbd> | Cut out the active layer and store it to clipboard |
| <kbd>Ctrl</kbd> + <kbd>v</kbd> | Paste a layer (or layers) from clipboard to new layer(s) |
| <kbd>Del</kbd>  | Delete active layer |
| left click      | Select a point, or toggle point for bitmap and pixmap layers |
| left click + move | Without a selected point, move the canvas |
| left click + move | With a selected point (either on-curve or control), move that point |
| <kbd>Shift</kbd> + left click | Add a line to contour |
| <kbd>Ctrl</kbd> + left click | Add a cubic Bezier curve to contour |
| <kbd>Ctrl</kbd> + right click | Add a quadratic Bezier curve to contour |
| <kbd>Backspace</kbd> | Remove last contour command from path |
| wheel           | zooming in or out |

Control points (green X) and on-curve points (blue dots) cannot be moved beyond the bounding box
of the layer, defined on the Metrics tab. The editor forces this. When you move a contour point
around, guide lines will pop up to help you with the alignment.

Hint: when you add new contour commands to the path, the new command is shown as soon as you press
the left button. But it won't be finalized until you release the mouse button, meaning you can
move it around. For example <kbd>Shift</kbd> + left click will show a new line from the last path
point to the current cursor position. You can move that, and the line will change. When you release
the left button, only then will be the line with the current coordinates appended to the path.

## Kerning

<img src='https://gitlab.com/bztsrc/scalable-font2/raw/master/docs/sfneditA.png'>

In this tab you can set up relative character pair offsets. Unlike advances, you can set
up both horizontal and vertical offsets for the same pair. This is very handy in glyph
composition, when you need to place an accent above or below a base glyph.

On the right you have the search box, and beneath the list of specified kerning pairs. By
default only characters with non-zero kerning are shown. When you search for a character,
then characters with zero kerning pop up in the list too.

At the bottom, you have three icons: setting Right-to-Left and Left-to-Right direction
(only available if the glyph hasn't got an Y advance set in the Metrics tab), and a drop
kerning pair icon.

On the left, in the big area, you'll see the choosen character pair. Depending whether
the glyph has vertical or horizontal advance, the pair is shown horizontally or vertically.
Most scripting systems (including Latin, Cyrillic, Greek, etc.) are left-to-right horizontal.
This distinction between horizontal and vertical is stored within the font file.

Unfortunatelly just by the UNICODE code point, you can't always tell if the glyph
has to be written in Right-to-Left direction. Therefore sfnedit tries to figure it
out, but lets the user change that by clicking on the "RTL" icon. This distinction between
Left-to-Right and Right-to-Left is NOT stored within the font file, and requires a BiDi
state machine to be applied when rendering the font. Read more on this in the
[API](https://gitlab.com/bztsrc/scalable-font2/blob/master/docs/API.md) documentation.

| Key Combination | Description |
| --------------- | ----------- |
| <kbd>Left</kbd> / <kbd>Right</kbd> | set horizontal kerning |
| <kbd>Up</kbd> / <kbd>Down</kbd> | set vertical kerning |
| any other key | Start a search |

Note: you can only move the layer if you click on the layer. The paired character might have
a similar background, but you can't click on that. Watch for the layer's measure lines on the
top and on the left.
