/*
 * sfnedit/g_layers.c
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
 * @brief Glyph window edit layers tool
 *
 */

#include <stdint.h>
#include <stdio.h>
#include "libsfn.h"
#include "ui.h"
#include "lang.h"

extern int colorsel;
int sellayers = 0, scrolllayers = 0, pagelayers = 0;

/**
 * Layer editor window
 */
void view_layers(int idx)
{
    ui_win_t *win = &wins[idx];
    int i, j, x = win->w - 74;
    uint32_t c;

    if(x < 0) x = 0;
    ssfn_dst.w = x - 2;
    ssfn_dst.h = win->h - 22;
    ui_grid(win, ctx.glyphs[win->unicode].width, ctx.glyphs[win->unicode].height);
    ui_gridbg(win, 20 + win->ox, 36 + win->oy, win->zoom * ctx.glyphs[win->unicode].width,
        win->zoom * ctx.glyphs[win->unicode].height, win->zoom, win->p, win->data);

    ssfn_dst.w = win->w - 1;
    ssfn_dst.h = win->h - 20;

    if(sellayers >= ctx.glyphs[win->unicode].numlayer) sellayers = ctx.glyphs[win->unicode].numlayer - 1;
    if(sellayers < 0) sellayers = 0;
    pagelayers = (win->h - 26 - 44) / 66; if(pagelayers < 1) pagelayers = 1;
    if(scrolllayers + pagelayers > ctx.glyphs[win->unicode].numlayer) scrolllayers = ctx.glyphs[win->unicode].numlayer-pagelayers;
    if(scrolllayers < 0) scrolllayers = 0;
    if(sellayers >= ctx.glyphs[win->unicode].numlayer)
        sellayers = 0;
    if(sellayers < ctx.glyphs[win->unicode].numlayer)
        colorsel = ctx.glyphs[win->unicode].layers[sellayers].color;
    else colorsel = 0xFE;
    c = colorsel < ctx.numcpal ? *((uint32_t*)&ctx.cpal[colorsel << 2]) : (colorsel == 0xFF ? 0 : 0xFF000000 | theme[THEME_FG]);

    for(i = 0; i < 2; i++) {
        j = win->field == 4 + i;
        ui_box(win, 144 + i * 24, 1, 22, 22, theme[j ? THEME_CURSOR : (selfield == i ? THEME_DARKER : THEME_LIGHT)],
          theme[j ? THEME_LIGHT : THEME_BG], theme[j ? THEME_CURSOR : (selfield == i ? THEME_LIGHT : THEME_DARKER)]);
        ui_icon(win, 147 + i * 24, 4, ICON_ZOOMOUT + i, 0);
    }
    for(i = 0; i < 3; i++) {
        j = win->field == 6 + i;
        ui_box(win, 198 + i * 24, 1, 22, 22, theme[j ? THEME_CURSOR : (selfield==i+2 ? THEME_DARKER : THEME_LIGHT)],
          theme[j ? THEME_LIGHT : THEME_BG], theme[j ? THEME_CURSOR : (selfield==i+2 ? THEME_LIGHT : THEME_DARKER)]);
        ui_icon(win, 201 + i * 24, 4, ICON_CUT + i, 0);
    }
    for(i = 0; i < 3; i++) {
        j = win->field == 9 + i;
        ui_box(win,  x + i * 24, 1, 22, 22,  theme[j ? THEME_CURSOR : (selfield==i+5 ? THEME_DARKER : THEME_LIGHT)],
          theme[j ? THEME_LIGHT : THEME_BG], theme[j ? THEME_CURSOR : (selfield==i+5 ? THEME_LIGHT : THEME_DARKER)]);
        ui_icon(win, x + 3 + i * 24, 4, ICON_VECTOR + i, 0);
    }

    ui_box(win, x, win->h - 42, 22, 22, theme[win->field == 13 ? THEME_CURSOR : (selfield == 8 ? THEME_DARKER : THEME_LIGHT)],
        theme[win->field == 13 ? THEME_LIGHT : THEME_BG],
        theme[win->field == 13 ? THEME_CURSOR : (selfield == 8 ? THEME_LIGHT : THEME_DARKER)]);
    ui_argb(win, x + 3, win->h - 39, 16, 16, sellayers < ctx.glyphs[win->unicode].numlayer ? c : 0);

    ui_box(win, x+24, win->h - 42, 22, 22, theme[win->field == 14 ? THEME_CURSOR : (selfield == 9 ? THEME_DARKER : THEME_LIGHT)],
        theme[win->field == 14 ? THEME_LIGHT : THEME_BG],
        theme[win->field == 14 ? THEME_CURSOR : (selfield == 9 ? THEME_LIGHT : THEME_DARKER)]);
    ui_icon(win, x+27, win->h - 39, ICON_ERASE, sellayers >= ctx.glyphs[win->unicode].numlayer);

    ui_box(win, x+48, win->h - 42, 22, 22, theme[win->field == 15 ? THEME_CURSOR : (selfield == 10 ? THEME_DARKER : THEME_LIGHT)],
        theme[win->field == 15 ? THEME_LIGHT : THEME_BG],
        theme[win->field == 15 ? THEME_CURSOR : (selfield == 10 ? THEME_LIGHT : THEME_DARKER)]);
    ui_icon(win, x+51, win->h - 39, ICON_DELETE, sellayers >= ctx.glyphs[win->unicode].numlayer);

    ui_box(win, x, 26, 70, win->h - 26 - 44, theme[win->field == 12 ? THEME_CURSOR : THEME_DARKER], theme[THEME_BG],
        theme[win->field == 12 ? THEME_CURSOR : THEME_LIGHT]);
    ssfn_dst.w = x + 68; ssfn_dst.h = win->h - 47; j = 29; x += 3;
    for(i = scrolllayers; i < ctx.glyphs[win->unicode].numlayer && j < ssfn_dst.h; i++, j += 66) {
        if(i == sellayers) {
            c = theme[THEME_SELBG];
            ssfn_dst.fg = theme[THEME_SELFG];
        } else {
            c = theme[THEME_DARK];
            ssfn_dst.fg = theme[THEME_FG];
        }
        ui_box(win, x, j, 64, 64, c, c, c);
        ui_icon(win, x, j+48, ctx.glyphs[win->unicode].layers[i].type + ICON_VECTOR, 2);
        ui_glyph(win, x, j, 64, win->unicode, i);
    }
    ssfn_dst.fg = theme[THEME_FG];
    ssfn_dst.bg = 0;
}

/**
 * On enter handler
 */
void ctrl_layers_onenter(int idx)
{
    ui_win_t *win = &wins[idx];
    switch(win->field) {
    }
}

/**
 * On key handler
 */
void ctrl_layers_onkey(int idx)
{
    ui_win_t *win = &wins[idx];
}

/**
 * On button press handler
 */
void ctrl_layers_onbtnpress(int idx)
{
    ui_win_t *win = &wins[idx];
    int x = win->w - 74;
    if(x < 0) x = 0;
    selfield = -1;
    if(event.y < 26) {
        if(event.x > x && event.x < x + 24) selfield = 5; else
        if(event.x > x + 24 && event.x < x + 48) selfield = 6; else
        if(event.x > x + 48 && event.x < x + 64) selfield = 7; else
        if(event.x > 144 && event.x < 166) selfield = 0; else
        if(event.x > 168 && event.x < 190) selfield = 1; else
        if(event.x > 198 && event.x < 220) selfield = 2; else
        if(event.x > 224 && event.x < 246) selfield = 3; else
        if(event.x > 248 && event.x < 270) selfield = 4;
    } else
    if(event.y > 26 && event.y < win->h - 42 && event.x > x && event.x < x + 70) {
        if(event.w & 1) sellayers = (event.y - 28) / 64 + scrolllayers; else
        if(event.w & (1 << 3)) scrolllayers--; else
        if(event.w & (1 << 4)) scrolllayers++;
    } else
    if(event.y > win->h - 42 && event.x > x && sellayers < ctx.glyphs[win->unicode].numlayer) {
        if(event.x > x && event.x < x + 24) selfield = 8; else
        if(event.x > x + 24 && event.x < x + 48) selfield = 9; else
        if(event.x > x + 48 && event.x < x + 72) selfield = 10;
    }
}

/**
 * On click (button release) handler
 */
void ctrl_layers_onclick(int idx)
{
    ui_win_t *win = &wins[idx];
    int x = win->w - 74;
    if(x < 0) x = 0;
    if(event.y < 26) {
        if(event.x > x && event.x < x + 24) selfield = 5; else
        if(event.x > x + 24 && event.x < x + 48) selfield = 6; else
        if(event.x > x + 48 && event.x < x + 64) selfield = 7; else
        if(event.x > 144 && event.x < 166) selfield = 0; else
        if(event.x > 168 && event.x < 190) selfield = 1; else
        if(event.x > 198 && event.x < 220) selfield = 2; else
        if(event.x > 224 && event.x < 246) selfield = 3; else
        if(event.x > 248 && event.x < 270) selfield = 4;
    } else
    if(event.y > win->h - 42 && event.x > x) {
        if(event.x > x && event.x < x + 24 && selfield == 8) win->tool = GLYPH_TOOL_COLOR; else
        if(event.x > x + 24 && event.x < x + 48 && selfield == 9) { colorsel = 0xFF; ctrl_colors_onenter(idx); } else
        if(event.x > x + 48 && event.x < x + 72) selfield = 10;
        win->field = -1;
    }
    selfield = -1;
}

/**
 * On mouse move handler
 */
void ctrl_layers_onmove(int idx)
{
    ui_win_t *win = &wins[idx];
    int x = win->w - 74;
    if(x < 0) x = 0;
    posx = posy = -1;
    if(event.y < 26) {
        if(event.x > x && event.x < x + 24) status = lang[LAYERS_VECTOR]; else
        if(event.x > x + 24 && event.x < x + 48) status = lang[LAYERS_BITMAP]; else
        if(event.x > x + 48 && event.x < x + 64) status = lang[LAYERS_PIXMAP]; else
        if(event.x > 144 && event.x < 166) status = lang[LAYERS_ZOOMOUT]; else
        if(event.x > 168 && event.x < 190) status = lang[LAYERS_ZOOMIN]; else
        if(event.x > 198 && event.x < 220) status = lang[LAYERS_CUT]; else
        if(event.x > 224 && event.x < 246) status = lang[LAYERS_COPY]; else
        if(event.x > 248 && event.x < 270) status = lang[LAYERS_PASTE];
    } else
    if(event.y > win->h - 42 && event.x > x) {
        if(event.x > x && event.x < x + 24) status = lang[LAYERS_FOREGROUND]; else
        if(event.x > x + 24 && event.x < x + 48) status = lang[LAYERS_BACKGROUND]; else
        if(event.x > x + 48 && event.x < x + 72) status = lang[LAYERS_DELETE];
    } else {
        posx = event.x - 16; posy = event.y - 26 - 8;
    }
}
