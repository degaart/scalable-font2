/*
 * sfnedit/m_glyphs.c
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
 * @brief Main window glyphs table
 *
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "libsfn.h"
#include "ui.h"
#include "lang.h"
#include "util.h"

int scrollglyphs = 0, numglyphs = 0, pageglyphs = 0, selstart = -1, selend = -1, rastsize = 64, greset = 1, gres[0x110000];
int gsize, glast = -1;
char gsearch[32] = { 0 }, gstat[256] = { 0 }, gdef[0x110000];

/**
 * Character table with glyphs window
 */
void view_glyphs()
{
    int i, j, k, l, x, y, n;
    ui_win_t *win = &wins[0];
    char cp[8], *s = NULL;

    ssfn_dst.ptr = (uint8_t*)win->data;
    ssfn_dst.p = win->p*4;
    ssfn_dst.w = win->w - 150; if(ssfn_dst.w < 1) ssfn_dst.w = 1;
    ssfn_dst.h = win->h;
    ui_box(win, 8, 29, 20, 20, theme[wins[0].field == 6 ? THEME_CURSOR : (!selfield ? THEME_DARKER : THEME_LIGHT)],
        theme[wins[0].field == 6 ? THEME_LIGHT : THEME_BG],
        theme[wins[0].field == 6 ? THEME_CURSOR : (!selfield ? THEME_LIGHT : THEME_DARKER)]);
    ui_icon(win, 10, 31, ICON_VECTOR, 0);
    if(rastsize < 8) rastsize = 8;
    if(rastsize > SSFN_SIZE_MAX) rastsize = SSFN_SIZE_MAX;
    ui_num(win, 32, 29, rastsize, wins[0].field == 7, selfield - 1);
    ui_rect(win, 32, 29, 72, 20, theme[wins[0].field == 7 ? THEME_CURSOR : THEME_DARKER],
        theme[wins[0].field == 7 ? THEME_CURSOR : THEME_LIGHT]);
    ui_box(win, 83, 30, 20, 18, theme[wins[0].field == 7 ? THEME_LIGHT : (selfield == 3 ? THEME_DARKER : THEME_LIGHT)],
        theme[wins[0].field == 7 ? THEME_LIGHT : THEME_BG], theme[wins[0].field == 7 ? THEME_LIGHT :
        (selfield == 3 ? THEME_LIGHT : THEME_DARKER)]);
    ui_icon(win, 85, 31, ICON_BITMAP, 0);
    ui_box(win, 117, 29, 20, 20, theme[wins[0].field == 8 ? THEME_CURSOR : (selfield == 4 ? THEME_DARKER : THEME_LIGHT)],
        theme[wins[0].field == 8 ? THEME_LIGHT : THEME_BG], theme[wins[0].field == 8 ? THEME_CURSOR :
        (selfield == 4 ? THEME_LIGHT : THEME_DARKER)]);
    ui_icon(win, 119, 31, ICON_ZOOMOUT, 0);
    ui_box(win, 139, 29, 20, 20, theme[wins[0].field == 9 ? THEME_CURSOR : (selfield == 5 ? THEME_DARKER : THEME_LIGHT)],
        theme[wins[0].field == 9 ? THEME_LIGHT : THEME_BG], theme[wins[0].field == 9 ? THEME_CURSOR :
        (selfield == 5 ? THEME_LIGHT : THEME_DARKER)]);
    ui_icon(win, 141, 31, ICON_ZOOMIN, 0);
    ui_box(win, 172, 29, 20, 20, theme[wins[0].field == 10 ? THEME_CURSOR : (selfield == 6 ? THEME_DARKER : THEME_LIGHT)],
        theme[wins[0].field == 10 ? THEME_LIGHT : THEME_BG], theme[wins[0].field == 10 ? THEME_CURSOR :
        (selfield == 6 ? THEME_LIGHT : THEME_DARKER)]);
    ui_icon(win, 175, 31, ICON_CUT, 0);
    ui_box(win, 194, 29, 20, 20, theme[wins[0].field == 11 ? THEME_CURSOR : (selfield == 7 ? THEME_DARKER : THEME_LIGHT)],
        theme[wins[0].field == 11 ? THEME_LIGHT : THEME_BG], theme[wins[0].field == 11 ? THEME_CURSOR :
        (selfield == 7 ? THEME_LIGHT : THEME_DARKER)]);
    ui_icon(win, 197, 31, ICON_COPY, 0);
    ui_box(win, 216, 29, 20, 20, theme[wins[0].field == 12 ? THEME_CURSOR : (selfield == 8 ? THEME_DARKER : THEME_LIGHT)],
        theme[wins[0].field == 12 ? THEME_LIGHT : THEME_BG], theme[wins[0].field == 12 ? THEME_CURSOR :
        (selfield == 8 ? THEME_LIGHT : THEME_DARKER)]);
    ui_icon(win, 219, 31, ICON_PASTE, 0);
    ui_box(win, 250, 29, 20, 20, theme[wins[0].field == 13 ? THEME_CURSOR : (selfield == 9 ? THEME_DARKER : THEME_LIGHT)],
        theme[wins[0].field == 13 ? THEME_LIGHT : THEME_BG], theme[wins[0].field == 13 ? THEME_CURSOR :
        (selfield == 9 ? THEME_LIGHT : THEME_DARKER)]);
    ui_icon(win, 252, 31, ICON_DELETE, 0);

    ssfn_dst.w = win->w - 8; if(ssfn_dst.w < 1) ssfn_dst.w = 1;
    ui_box(win, 284, 29, win->w - 150 - 284, 18, theme[THEME_BG], theme[THEME_BG], theme[THEME_BG]);
    if(!gsearch[0]) {
        for(i = 0; i < UNICODE_NUMBLOCKS && (scrollglyphs < ublocks[i].start || scrollglyphs > ublocks[i].end); i++);
        if(i < UNICODE_NUMBLOCKS) s = ublocks[i].name;
    } else
        s = lang[GLYPHS_RESULTS];
    if(s) {
        ssfn_dst.fg = theme[THEME_LIGHTER];
        ui_text(win, 284, 30, s);
        ssfn_dst.fg = theme[THEME_DARKER];
        ui_text(win, 283, 29, s);
        ssfn_dst.fg = theme[THEME_FG];
    }
    ui_icon(win, win->w - 134 - 16, 30, ICON_SEARCH, 0);
    ui_input(win, win->w - 132, 29, 120, gsearch, wins[0].field == 14, 31, 0);
    gsize = (win->w - 20) / (1<<wins[0].zoom);
    i = gsize * (1<<wins[0].zoom); x = (win->w - i) / 2 + 1;
    pageglyphs = ((win->h - 26 - 53) / gsize - 1) * (1<<wins[0].zoom); if(pageglyphs < (1<<wins[0].zoom)) pageglyphs = (1<<wins[0].zoom);
    scrollglyphs &= ~((1 << wins[0].zoom) - 1);
    ui_rect(win, x - 1, 51, i + 1, win->h - 26 - 51, theme[THEME_DARKER], theme[THEME_LIGHT]);
    ssfn_dst.w = x + i;
    ssfn_dst.bg = 0;
    if(greset || (input_refresh && !gsearch[0])) {
        gsearch[0] = 0;
        for(i = 0; i < 0x110000; i++) gres[i] = i;
        memset(gdef, 0, sizeof(gdef));
        for(i = 0; i < UNICODE_NUMNAMES; i++) gdef[uninames[i].unicode] = 1;
        numglyphs = 0x110000;
    } else
    if(input_refresh) {
        numglyphs = scrollglyphs = 0; selstart = selend = -1;
        l = strlen(gsearch);
        n = (gsearch[0] == 'U' || gsearch[0] == 'u') && gsearch[1] == '+' ? (int)gethex(gsearch + 2, 6) : -1;
        /* exact matches first */
        for(i = 0; i < UNICODE_NUMNAMES; i++) {
            if(!strcmp(utf8(uninames[i].unicode), gsearch) || uninames[i].unicode == n)
                gres[numglyphs++] = uninames[i].unicode;
        }
        /* name matches after */
        for(i = 0; i < UNICODE_NUMNAMES; i++) {
            if(strcmp(utf8(uninames[i].unicode), gsearch) && uninames[i].unicode != n) {
                k = strlen(uninames[i].name);
                if(k < l) continue;
                k -= l;
                for(j = 0; j <= k && ui_casecmp(uninames[i].name + j, gsearch, l); j++);
                if(j > k) continue;
                gres[numglyphs++] = uninames[i].unicode;
            }
        }
    }
    greset = input_refresh = 0;
    ssfn_dst.h = win->h - 27;
    for(i = scrollglyphs, j = 0, y = 52; i < numglyphs && y < ssfn_dst.h; i++, j++) {
        if(j == (1<<wins[0].zoom)) { j = 0; y += gsize; }
        if(ctx.glyphs[gres[i]].numlayer || (iswhitespace(gres[i]) && ctx.glyphs[gres[i]].adv_x+ctx.glyphs[gres[i]].adv_y > 0)) {
            ui_box(win, x + j * gsize, y, gsize - 1, gsize - 1, theme[THEME_DARKER],
                theme[i >= selstart && i <= selend ? THEME_SELBG : THEME_DARKER], theme[THEME_DARKER]);
            ssfn_dst.fg = theme[i >= selstart && i <= selend ? THEME_SELFG : THEME_FG];
            ui_glyph(win, x + j * gsize, y, gsize - 1, gres[i], -1);
        } else if(gdef[gres[i]]) {
            ui_box(win, x + j * gsize, y, gsize - 1, gsize - 1, theme[THEME_DARK],
                theme[i >= selstart && i <= selend ? THEME_SELBG : THEME_UNDEF], theme[THEME_LIGHT]);
            ssfn_dst.x = k = x + j * gsize + 3;
            ssfn_dst.y = y + 3;
            ssfn_dst.fg = theme[THEME_DARKER];
            if(ssfn_putc(gres[i])) {
                sprintf(cp, "%06X", gres[i]);
                ssfn_dst.x = k;
                ssfn_dst.y = y + 3;
                ui_hex(win, cp[0]);
                ui_hex(win, cp[1]);
                ui_hex(win, cp[2]);
                ssfn_dst.x = k;
                ssfn_dst.y = y + 9;
                ui_hex(win, cp[3]);
                ui_hex(win, cp[4]);
                ui_hex(win, cp[5]);
            }
        } else
            ui_box(win, x + j * gsize, y, gsize - 1, gsize - 1, theme[THEME_DARK],
                theme[i >= selstart && i <= selend ? THEME_LIGHT : THEME_BG], theme[THEME_LIGHT]);
    }
    if(i == numglyphs)
        for(; y < ssfn_dst.h; i++, j++) {
            if(j == (1<<wins[0].zoom)) { j = 0; y += gsize; }
            ui_box(win, x + j * gsize, y, gsize - 1, gsize - 1, theme[THEME_BG], theme[THEME_BG], theme[THEME_BG]);
        }
    ssfn_dst.fg = theme[THEME_FG];
    ssfn_dst.w = win->w;
    ssfn_dst.h = win->h;
}

/**
 * Copy and cut controller
 */
void ctrl_glyphs_copy(int cut)
{
    int i, j, n;
    if(selend > -1) {
        ui_cursorwin(&wins[0], CURSOR_LOADING);
        for(n = 0, j = 0x110000, i = selstart; i <= selend; i++) {
            if(gres[i] < j) j = gres[i];
            if(ctx.glyphs[gres[i]].numlayer) n++;
        }
        copypaste_start(j);
        for(i = selstart; i <= selend; i++) {
            if(n > 1024) ui_pb(0, 0, i, selend - selstart + 1, PBAR_COPY);
            copypaste_copy(gres[i], -1);
            if(cut) sfn_chardel(gres[i]);
        }
        ui_cursorwin(&wins[0], CURSOR_PTR);
        if(cut) ui_refreshall();
    }
}

/**
 * On enter handler
 */
void ctrl_glyphs_onenter()
{
    int i;

    switch(wins[0].field) {
        case 6:
            wins[0].field = -1;
            selfield = 0;
            ui_cursorwin(&wins[0], CURSOR_LOADING);
            ui_refreshwin(0, 0, 0, wins[0].w, wins[0].h);
            sfn_vectorize();
            ui_cursorwin(&wins[0], CURSOR_PTR);
            ui_refreshall();
            modified++;
        break;
        case 7:
            wins[0].field = -1;
            selfield = 3;
            ui_cursorwin(&wins[0], CURSOR_LOADING);
            ui_refreshwin(0, 0, 0, wins[0].w, wins[0].h);
            sfn_rasterize(rastsize);
            ui_cursorwin(&wins[0], CURSOR_PTR);
            ui_refreshall();
            modified++;
        break;
        case 8:
            if(wins[0].zoom < 6) wins[0].zoom++;
        break;
        case 9:
            if(wins[0].zoom > 2) wins[0].zoom--;
        break;
        case 10:
            ctrl_glyphs_copy(1);
        break;
        case 11:
            ctrl_glyphs_copy(0);
        break;
        case 12:
            ui_cursorwin(&wins[0], CURSOR_LOADING);
            copypaste_paste(selstart >= 0 ? gres[selstart] : 0, 0);
            selstart = selend = glast = -1;
            ui_cursorwin(&wins[0], CURSOR_PTR);
            ui_refreshall();
            modified++;
        break;
        case 13:
            if(selend == -1)
                wins[0].tool = MAIN_TOOL_NEW;
            else {
                for(i = selstart; i <= selend; i++)
                    sfn_chardel(gres[i]);
                ui_refreshall();
                modified++;
            }
        break;
    }
}

/**
 * On key handler
 */
void ctrl_glyphs_onkey()
{

    switch(event.x) {
        case K_UP:
            if(wins[0].field == 7) rastsize++;
            else {
                if(scrollglyphs > 0) scrollglyphs -= (1<<wins[0].zoom);
                wins[0].field = 15;
            }
        break;
        case K_DOWN:
            if(wins[0].field == 7) rastsize--;
            else {
                if(scrollglyphs + pageglyphs + (1<<wins[0].zoom) < numglyphs) scrollglyphs += (1<<wins[0].zoom);
                wins[0].field = 15;
            }
        break;
        case K_PGUP:
            if(scrollglyphs > pageglyphs) scrollglyphs -= pageglyphs; else scrollglyphs = 0;
            wins[0].field = 15;
        break;
        case K_PGDN:
            if(scrollglyphs + pageglyphs + (1<<wins[0].zoom) < numglyphs) scrollglyphs += pageglyphs;
            else scrollglyphs = (numglyphs - pageglyphs - 1) & ~((1<<wins[0].zoom) - 1);
            wins[0].field = 15;
        break;
        case K_HOME: scrollglyphs = 0; wins[0].field = 15; break;
        case K_END: scrollglyphs = (numglyphs - pageglyphs - 1) & ~((1<<wins[0].zoom) - 1); wins[0].field = 15; break;
        case K_DEL: case K_BACKSPC: if(selend != -1) { wins[0].field = 13; ctrl_glyphs_onenter(); wins[0].field = -1; } break;
        default:
            if(event.h & (3 << 1)) {
                switch(event.x) {
                    case 'a': case 'A': selstart = 0; selend = numglyphs - 1; break;
                    case 'x': case 'X': wins[0].field = 10; ctrl_glyphs_onenter(); break;
                    case 'c': case 'C': wins[0].field = 11; ctrl_glyphs_onenter(); break;
                    case 'v': case 'V': wins[0].field = 12; ctrl_glyphs_onenter(); break;
                }
                wins[0].field = 15;
            } else
            if(event.x >= ' ') {
                strcpy(gsearch, (char*)&event.x);
                wins[0].field = 14;
                input_refresh = 1;
                input_maxlen = 0;
                input_str = NULL;
            }
        break;
    }
    ui_refreshwin(0, 0, 0, wins[0].w, wins[0].h);
}

/**
 * On button press handler
 */
void ctrl_glyphs_onbtnpress()
{
    ui_win_t *win = &wins[0];

    selfield = -1;
    if(event.w != 1) glast = -1;
    if(event.y > 29 && event.y < 50) {
        if(event.x >= 8 && event.x < 28) selfield = 0; else
        if(((event.w >> 3) & 3) && event.x >= 32 && event.x < 83) {
            if(event.w & (1 << 3)) rastsize++;
            if(event.w & (1 << 4)) rastsize--;
        } else
        if(event.x >= 72 && event.x < 83) selfield = event.y < 41 ? 1 : 2; else
        if(event.x >= 83 && event.x < 103) selfield = 3; else
        if(event.x >= 117 && event.x < 138) selfield = 4; else
        if(event.x >= 139 && event.x < 159) selfield = 5; else
        if(event.x >= 172 && event.x < 192) selfield = 6; else
        if(event.x >= 194 && event.x < 214) selfield = 7; else
        if(event.x >= 216 && event.x < 236) selfield = 8; else
        if(event.x >= 250 && event.x < 270) selfield = 9; else
        if(event.x >= win->w - 132 && event.x < win->w - 8) wins[0].field = 14;
    } else
    if(event.y >= 52 && event.y < win->h - 26) {
        wins[0].field = 15;
        if(event.w & (1 << 3)) {
            if(scrollglyphs > 0) scrollglyphs -= (1<<wins[0].zoom);
        } else
        if(event.w & (1 << 4)) {
            if(scrollglyphs + pageglyphs + (1<<wins[0].zoom) < numglyphs) scrollglyphs += (1<<wins[0].zoom);
        } else
        if(event.w & 1) {
            selfield = 10;
            selstart = ((event.y - 52) / gsize) * (1<<wins[0].zoom) + (event.x - ((wins[0].w - gsize * (1<<wins[0].zoom))/2 + 1))
                / gsize + scrollglyphs;
            if(selstart < 0 || selstart >= numglyphs) selstart = -1;
            selend = glast = -1;
        } else
        if(event.w & 4) {
            if(selend != -1) { wins[0].field = 11; ctrl_glyphs_onenter(); }
            selstart = ((event.y - 52) / gsize) * (1<<wins[0].zoom) + (event.x - ((wins[0].w - gsize * (1<<wins[0].zoom))/2 + 1))
                / gsize + scrollglyphs;
            if(selstart < 0 || selstart >= numglyphs) selstart = -1;
            selend = glast = -1;
            wins[0].field = 12; ctrl_glyphs_onenter();
            selstart = -1;
        }
    }
}

/**
 * On click (button release) handler
 */
void ctrl_glyphs_onclick()
{
    if(event.y > 29 && event.y < 50) {
        if(event.x >= 8 && event.x < 28 && !selfield) { wins[0].field = 6; ctrl_glyphs_onenter(); } else
        if(event.x >= 72 && event.x < 83) {
            if(selfield == 1 && event.y < 41) rastsize++;
            if(selfield == 2 && event.y >= 41) rastsize--;
        } else
        if(event.x >= 83 && event.x < 103 && selfield == 3) { wins[0].field = 7; ctrl_glyphs_onenter(); }
        if(event.x >= 117 && event.x < 138 && selfield == 4) { wins[0].field = 8; ctrl_glyphs_onenter(); wins[0].field = -1; }
        if(event.x >= 139 && event.x < 159 && selfield == 5) { wins[0].field = 9; ctrl_glyphs_onenter(); wins[0].field = -1; }
        if(event.x >= 172 && event.x < 192 && selfield == 6) { wins[0].field = 10; ctrl_glyphs_onenter(); wins[0].field = -1; }
        if(event.x >= 194 && event.x < 214 && selfield == 7) { wins[0].field = 11; ctrl_glyphs_onenter(); wins[0].field = -1; }
        if(event.x >= 216 && event.x < 236 && selfield == 8) { wins[0].field = 12; ctrl_glyphs_onenter(); wins[0].field = -1; }
        if(event.x >= 250 && event.x < 270 && selfield == 9) { wins[0].field = 13; ctrl_glyphs_onenter(); wins[0].field = -1; }
    } else
    if(event.y >= 52 && event.y < wins[0].h - 26 && selfield == 10 && selstart > -1 && selend == -1) ui_openwin(gres[selstart]);
    selfield = -1;
}

/**
 * On mouse move handler
 */
void ctrl_glyphs_onmove()
{
    int i;

    if(event.y > 29 && event.y < 49) {
        if(event.x >= 8 && event.x < 28) status = lang[GLYPHS_VECTORIZE]; else
        if(event.x >= 32 && event.x < 103) status = lang[GLYPHS_RASTERIZE]; else
        if(event.x >= 117 && event.x < 138) status = lang[GLYPHS_ZOOMOUT]; else
        if(event.x >= 139 && event.x < 159) status = lang[GLYPHS_ZOOMIN]; else
        if(event.x >= 172 && event.x < 192) status = lang[GLYPHS_CUT]; else
        if(event.x >= 194 && event.x < 214) status = lang[GLYPHS_COPY]; else
        if(event.x >= 216 && event.x < 236) status = lang[GLYPHS_PASTE]; else
        if(event.x >= 250 && event.x < 270) status = lang[GLYPHS_DELETE];
        glast = -1;
    } else
    if(event.y >= 52 && event.y < wins[0].h - 26) {
        i = ((event.y - 52) / gsize) * (1<<wins[0].zoom) + (event.x - ((wins[0].w - gsize * (1<<wins[0].zoom))/2 + 1))
            / gsize + scrollglyphs;
        if(i >= 0 && i < numglyphs) {
            status = gstat;
            if(gres[i] != glast) {
                if(selfield == 10 && selstart != -1) {
                    selend = i;
                    ui_refreshwin(0, 0, 0, wins[0].w, wins[0].h - 18);
                    sprintf(gstat, "%s U+%06x .. U+%06X", lang[GLYPHS_SELECT], gres[selstart], gres[selend]);
                } else {
                    ui_chrinfo(gres[i]);
                }
                glast = gres[i];
            }
        }
    } else
        glast = -1;
}
