/*
 * sfnedit/m_ranges.c
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
 * @brief Main window UNICODE ranges tool
 *
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "libsfn.h"
#include "ui.h"
#include "lang.h"

int showallrange = 0, numranges = 0, scrollranges = 0, pageranges = 0, selranges = 0, clkranges = -1, rres[UNICODE_NUMBLOCKS];
char rsearch[32] = { 0 };
extern int scrollglyphs, selstart, selend, greset;

/**
 * UNICODE ranges window
 */
void view_ranges()
{
    ui_win_t *win = &wins[0];
    char tmp[32];
    int i, j, k, l, m;

    l = strlen(rsearch); numranges = 0;
    if(input_refresh) { scrollranges = selranges = 0; }
    input_refresh = 0;
    for(i = 0; i < UNICODE_NUMBLOCKS; i++) {
        ublocks[i].cnt = 0;
        if(l) {
            k = strlen(ublocks[i].name);
            if(k < l) continue;
            k -= l;
            for(j = 0; j <= k && ui_casecmp(ublocks[i].name + j, rsearch, l); j++);
            if(j > k) continue;
        }
        for(j = ublocks[i].start; j <= ublocks[i].end; j++)
            if(ctx.glyphs[j].numlayer) ublocks[i].cnt++;
        if(!showallrange && !ublocks[i].cnt) continue;
        rres[numranges++] = i;
    }

    ssfn_dst.w = win->w - 8; if(ssfn_dst.w < 1) ssfn_dst.w = 1;
    ssfn_dst.h = win->h;
    ui_bool(win, 8, 29, lang[RANGES_SHOWALL], showallrange, wins[0].field == 6);
    ui_icon(win, win->w - 134 - 16, 30, ICON_SEARCH, 0);
    ui_input(win, win->w - 132, 29, 120, rsearch, wins[0].field == 7, 31, 0);
    ui_rect(win, 7, 51, win->w - 14, win->h - 26 - 51, theme[THEME_DARKER], theme[THEME_LIGHT]);
    ssfn_dst.w = win->w - 9; if(ssfn_dst.w < 1) ssfn_dst.w = 1;
    ui_box(win, 8, 52, 172, 20, theme[THEME_LIGHT], theme[THEME_BG], theme[THEME_DARKER]);
    ssfn_dst.fg = theme[THEME_LIGHTER];
    ui_text(win, 16, 53, lang[RANGES_COVERAGE]);
    ssfn_dst.fg = theme[THEME_DARKER];
    ui_text(win, 15, 52, lang[RANGES_COVERAGE]);
    ui_box(win, 180, 52, win->w - 189, 20, theme[THEME_LIGHT], theme[THEME_BG], theme[THEME_DARKER]);
    ssfn_dst.fg = theme[THEME_LIGHTER];
    ui_text(win, 188, 53, lang[RANGES_NAME]);
    ssfn_dst.fg = theme[THEME_DARKER];
    ui_text(win, 187, 52, lang[RANGES_NAME]);
    ssfn_dst.fg = theme[THEME_FG];
    ssfn_dst.bg = 0;

    ssfn_dst.y = 72;
    pageranges = (win->h - 26 - 72) / 16; if(pageranges < 1) pageranges = 1;
    if(selranges + 1 > scrollranges + pageranges - 1) scrollranges = selranges - pageranges + 1;
    if(selranges >= 0 && selranges < scrollranges) scrollranges = selranges;
    ssfn_dst.h = win->h - 26;
    for(i = scrollranges; i < numranges && ssfn_dst.y < ssfn_dst.h; i++) {
        j = rres[i];
        if(i == selranges && win->field == 8)
            ui_box(win, 9, ssfn_dst.y, win->w - 18, 16, theme[THEME_SELBG], theme[THEME_SELBG], theme[THEME_SELBG]);
        m = ublocks[j].cnt * 1000 / (ublocks[j].end - ublocks[j].start + 1 - ublocks[j].undef);
        sprintf(tmp, "%3d.%02d%%", m / 10, m % 10); m /= 10; if(m > 100) m = 100;
        ssfn_dst.fg = theme[i == selranges && win->field == 8 ? THEME_SELFG : THEME_FG];
        ui_text(win, 13, ssfn_dst.y, tmp);
        ui_box(win, 74, ssfn_dst.y + 3, m, 11, theme[THEME_FG], ssfn_dst.fg, theme[THEME_FG]);
        ui_box(win, 74 + m, ssfn_dst.y + 3, 100 - m, 11, theme[THEME_DARKER], theme[THEME_DARKER], theme[THEME_DARKER]);
        ui_text(win, 185, ssfn_dst.y, ublocks[j].name);
        ssfn_dst.y += 16;
    }
    ssfn_dst.fg = theme[THEME_FG];
    ssfn_dst.w = win->w;
    ssfn_dst.h = win->h;
}

/**
 * On enter handler
 */
void ctrl_ranges_onenter()
{
    if(wins[0].field == 6) { showallrange ^= 1; input_refresh = 1; } else
    if(wins[0].field == 8 && selranges >= 0 && selranges < numranges) {
        wins[0].tool = MAIN_TOOL_GLYPHS;
        wins[0].field = selfield = -1;
        greset = 1;
        selstart = ublocks[rres[selranges]].start;
        selend = ublocks[rres[selranges]].end;
        scrollglyphs = selstart & ~((1<<wins[0].zoom) - 1);
    }
    ui_resizewin(&wins[0], wins[0].w, wins[0].h);
    ui_refreshwin(0, 0, 0, wins[0].w, wins[0].h);
}

/**
 * On key handler
 */
void ctrl_ranges_onkey()
{
    if(wins[0].field == 8) {
        switch(event.x) {
            case K_UP: if(selranges > 0) { selranges--; } break;
            case K_DOWN: if(selranges + 1 < numranges) { selranges++; } break;
            case K_PGUP:
                selranges -= pageranges - 1;
                if(selranges < 0) selranges = 0;
            break;
            case K_PGDN:
                selranges += pageranges - 1;
                if(selranges >= numranges) selranges = numranges - 1;
            break;
            case K_HOME: selranges = 0; break;
            case K_END: selranges = numranges - 1; break;
        }
    } else if(event.x == K_DOWN) {
        wins[0].field = 8;
        selranges = 0;
    }
    if(event.x >= ' ') {
        strcpy(rsearch, (char*)&event.x);
        wins[0].field = 7;
        input_refresh = 1;
        input_maxlen = 0;
        input_str = NULL;
    }

    ui_resizewin(&wins[0], wins[0].w, wins[0].h);
    ui_refreshwin(0, 0, 0, wins[0].w, wins[0].h);
}

/**
 * On button press handler
 */
void ctrl_ranges_onbtnpress()
{
    ui_win_t *win = &wins[0];

    if(event.y > 29 && event.y < 48) {
        if(event.x >= 8 && event.x < win->w - 154) { showallrange ^= 1; input_refresh = 1; } else
        if(event.x >= win->w - 132 && event.x < win->w - 8) wins[0].field = 7;
    } else
    if(event.y > 73 && event.y < win->h - 26) {
        wins[0].field = 8;
        if(event.w & 1) {
            selranges = (event.y - 73) / 16 + scrollranges;
            if(selranges >= numranges) selranges = numranges - 1;
            if(selranges != clkranges) clkranges = selranges;
            else ctrl_ranges_onenter();
        } else
        if(event.w & (1 << 3)) {
            if(scrollranges > 0) scrollranges--;
            if(selranges > scrollranges + pageranges - 1) selranges = scrollranges + pageranges - 1;
        } else
        if(event.w & (1 << 4)) {
            if(scrollranges + pageranges < numranges) scrollranges++;
            if(selranges < scrollranges) selranges = scrollranges;
        }
    } else wins[0].field = -1;
    ui_resizewin(&wins[0], wins[0].w, wins[0].h);
    ui_refreshwin(0, 0, 0, wins[0].w, wins[0].h);
}
