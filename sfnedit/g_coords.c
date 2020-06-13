/*
 * sfnedit/g_coords.c
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
 * @brief Glyph window measure coordinates tool
 *
 */

#include <stdint.h>
#include <stdio.h>
#include "libsfn.h"
#include "ui.h"
#include "lang.h"

/**
 * Glyph coordinates and dimensions window
 */
void view_coords(int idx)
{
    ui_win_t *win = &wins[idx];
    int i = 0, j, x = win->w - 74;
    char *u, s[48];

    if(x < 0) x = 0;
    ssfn_dst.w = x - 2;
    ssfn_dst.h = win->h - 22;
    ui_grid(win, ctx.glyphs[win->unicode].width, ctx.glyphs[win->unicode].height);
    ui_gridbg(win, 20 + win->ox, 36 + win->oy, win->zoom * ctx.glyphs[win->unicode].width,
        win->zoom * ctx.glyphs[win->unicode].height, win->zoom, win->p, win->data);

    ssfn_dst.w = win->w - 1;
    ssfn_dst.h = win->h - 20;

    u = utf8(win->unicode);
    if(!u[1]) u[2] = u[3] = 0; else if(!u[2]) u[3] = 0;
    sprintf(s, "U+%06X", win->unicode);
    ssfn_dst.fg = theme[THEME_FG]; ui_text(win, 144, 4, s);
    ssfn_dst.fg = theme[THEME_LIGHTER]; ui_text(win, 144+80, 4, "UTF-8");
    sprintf(s, "%02x %02x %02x %02x", (unsigned char)u[0], (unsigned char)u[1], (unsigned char)u[2], (unsigned char)u[3]);
    ssfn_dst.fg = theme[THEME_FG]; ui_text(win, 144+80+6*8, 4, s);
    ssfn_dst.fg = theme[THEME_LIGHTER]; ui_text(win, 144+80+6*8+12*8, 4, "DEC");
    sprintf(s, "%d", win->unicode);
    ssfn_dst.fg = theme[THEME_FG]; ui_text(win, 144+80+6*8+12*8+4*8, 4, s);

    ui_box(win, x,  30, 10, 10, theme[THEME_DARKER], theme[THEME_DIM], theme[THEME_LIGHTER]);
    ui_num(win, x+18,  26, ctx.glyphs[win->unicode].width, win->field == 4, selfield);

    ui_box(win, x,  52, 10, 10, theme[THEME_DARKER], theme[THEME_DIM], theme[THEME_LIGHTER]);
    ui_num(win, x+18,  48, ctx.glyphs[win->unicode].height, win->field == 5, selfield - 2);

    ui_box(win, x,  74, 10, 10, theme[THEME_DARKER], theme[THEME_BASE], theme[THEME_LIGHTER]);
    ui_num(win, x+18,  70, ctx.baseline, win->field == 6, selfield - 4);

    ui_box(win, x,  96, 10, 10, theme[THEME_DARKER], theme[THEME_UNDER], theme[THEME_LIGHTER]);
    ui_num(win, x+18,  92, ctx.underline, win->field == 7, selfield - 6);

    ui_box(win, x, 140, 10, 10, theme[THEME_DARKER], theme[THEME_OVL], theme[THEME_LIGHTER]);
    ui_num(win, x+18, 136, ctx.glyphs[win->unicode].ovl_x, win->field == 8, selfield - 8);

    ui_box(win, x, 162, 10, 10, theme[THEME_DARKER], theme[THEME_ADV], theme[THEME_LIGHTER]);
    ui_num(win, x+18, 158, ctx.glyphs[win->unicode].adv_x, win->field == 9, selfield - 10);

    ui_box(win, x, 184, 10, 10, theme[THEME_DARKER], theme[THEME_ADV], theme[THEME_LIGHTER]);
    ui_num(win, x+18, 180, ctx.glyphs[win->unicode].adv_y, win->field == 10, selfield - 12);

    for(i = 0; i < 3; i++) {
        j = win->field == 11 + i;
        ui_box(win, x + i * 24, 206, 22, 22, theme[j ? THEME_CURSOR : (selfield==i+14 ? THEME_DARKER : THEME_LIGHT)],
          theme[j ? THEME_LIGHT : THEME_BG], theme[j ? THEME_CURSOR : (selfield==i+14 ? THEME_LIGHT : THEME_DARKER)]);
        ui_icon(win, x + 3 + i * 24, 209, ICON_RHORIZ + i, 0);
    }

    j = win->field == 14;
    ui_box(win, x + 24, 250, 22, 22, theme[j ? THEME_CURSOR : (selfield==17 ? THEME_DARKER : THEME_LIGHT)],
      theme[j ? THEME_LIGHT : THEME_BG], theme[j ? THEME_CURSOR : (selfield==17 ? THEME_LIGHT : THEME_DARKER)]);
    ui_icon(win, x + 3 + 24, 253, ICON_UARR, 0);

    j = win->field == 15;
    ui_box(win, x, 274, 22, 22, theme[j ? THEME_CURSOR : (selfield==18 ? THEME_DARKER : THEME_LIGHT)],
      theme[j ? THEME_LIGHT : THEME_BG], theme[j ? THEME_CURSOR : (selfield==18 ? THEME_LIGHT : THEME_DARKER)]);
    ui_icon(win, x + 3, 277, ICON_LARR, 0);

    j = win->field == 16;
    ui_box(win, x + 48, 274, 22, 22, theme[j ? THEME_CURSOR : (selfield==19 ? THEME_DARKER : THEME_LIGHT)],
      theme[j ? THEME_LIGHT : THEME_BG], theme[j ? THEME_CURSOR : (selfield==19 ? THEME_LIGHT : THEME_DARKER)]);
    ui_icon(win, x + 3 + 48, 277, ICON_RARR, 0);

    j = win->field == 17;
    ui_box(win, x + 24, 298, 22, 22, theme[j ? THEME_CURSOR : (selfield==20 ? THEME_DARKER : THEME_LIGHT)],
      theme[j ? THEME_LIGHT : THEME_BG], theme[j ? THEME_CURSOR : (selfield==20 ? THEME_LIGHT : THEME_DARKER)]);
    ui_icon(win, x + 3 + 24, 301, ICON_DARR, 0);


    j = win->field == 18;
    ui_box(win, x, 342, 22, 22, theme[j ? THEME_CURSOR : (selfield==21 ? THEME_DARKER : THEME_LIGHT)],
      theme[j ? THEME_LIGHT : THEME_BG], theme[j ? THEME_CURSOR : (selfield==21 ? THEME_LIGHT : THEME_DARKER)]);
    ui_icon(win, x + 3, 345, ICON_SHEAR, 0);

    j = win->field == 19;
    ui_box(win, x + 24, 342, 22, 22, theme[j ? THEME_CURSOR : (selfield==22 ? THEME_DARKER : THEME_LIGHT)],
      theme[j ? THEME_LIGHT : THEME_BG], theme[j ? THEME_CURSOR : (selfield==22 ? THEME_LIGHT : THEME_DARKER)]);
    ui_icon(win, x + 27, 345, ICON_HFLIP, 0);

    j = win->field == 20;
    ui_box(win, x + 48, 342, 22, 22, theme[j ? THEME_CURSOR : (selfield==23 ? THEME_DARKER : THEME_LIGHT)],
      theme[j ? THEME_LIGHT : THEME_BG], theme[j ? THEME_CURSOR : (selfield==23 ? THEME_LIGHT : THEME_DARKER)]);
    ui_icon(win, x + 51, 345, ICON_MEASURES, 0);

    j = win->field == 21;
    ui_box(win, x, 366, 22, 22, theme[j ? THEME_CURSOR : (selfield==24 ? THEME_DARKER : THEME_LIGHT)],
      theme[j ? THEME_LIGHT : THEME_BG], theme[j ? THEME_CURSOR : (selfield==24 ? THEME_LIGHT : THEME_DARKER)]);
    ui_icon(win, x + 3, 369, ICON_UNSHEAR, 0);

    j = win->field == 22;
    ui_box(win, x + 24, 366, 22, 22, theme[j ? THEME_CURSOR : (selfield==25 ? THEME_DARKER : THEME_LIGHT)],
      theme[j ? THEME_LIGHT : THEME_BG], theme[j ? THEME_CURSOR : (selfield==25 ? THEME_LIGHT : THEME_DARKER)]);
    ui_icon(win, x + 27, 369, ICON_VFLIP, 0);

    j = win->field == 23;
    ui_box(win, x + 48, 366, 22, 22, theme[j ? THEME_CURSOR : (selfield==26 ? THEME_DARKER : THEME_LIGHT)],
      theme[j ? THEME_LIGHT : THEME_BG], theme[j ? THEME_CURSOR : (selfield==26 ? THEME_LIGHT : THEME_DARKER)]);
    ui_icon(win, x + 51, 369, ICON_DELETE, 0);

}

/**
 * Set glyph's size
 */
void ctrl_setsize(int idx, int dw, int dh)
{
    ui_win_t *win = &wins[idx];
    int w = ctx.glyphs[win->unicode].width, h = ctx.glyphs[win->unicode].height;
    if(ctx.glyphs[win->unicode].width + dw > 0 && ctx.glyphs[win->unicode].width + dw < 254)
        ctx.glyphs[win->unicode].width += dw;
    if(ctx.glyphs[win->unicode].height + dh > 0 && ctx.glyphs[win->unicode].height + dh < 254)
        ctx.glyphs[win->unicode].height += dh;
    if(ctx.glyphs[win->unicode].width != w || ctx.glyphs[win->unicode].height != h) {
    }
}

/**
 * Position glyph
 */
void ctrl_pos(int idx, int dx, int dy)
{
    ui_win_t *win = &wins[idx];
}

/**
 * On enter handler
 */
void ctrl_coords_onenter(int idx)
{
    ui_win_t *win = &wins[idx];
    switch(win->field) {
    }
}

/**
 * On key handler
 */
void ctrl_coords_onkey(int idx)
{
    ui_win_t *win = &wins[idx];
}

/**
 * On button press handler
 */
void ctrl_coords_onbtnpress(int idx)
{
    ui_win_t *win = &wins[idx];
    int x = win->w - 74;
    if(x < 0) x = 0;
    selfield = -1;
    if(event.x > x) {
        if(event.y > 26 && event.y < 48) {
            if(event.w & (1 << 3)) ctrl_setsize(idx, 1, 0); else
            if(event.w & (1 << 4)) ctrl_setsize(idx,-1, 0); else
            if(event.x >= x + 58) selfield = 0 + (event.y - 26 > 12 ? 1 : 0);
        } else
        if(event.y > 52 && event.y < 70) {
            if(event.w & (1 << 3)) ctrl_setsize(idx, 0, 1); else
            if(event.w & (1 << 4)) ctrl_setsize(idx, 0,-1); else
            if(event.x >= x + 58) selfield = 2 + (event.y - 52 > 9 ? 1 : 0);
        } else
        if(event.y > 74 && event.y < 92) {
            if(event.w & (1 << 3)) { if(ctx.baseline < 254) ctx.baseline++; } else
            if(event.w & (1 << 4)) { if(ctx.baseline > 0) ctx.baseline--; } else
            if(event.x >= x + 58) selfield = 4 + (event.y - 74 > 9 ? 1 : 0);
        } else
        if(event.y > 96 && event.y < 114) {
            if(event.w & (1 << 3)) { if(ctx.underline < 254) ctx.underline++; } else
            if(event.w & (1 << 4)) { if(ctx.underline > 0) ctx.underline--; } else
            if(event.x >= x + 58) selfield = 6 + (event.y - 96 > 9 ? 1 : 0);
        } else
        if(event.y > 140 && event.y < 158) {
            if(event.w & (1 << 3)) { if(ctx.glyphs[win->unicode].ovl_x < 63) ctx.glyphs[win->unicode].ovl_x++; } else
            if(event.w & (1 << 4)) { if(ctx.glyphs[win->unicode].ovl_x > 0) ctx.glyphs[win->unicode].ovl_x--; } else
            if(event.x >= x + 58) selfield = 8 + (event.y - 140 > 9 ? 1 : 0);
        } else
        if(event.y > 162 && event.y < 180) {
            if(event.w & (1 << 3)) { if(ctx.glyphs[win->unicode].adv_x < 254) ctx.glyphs[win->unicode].adv_x++; } else
            if(event.w & (1 << 4)) { if(ctx.glyphs[win->unicode].adv_x > 0) ctx.glyphs[win->unicode].adv_x--; } else
            if(event.x >= x + 58) selfield = 10 + (event.y - 162 > 9 ? 1 : 0);
        } else
        if(event.y > 184 && event.y < 204) {
            if(event.w & (1 << 3)) { if(ctx.glyphs[win->unicode].adv_y < 254) ctx.glyphs[win->unicode].adv_y++; } else
            if(event.w & (1 << 4)) { if(ctx.glyphs[win->unicode].adv_y > 0) ctx.glyphs[win->unicode].adv_y--; } else
            if(event.x >= x + 58) selfield = 12 + (event.y - 184 > 9 ? 1 : 0);
        } else
        if(event.y > 206 && event.y < 228) {
            if(event.x > x && event.x < x + 24) selfield = 14; else
            if(event.x > x + 24 && event.x < x + 48) selfield = 15; else
            if(event.x > x + 48 && event.x < x + 72) selfield = 16;
        } else
        if(event.y > 250 && event.y < 272 && event.x > x + 24 && event.x < x + 48) selfield = 17; else
        if(event.y > 274 && event.y < 296) {
            if(event.x > x && event.x < x + 24) selfield = 18; else
            if(event.x > x + 48 && event.x < x + 72) selfield = 19;
        } else
        if(event.y > 298 && event.y < 340 && event.x > x + 24 && event.x < x + 48) selfield = 20; else
        if(event.y > 342 && event.y < 364) {
            if(event.x > x && event.x < x + 24) selfield = 21; else
            if(event.x > x + 24 && event.x < x + 48) selfield = 22; else
            if(event.x > x + 48 && event.x < x + 72) selfield = 23;
        } else
        if(event.y > 366 && event.y < 388) {
            if(event.x > x && event.x < x + 24) selfield = 24; else
            if(event.x > x + 24 && event.x < x + 48) selfield = 25; else
            if(event.x > x + 48 && event.x < x + 72) selfield = 26;
        }
    }
}

/**
 * On click (button release) handler
 */
void ctrl_coords_onclick(int idx)
{
    ui_win_t *win = &wins[idx];
    int x = win->w - 74;
    if(x < 0) x = 0;
    if(event.x > x) {
        if(event.y > 26 && event.y < 48 && event.x >= x + 58) {
            if(selfield == 0) ctrl_setsize(idx, 1, 0); else
            if(selfield == 1) ctrl_setsize(idx,-1, 0);
        } else
        if(event.y > 52 && event.y < 70 && event.x >= x + 58) {
            if(selfield == 2) ctrl_setsize(idx, 0, 1); else
            if(selfield == 3) ctrl_setsize(idx, 0,-1);
        } else
        if(event.y > 74 && event.y < 92 && event.x >= x + 58) {
            if(selfield == 4) { if(ctx.baseline < 254) ctx.baseline++; } else
            if(selfield == 5) { if(ctx.baseline > 0) ctx.baseline--; }
        } else
        if(event.y > 96 && event.y < 114 && event.x >= x + 58) {
            if(selfield == 6) { if(ctx.underline < 254) ctx.underline++; } else
            if(selfield == 7) { if(ctx.underline > 0) ctx.underline--; }
        } else
        if(event.y > 140 && event.y < 158 && event.x >= x + 58) {
            if(selfield == 8) { if(ctx.glyphs[win->unicode].ovl_x < 63) ctx.glyphs[win->unicode].ovl_x++; } else
            if(selfield == 9) { if(ctx.glyphs[win->unicode].ovl_x > 0) ctx.glyphs[win->unicode].ovl_x--; }
        } else
        if(event.y > 162 && event.y < 180 && event.x >= x + 58) {
            if(selfield == 10) { if(ctx.glyphs[win->unicode].adv_x < 254) ctx.glyphs[win->unicode].adv_x++; } else
            if(selfield == 11) { if(ctx.glyphs[win->unicode].adv_x > 0) ctx.glyphs[win->unicode].adv_x--; }
            ctx.glyphs[win->unicode].adv_y = 0;
        } else
        if(event.y > 184 && event.y < 204 && event.x >= x + 58) {
            if(selfield == 12) { if(ctx.glyphs[win->unicode].adv_y < 254) ctx.glyphs[win->unicode].adv_y++; } else
            if(selfield == 13) { if(ctx.glyphs[win->unicode].adv_y > 0) ctx.glyphs[win->unicode].adv_y--; }
            ctx.glyphs[win->unicode].adv_x = 0;
        } else
        if(event.y > 206 && event.y < 228) {
            if(event.x > x && event.x < x + 24 && selfield == 14) {
                ctx.glyphs[win->unicode].rtl = 1;
                ctx.glyphs[win->unicode].adv_y = 0;
                if(ctx.glyphs[win->unicode].adv_x < 1) ctx.glyphs[win->unicode].adv_x = ctx.glyphs[win->unicode].width;
            } else
            if(event.x > x + 24 && event.x < x + 48 && selfield == 15) {
                ctx.glyphs[win->unicode].adv_x = 0;
                if(ctx.glyphs[win->unicode].adv_y < 1) ctx.glyphs[win->unicode].adv_y = ctx.glyphs[win->unicode].height;
            } else
            if(event.x > x + 48 && event.x < x + 72 && selfield == 16) {
                ctx.glyphs[win->unicode].rtl = 0;
                ctx.glyphs[win->unicode].adv_y = 0;
                if(ctx.glyphs[win->unicode].adv_x < 1) ctx.glyphs[win->unicode].adv_x = ctx.glyphs[win->unicode].width;
            }
        } else
        if(event.y > 250 && event.y < 272 && event.x > x + 24 && event.x < x + 48 && selfield == 17) ctrl_pos(idx, 0, -1); else
        if(event.y > 274 && event.y < 296) {
            if(event.x > x && event.x < x + 24 && selfield == 18) ctrl_pos(idx, -1, 0); else
            if(event.x > x + 48 && event.x < x + 72 && selfield == 19) ctrl_pos(idx, 1, 0);
        } else
        if(event.y > 298 && event.y < 340 && event.x > x + 24 && event.x < x + 48 && selfield == 20) ctrl_pos(idx, 0, 1); else
        if(event.y > 342 && event.y < 364) {
            if(event.x > x && event.x < x + 24 && selfield == 21) {} else
            if(event.x > x + 24 && event.x < x + 48 && selfield == 22) {} else
            if(event.x > x + 48 && event.x < x + 72 && selfield == 23) {}
        } else
        if(event.y > 366 && event.y < 388) {
            if(event.x > x && event.x < x + 24 && selfield == 24) {} else
            if(event.x > x + 24 && event.x < x + 48 && selfield == 25) {} else
            if(event.x > x + 48 && event.x < x + 72 && selfield == 26) sfn_chardel(win->unicode);
        }
    }
    selfield = -1;
}

/**
 * On mouse move handler
 */
void ctrl_coords_onmove(int idx)
{
    ui_win_t *win = &wins[idx];
    int x = win->w - 74;
    if(x < 0) x = 0;
    posx = posy = -1;
    if(event.x > x) {
        if(event.y > 26 && event.y < 48) status = lang[COORDS_WIDTH]; else
        if(event.y > 52 && event.y < 70) status = lang[COORDS_HEIGHT]; else
        if(event.y > 74 && event.y < 92) status = lang[COORDS_BASELINE]; else
        if(event.y > 96 && event.y < 114) status = lang[COORDS_UNDERLINE]; else
        if(event.y > 140 && event.y < 158) status = lang[COORDS_OVERLAP]; else
        if(event.y > 162 && event.y < 180) status = lang[COORDS_HADV]; else
        if(event.y > 184 && event.y < 204) status = lang[COORDS_VADV]; else
        if(event.y > 206 && event.y < 228) {
            if(event.x > x && event.x < x + 24) status = lang[COORDS_RTL]; else
            if(event.x > x + 24 && event.x < x + 48) status = lang[COORDS_HORIZ]; else
            if(event.x > x + 48 && event.x < x + 72) status = lang[COORDS_LTR];
        } else
        if(event.y > 250 && event.y < 320) status = lang[COORDS_REPOS]; else
        if(event.y > 342 && event.y < 364) {
            if(event.x > x && event.x < x + 24) status = lang[COORDS_ITALIC]; else
            if(event.x > x + 24 && event.x < x + 48) status = lang[COORDS_HFLIP]; else
            if(event.x > x + 48 && event.x < x + 72) status = lang[COORDS_RECALC];
        } else
        if(event.y > 366 && event.y < 388) {
            if(event.x > x && event.x < x + 24) status = lang[COORDS_UNITALIC]; else
            if(event.x > x + 24 && event.x < x + 48) status = lang[COORDS_VFLIP]; else
            if(event.x > x + 48 && event.x < x + 72) status = lang[COORDS_DELETE];
        }
    } else {
        posx = event.x - 16; posy = event.y - 26 - 8;
    }
}
