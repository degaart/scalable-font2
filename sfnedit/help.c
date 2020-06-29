/*
 * sfnedit/help.c
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
 * @brief Contextual help window
 *
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "libsfn.h"
#include "ui.h"
#include "lang.h"

/**
 * Help window
 */
void view_help(int idx)
{
    int i, c;
    char *title, *wt, *s;
    ui_win_t *win = &wins[idx];

    i = wins[idx].tool < 0 ? 0 : wins[idx].tool;
    if(!idx) {
        title = lang[MTOOL_ABOUT + i];
        wt = lang[WHELP_MWIN];
        s = lang[MHELP_ABOUT + i];
    } else {
        title = lang[GTOOL_MEASURES + i];
        wt = lang[WHELP_GWIN];
        s = lang[GHELP_MEASURES + i];
    }
    ssfn_dst.ptr = (uint8_t*)win->data;
    ssfn_dst.p = win->p*4;
    ssfn_dst.w = win->w;
    ssfn_dst.h = win->h;
    ssfn_dst.x = 8;
    ssfn_dst.y = 8;

    ssfn_dst.fg = 0xFF000000 | ((theme[THEME_BG] >> 2) & 0x3F3F3F);
    ssfn_dst.bg = 0;
    ui_text(win, ssfn_dst.x+1, 9, lang[HELP]);
    ui_text(win, ssfn_dst.x+8, 9, title);
    ssfn_dst.x = 8;
    ssfn_dst.fg = 0xFF000000 | ((theme[THEME_FG] << 2) & 0xFCFCFC);
    ui_text(win, ssfn_dst.x, 8, lang[HELP]);
    ui_text(win, ssfn_dst.x+8, 8, title);
    ssfn_dst.fg = theme[THEME_FG];
    ui_text(win, 8, 32, wt);
    ssfn_dst.x = 8;
    ssfn_dst.y = 52;
    while(*s) {
        c = ssfn_utf8(&s);
        if(c == '[') { ssfn_dst.fg = theme[THEME_BG]; ssfn_dst.bg = theme[THEME_FG]; ssfn_putc(' '); } else
        if(c == ']') { ssfn_putc(' '); ssfn_dst.fg = theme[THEME_FG]; ssfn_dst.bg = theme[THEME_BG]; } else
        if(c == '\t') { ssfn_dst.x = 8 + 20*8; } else
        if(c == '\n' || ssfn_dst.x + 8 >= win->w) { ssfn_dst.x = 8; ssfn_dst.y += 18; }
        else ssfn_putc(c);
    }
}
