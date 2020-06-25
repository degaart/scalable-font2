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
#include <string.h>
#include "libsfn.h"
#include "ui.h"
#include "lang.h"

char ksearch[32] = { 0 };
int selkern = 0, scrollkern = 0, pagekern = 0;
extern int mousex, mousey, isclick;
void ctrl_zoom_in(int idx, int mx, int my);
void ctrl_zoom_out(int idx, int mx, int my);
void ctrl_move(int idx, int mx, int my);

/**
 * Kerning window
 */
void view_kern(int idx)
{
    ui_win_t *win = &wins[idx];
    int i, x = win->w - 74, ox = win->zx > 0 ? win->zx : 0, oy = win->zy > 0 ? win->zy : 0;
    int px = win->zx < 0 ? -win->zx : 0, py = win->zy < 0 ? -win->zy : 0;
    char st[8];
    uint32_t c;

    if(x < 0) x = 0;
    ssfn_dst.w = x - 6;
    ssfn_dst.h = win->h - 24;
    ui_grid(win, ctx.glyphs[win->unicode].width, ctx.glyphs[win->unicode].height);
    if(!ctx.glyphs[win->unicode].adv_y) {
        ui_gridbg(win, 20 + ox + (ctx.glyphs[win->unicode].rtl ? -1 : 1)* (win->zoom * ctx.glyphs[win->unicode].width) - px,
            36 + oy - py, win->zoom * ctx.width, win->zoom * ctx.height, win->zoom, 0, win->p, win->data);
    } else if(!ctx.glyphs[win->unicode].adv_x) {
        ui_gridbg(win, 20 + ox - px, 36 + oy + win->zoom * ctx.glyphs[win->unicode].height - py,
            win->zoom * ctx.width, win->zoom * ctx.height, win->zoom, 0, win->p, win->data);
    }
    ui_gridbg(win, 20 + ox, 36 + oy,
        win->zoom * ctx.glyphs[win->unicode].width, win->zoom * ctx.glyphs[win->unicode].height, win->zoom, 1, win->p, win->data);
    ssfn_dst.w = win->w - 1;
    ssfn_dst.h = win->h - 20;

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
    ui_icon(win, x+51, win->h - 39, ICON_DELETE, ctx.glyphs[win->unicode].numkern < 1);

    if(selkern >= ctx.glyphs[win->unicode].numkern) selkern = ctx.glyphs[win->unicode].numkern - 1;
    if(selkern < 0) selkern = 0;

    ui_box(win, x, 26, 70, win->h - 26 - 44, theme[win->field == 5 ? THEME_CURSOR : THEME_DARKER], theme[THEME_BG],
        theme[win->field == 5 ? THEME_CURSOR : THEME_LIGHT]);
    if(selkern >= ctx.glyphs[win->unicode].numkern) selkern = ctx.glyphs[win->unicode].numkern - 1;
    if(selkern < 0) selkern = 0;
    pagekern = (win->h - 47 - 29) / 22; if(pagekern < 1) pagekern = 1;
    if(scrollkern + pagekern > ctx.glyphs[win->unicode].numkern) scrollkern = ctx.glyphs[win->unicode].numkern-pagekern;
    if(scrollkern < 0) scrollkern = 0;
    ssfn_dst.w = x + 68; ssfn_dst.h = win->h - 47; ssfn_dst.y = 29; x += 3;
    for(i = scrollkern; i < ctx.glyphs[win->unicode].numkern && ssfn_dst.y < ssfn_dst.h; i++, ssfn_dst.y += 11) {
        if(i == selkern) {
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
 * On enter handler
 */
void ctrl_kern_onenter(int idx)
{
    ui_win_t *win = &wins[idx];
    switch(win->field) {
        case 6: ctx.glyphs[win->unicode].rtl = 1; break;
        case 7: ctx.glyphs[win->unicode].rtl = 0; break;
        case 8:
            memcpy(&ctx.glyphs[win->unicode].kern[selkern], &ctx.glyphs[win->unicode].kern[selkern+1],
                (ctx.glyphs[win->unicode].numkern - selkern) * sizeof(sfnkern_t));
            ctx.glyphs[win->unicode].numkern--;
        break;
    }
}

/**
 * On key handler
 */
void ctrl_kern_onkey(int idx)
{
    ui_win_t *win = &wins[idx];
}

/**
 * On button press handler
 */
void ctrl_kern_onbtnpress(int idx)
{
    ui_win_t *win = &wins[idx];
    int x = win->w - 74, ox = win->zx > 0 ? win->zx : 0, oy = win->zy > 0 ? win->zy : 0;
    if(x < 0) x = 0;
    selfield = win->field = mousex = mousey = -1;
    if(event.y < 26 && event.x > win->w - 130 - 16) win->field = 4; else
    if(event.x > x) {
        if(event.y > 26 && event.y < win->h - 44 && event.x > x && event.x < x + 70) {
            if(event.w & 1) selkern = (event.y - 31) / 22 + scrollkern; else
            if(event.w & (1 << 3)) scrollkern--; else
            if(event.w & (1 << 4)) scrollkern++;
        } else
        if(event.y > win->h - 42 && event.x > x) {
            if(event.x > x && event.x < x + 24 && ctx.glyphs[win->unicode].adv_y < 1) selfield = 0; else
            if(event.x > x + 24 && event.x < x + 48 && ctx.glyphs[win->unicode].adv_y < 1) selfield = 1; else
            if(event.x > x + 48 && event.x < x + 72 && ctx.glyphs[win->unicode].numkern) selfield = 2;
        }
    } else
    if(event.y < win->h - 22) {
        if(event.w & (1 << 3)) ctrl_zoom_in(event.win, event.x, event.y); else
        if(event.w & (1 << 4)) ctrl_zoom_out(event.win, event.x, event.y); else
        if(event.x >= ox + 20 && event.y >= oy + 36 &&
            event.x <= ox + 20 + win->zoom * ctx.glyphs[win->unicode].width &&
            event.y <= oy + 36 + win->zoom * ctx.glyphs[win->unicode].height) {
                mousex = event.x; mousey = event.y;
                isclick = 1;
        }
    }
}

/**
 * On click (button release) handler
 */
void ctrl_kern_onclick(int idx)
{
    ui_win_t *win = &wins[idx];
    int x = win->w - 74, ox = win->zx > 0 ? win->zx : 0, oy = win->zy > 0 ? win->zy : 0;
    if(x < 0) x = 0;
    if(event.y < 26 && event.x > win->w - 130 - 16) win->field = 4; else
    if(event.x > x) {
        if(event.y > win->h - 42 && event.x > x) {
            if(event.x > x && event.x < x + 24 && selfield == 0) { win->field = 6; ctrl_kern_onenter(idx); } else
            if(event.x > x + 24 && event.x < x + 48 && selfield == 1) { win->field = 7; ctrl_kern_onenter(idx); } else
            if(event.x > x + 48 && event.x < x + 72 && ctx.glyphs[win->unicode].numkern && selfield == 2) {
                win->field = 8; ctrl_kern_onenter(idx);
            }
            win->field = -1;
        }
    } else
    if(isclick && event.x >= ox + 20 && event.y >= oy + 36 &&
        event.x <= ox + 20 + win->zoom * ctx.glyphs[win->unicode].width &&
        event.y <= oy + 36 + win->zoom * ctx.glyphs[win->unicode].height && posx != -1 && posy != -1) {
            printf("click\n");
    }
    cursor = CURSOR_PTR; isclick = 0;
    selfield = mousex = mousey = -1;
}

/**
 * On mouse move handler
 */
void ctrl_kern_onmove(int idx)
{
    ui_win_t *win = &wins[idx];
    int x = win->w - 74, ox = win->zx > 0 ? win->zx : 0, oy = win->zy > 0 ? win->zy : 0;
    if(x < 0) x = 0;
    posx = posy = -1; isclick = 0;
    if(event.x > x) {
        if(event.y > win->h - 42) {
            if(event.x > x && event.x < x + 24) status = lang[COORDS_RTL]; else
            if(event.x > x + 24 && event.x < x + 48) status = lang[COORDS_LTR]; else
            if(event.x > x + 48 && event.x < x + 72) status = lang[KERN_DELETE];
        }
    } else
    if(event.x >= ox + 20 && event.y >= oy + 36 &&
        event.x <= ox + 20 + win->zoom * ctx.glyphs[win->unicode].width &&
        event.y <= oy + 36 + win->zoom * ctx.glyphs[win->unicode].height && event.y < win->h - 22) {
            if(mousex != -1 && mousey != -1) ctrl_move(event.win, event.x, event.y);
            posx = (event.x - ox - 20 - (win->zx < 0 ? win->zx : 0)) / win->zoom;
            if(posx >= ctx.glyphs[win->unicode].width) posx = -1;
            posy = (event.y - oy - 36 - (win->zy < 0 ? win->zy : 0)) / win->zoom;
            if(posy >= ctx.glyphs[win->unicode].height) posy = -1;
    }
}
