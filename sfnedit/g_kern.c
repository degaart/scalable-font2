/*
 * sfnedit/g_kern.c
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
 * @brief Glyph window kerning tool
 *
 */

#include <stdint.h>
#include <stdio.h>
#include "libsfn.h"
#include "ui.h"
#include "lang.h"

char ksearch[32] = { 0 };
int selkern = 0;

/**
 * Kerning window
 */
void view_kern(int idx)
{
    ui_win_t *win = &wins[idx];
    int i, x = win->w - 74;
    char st[8];
    uint32_t c;

    if(x < 0) x = 0;
    ssfn_dst.w = win->w - 1;

    ui_icon(win, win->w - 130 - 16, 4, ICON_SEARCH, 0);
    ui_input(win, win->w - 128, 2, 120, ksearch, win->field == 4, 31, 0);

    ui_box(win, x, win->h - 42, 22, 22, theme[win->field == 6 ? THEME_CURSOR : (selfield == 0 ? THEME_DARKER : THEME_LIGHT)],
        theme[win->field == 6 ? THEME_LIGHT : THEME_BG],
        theme[win->field == 6 ? THEME_CURSOR : (selfield == 0 ? THEME_LIGHT : THEME_DARKER)]);
    ui_icon(win, x+3, win->h - 39, ICON_RHORIZ, ctx.glyphs[win->unicode].adv_y > 0);

    ui_box(win, x+24, win->h - 42, 22, 22, theme[win->field == 7 ? THEME_CURSOR : (selfield == 1 ? THEME_DARKER : THEME_LIGHT)],
        theme[win->field == 7 ? THEME_LIGHT : THEME_BG],
        theme[win->field == 7 ? THEME_CURSOR : (selfield == 1 ? THEME_LIGHT : THEME_DARKER)]);
    ui_icon(win, x+27, win->h - 39, ICON_HORIZ, ctx.glyphs[win->unicode].adv_y > 0);

    ui_box(win, x+48, win->h - 42, 22, 22, theme[win->field == 8 ? THEME_CURSOR : (selfield == 2 ? THEME_DARKER : THEME_LIGHT)],
        theme[win->field == 8 ? THEME_LIGHT : THEME_BG],
        theme[win->field == 8 ? THEME_CURSOR : (selfield == 2 ? THEME_LIGHT : THEME_DARKER)]);
    ui_icon(win, x+51, win->h - 39, ICON_DELETE, 0);

    if(selkern >= ctx.glyphs[win->unicode].numkern) selkern = ctx.glyphs[win->unicode].numkern - 1;
    if(selkern < 0) selkern = 0;

    ui_box(win, x, 26, 70, win->h - 26 - 44, theme[win->field == 5 ? THEME_CURSOR : THEME_DARKER], theme[THEME_BG],
        theme[win->field == 5 ? THEME_CURSOR : THEME_LIGHT]);

    ssfn_dst.w = x + 68; ssfn_dst.h = win->h - 47; ssfn_dst.y = 29; x += 3;
    for(i = 0; i < ctx.glyphs[win->unicode].numkern && ssfn_dst.y < ssfn_dst.h; i++, ssfn_dst.y += 11) {
        if(i == win->layer) {
            c = theme[THEME_SELBG];
            ssfn_dst.fg = theme[THEME_SELFG];
        } else {
            c = theme[THEME_DARK];
            ssfn_dst.fg = theme[THEME_FG];
        }
        ui_box(win, x, ssfn_dst.y, 64, 20, c, c, c);
        ssfn_dst.x = x + 2; ssfn_dst.y++;
        ssfn_putc(ctx.glyphs[win->unicode].kern[i].n);
        sprintf(st, "%06X", ctx.glyphs[win->unicode].kern[i].n);
        ssfn_dst.x = x + 22; ssfn_dst.y += 3;
        ui_hex(win, st[0]); ui_hex(win, st[1]); ui_hex(win, st[2]);
        if(ctx.glyphs[win->unicode].kern[i].x || ctx.glyphs[win->unicode].kern[i].y) {
            ssfn_dst.x = x + 40;
            ui_hex(win, 'X'); ui_number(win, x + 46, ssfn_dst.y, ctx.glyphs[win->unicode].kern[i].x, theme[THEME_FG]);
        }
        ssfn_dst.x = x + 22; ssfn_dst.y += 7;
        ui_hex(win, st[3]); ui_hex(win, st[4]); ui_hex(win, st[5]);
        if(ctx.glyphs[win->unicode].kern[i].x || ctx.glyphs[win->unicode].kern[i].y) {
            ssfn_dst.x = x + 40;
            ui_hex(win, 'Y'); ui_number(win, x + 46, ssfn_dst.y, ctx.glyphs[win->unicode].kern[i].y, theme[THEME_FG]);
        }
    }
}

/**
 * On button press handler
 */
void ctrl_kern_onbtnpress(int idx)
{
    ui_win_t *win = &wins[idx];
    int x = win->w - 74;
    if(x < 0) x = 0;
    selfield = win->field = -1;
    if(event.y < 26 && event.x > win->w - 130 - 16) win->field = 4;
    if(event.y > win->h - 42 && event.x > x) {
        if(event.x > x && event.x < x + 24) selfield = 0; else
        if(event.x > x + 24 && event.x < x + 48) selfield = 1; else
        if(event.x > x + 48 && event.x < x + 72) selfield = 2;
    }
}

/**
 * On click (button release) handler
 */
void ctrl_kern_onclick(int idx)
{
    ui_win_t *win = &wins[idx];
    int x = win->w - 74;
    if(x < 0) x = 0;
    if(event.y < 26 && event.x > win->w - 130 - 16) win->field = 4;
    if(event.y > win->h - 42 && event.x > x) {
        if(event.x > x && event.x < x + 24) selfield = 0; else
        if(event.x > x + 24 && event.x < x + 48) selfield = 1; else
        if(event.x > x + 48 && event.x < x + 72) selfield = 2;
    }
    selfield = -1;
}

/**
 * On mouse move handler
 */
void ctrl_kern_onmove(int idx)
{
    ui_win_t *win = &wins[idx];
    int x = win->w - 74;
    if(x < 0) x = 0;
    if(event.y > win->h - 42 && event.x > x) {
        if(event.x > x && event.x < x + 24) status = lang[COORDS_RTL]; else
        if(event.x > x + 24 && event.x < x + 48) status = lang[COORDS_LTR]; else
        if(event.x > x + 48 && event.x < x + 72) status = lang[KERN_DELETE];
    }
}
