/*
 * sfnedit/ui_dummy.c
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
 * @brief User interface driver skeleton for an arbitrary hobby OS
 *
 */

#include <string.h>
#include "lang.h"
#include "util.h"
#include "ui.h"

/* TODO: include your user interface's headers here */
/*#include <dummy.h> */

/**
 * Copy text to clipboard
 */
void ui_copy(char *s)
{
    /* TODO: store the string "s" to the clipboard (up to 5 chars). In lack of such an interface, just print it to console */
    printf("UTF8: %s\n", s);
}

/**
 * Create a window
 */
void *ui_createwin(int w, int h)
{
    /* TODO: open a window of size w x h. "youwintype" could be a pointer, a numeric id, whatever you use. */
    yourwintype *window = ...;
    return (void *)window;
}

/**
 * Set window title
 */
void ui_titlewin(ui_win_t *win, char *title)
{
    /* TODO: set the title for (yourwintype *)win->winid */
}

/**
 * Resize a window
 */
void ui_resizewin(ui_win_t *win, int w, int h)
{
    /* TODO: resize a window to w x h and clear its backstore pixel buffer to theme[THEME_BG] */
    win->data = (uint32_t*)(((yourwintype*)win->winid)->pixelbuffer);
    for(i = 0; i < w * h; i++) win->data[i] = theme[THEME_BG];
}

/**
 * Display modified window
 */
void ui_flushwin(ui_win_t *win, int x, int y, int w, int h)
{
    /* TODO: the area (x,y)-(x+w,y+h) in backstore pixel buffer has changed, refresh window (yourwintype *)win->winid */
}

/**
 * Destroy a window
 */
void ui_destroywin(ui_win_t *win)
{
    /* TODO: free backstore buffer win->data and close the window (yourwintype *)win->winid */
}

/**
 * Focus a window
 */
void ui_focuswin(ui_win_t *win)
{
    /* TODO: refresh window (yourwintype *)win->winid with the backstore buffer, raise and give focus to it */
}

/**
 * Change cursor of window
 */
void ui_cursorwin(ui_win_t *win, int cursor)
{
    /* TODO: set cursor of window (yourwintype *)win->winid to "cursor" (see ui.h CURSOR_x defines) */
}

/**
 * Initialize DUMMY driver
 */
void ui_init()
{
    /* TODO: initialize your user interaface, load icons, cursors etc. report errors with */
    ui_error("DUMMY", ERR_DISPLAY);
}

/**
 * Finish DUMMY driver
 */
void ui_fini()
{
    /* TODO: release resources you allocated in ui_init() */
}

/**
 * Convert a DUMMY event
 */
void ui_getevent()
{
    /* TODO: wait for and read an event (blocking). Fill up the "event" global structure accordingly, see ui.h E_x defines */
}
