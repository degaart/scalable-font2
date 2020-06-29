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
#include <stdlib.h>
#include <string.h>
#include "libsfn.h"
#include "ui.h"
#include "lang.h"

int mousex = -1, mousey = -1;

/**
 * Glyph coordinates and dimensions window
 */
void view_coords(int idx)
{
    ui_win_t *win = &wins[idx];
    int i = 0, j, x = win->w - 74;
    char *u, s[48];

    if(x < 0) x = 0;
    ssfn_dst.w = x - 6;
    ssfn_dst.h = win->h - 24;
    ui_grid(win, ctx.glyphs[win->unicode].width, ctx.glyphs[win->unicode].height);
    ui_gridbg(win, 20 + (win->zx > 0 ? win->zx : 0), 36 + (win->zy > 0 ? win->zy : 0),
        win->zoom * ctx.glyphs[win->unicode].width, win->zoom * ctx.glyphs[win->unicode].height, 1, win->data, -1, -1);
    ui_edit(win, 20 + win->zx, 36 + win->zy, win->unicode, -1);

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
 * Zoom in at mouse coordinates
 */
void ctrl_zoom_in(int idx, int mx, int my)
{
    ui_win_t *win = &wins[idx];
    if(win->zoom >= 64) { win->zoom = 64; return; }
    win->zoom++;
    if(posx == -1 || posy == -1) win->rc = 1;
    else {
        win->zx = mx - 20 - posx * win->zoom - win->zoom / 2;
        win->zy = my - 36 - posy * win->zoom - win->zoom / 2;
    }
    ui_resizewin(win, win->w, win->h);
    ui_refreshwin(idx, 0, 0, win->w, win->h);
}

/**
 * Zoom out at mouse coordinates
 */
void ctrl_zoom_out(int idx, int mx, int my)
{
    ui_win_t *win = &wins[idx];
    if(win->zoom <= 1) { win->zoom = 1; return; }
    win->zoom--;
    if(posx == -1 || posy == -1) win->rc = 1;
    else {
        win->zx = mx - 20 - posx * win->zoom - win->zoom / 2;
        win->zy = my - 36 - posy * win->zoom - win->zoom / 2;
    }
    ui_resizewin(win, win->w, win->h);
    ui_refreshwin(idx, 0, 0, win->w, win->h);
}

/**
 * Move glyph around
 */
void ctrl_move(int idx, int mx, int my)
{
    ui_win_t *win = &wins[idx];
    if(mousex == -1 && mousey == -1) return;
    win->zx += mx - mousex; mousex = mx;
    win->zy += my - mousey; mousey = my;
    cursor = CURSOR_MOVE;
    ui_resizewin(win, win->w, win->h);
    ui_refreshwin(event.win, 0, 0, win->w, win->h - 24);
}

/**
 * Set glyph's size
 */
void ctrl_setsize(int idx, int dw, int dh)
{
    ui_win_t *win = &wins[idx];
    sfncont_t *cont;
    unsigned char *data;
    int i, j, k, l, n, w = ctx.glyphs[win->unicode].width, h = ctx.glyphs[win->unicode].height;
    if(ctx.glyphs[win->unicode].width + dw >= 0 && ctx.glyphs[win->unicode].width + dw <= 254)
        ctx.glyphs[win->unicode].width += dw;
    if(ctx.glyphs[win->unicode].height + dh >= 0 && ctx.glyphs[win->unicode].height + dh <= 254)
        ctx.glyphs[win->unicode].height += dh;
    if(ctx.glyphs[win->unicode].width != w || ctx.glyphs[win->unicode].height != h) {
        for(n = 0; n < ctx.glyphs[win->unicode].numlayer; n++)
            switch(ctx.glyphs[win->unicode].layers[n].type) {
                case SSFN_FRAG_CONTOUR:
                    for(i = 0, cont = (sfncont_t*)ctx.glyphs[win->unicode].layers[n].data;
                        i < ctx.glyphs[win->unicode].layers[n].len; i++, cont++) {
                            if(cont->px >= ctx.glyphs[win->unicode].width) cont->px = ctx.glyphs[win->unicode].width - 1;
                            if(cont->py >= ctx.glyphs[win->unicode].height) cont->py = ctx.glyphs[win->unicode].height - 1;
                            if(cont->type >= SSFN_CONTOUR_QUAD) {
                                if(cont->c1x >= ctx.glyphs[win->unicode].width) cont->c1x = ctx.glyphs[win->unicode].width - 1;
                                if(cont->c1y >= ctx.glyphs[win->unicode].height) cont->c1y = ctx.glyphs[win->unicode].height - 1;
                                if(cont->type == SSFN_CONTOUR_CUBIC) {
                                    if(cont->c2x >= ctx.glyphs[win->unicode].width) cont->c2x = ctx.glyphs[win->unicode].width - 1;
                                    if(cont->c2y >= ctx.glyphs[win->unicode].height) cont->c2y = ctx.glyphs[win->unicode].height - 1;
                                }
                            }
                        }
                break;
                case SSFN_FRAG_BITMAP:
                case SSFN_FRAG_PIXMAP:
                    if(!dw) {
                        ctx.glyphs[win->unicode].layers[n].data = realloc(ctx.glyphs[win->unicode].layers[n].data,
                            w * ctx.glyphs[win->unicode].height);
                        if(!ctx.glyphs[win->unicode].layers[n].data) ui_error("coords", ERR_MEM);
                        if(dh > 0) memset(ctx.glyphs[win->unicode].layers[n].data + w * h, 0xFF, w);
                    } else {
                        data = malloc(ctx.glyphs[win->unicode].width * h);
                        if(!data) ui_error("coords", ERR_MEM);
                        for(j = k = l = 0; j < h; j++) {
                            for(i = 0; i < w + (dw < 0 ? dw : 0); i++)
                                data[k++] = ctx.glyphs[win->unicode].layers[n].data[l++];
                            if(dw < 0) l++; else data[k++] = 0xFF;
                        }
                        free(ctx.glyphs[win->unicode].layers[n].data);
                        ctx.glyphs[win->unicode].layers[n].data = data;
                    }
                break;
            }
    }
    ctx.width = ctx.height = 0;
    for(i = 0; i < 0x110000; i++) {
        if(ctx.glyphs[i].width > ctx.width) ctx.width = ctx.glyphs[i].width;
        if(ctx.glyphs[i].height > ctx.height) ctx.height = ctx.glyphs[i].height;
    }
    win->rc = 1; win->zoom = 0;
    modified++;
}

/**
 * Set glyph's baseline
 */
void ctrl_setbase(int idx, int d)
{
    (void)idx;
    if(d > 0 && ctx.baseline < 254) { ctx.baseline++; modified++; }
    if(d < 0 && ctx.baseline > 0) { ctx.baseline--; modified++; }
}

/**
 * Set glyph's underlineline
 */
void ctrl_setunder(int idx, int d)
{
    (void)idx;
    if(d > 0 && ctx.underline < 254) { ctx.underline++; modified++; }
    if(d < 0 && ctx.underline > 0) { ctx.underline--; modified++; }
}

/**
 * Set glyph's overlap x
 */
void ctrl_setox(int idx, int d)
{
    ui_win_t *win = &wins[idx];
    if(d > 0 && ctx.glyphs[win->unicode].ovl_x < 63) { ctx.glyphs[win->unicode].ovl_x++; modified++; }
    if(d < 0 && ctx.glyphs[win->unicode].ovl_x > 0) { ctx.glyphs[win->unicode].ovl_x--; modified++; }
}

/**
 * Set glyph's advance x
 */
void ctrl_setax(int idx, int d)
{
    ui_win_t *win = &wins[idx];
    ctx.glyphs[win->unicode].adv_y = 0;
    if(d > 0 && ctx.glyphs[win->unicode].adv_x < 254) { ctx.glyphs[win->unicode].adv_x++; modified++; }
    if(d < 0 && ctx.glyphs[win->unicode].adv_x > 0) { ctx.glyphs[win->unicode].adv_x--; modified++; }
}

/**
 * Set glyph's advance y
 */
void ctrl_setay(int idx, int d)
{
    ui_win_t *win = &wins[idx];
    ctx.glyphs[win->unicode].adv_x = 0;
    if(d > 0 && ctx.glyphs[win->unicode].adv_y < 254) { ctx.glyphs[win->unicode].adv_y++; modified++; }
    if(d < 0 && ctx.glyphs[win->unicode].adv_y > 0) { ctx.glyphs[win->unicode].adv_y--; modified++; }
}

/**
 * Set glyph's advance direction
 */
void ctrl_setadv(int idx, int d)
{
    ui_win_t *win = &wins[idx];
    switch(d) {
        case 0:
            ctx.glyphs[win->unicode].rtl = 1;
            ctx.glyphs[win->unicode].adv_y = 0;
            if(ctx.glyphs[win->unicode].adv_x < 1) ctx.glyphs[win->unicode].adv_x = ctx.glyphs[win->unicode].width;
        break;
        case 1:
            ctx.glyphs[win->unicode].rtl = 0;
            ctx.glyphs[win->unicode].adv_x = 0;
            if(ctx.glyphs[win->unicode].adv_y < 1) ctx.glyphs[win->unicode].adv_y = ctx.glyphs[win->unicode].height;
        break;
        case 2:
            ctx.glyphs[win->unicode].rtl = 0;
            ctx.glyphs[win->unicode].adv_y = 0;
            if(ctx.glyphs[win->unicode].adv_x < 1) ctx.glyphs[win->unicode].adv_x = ctx.glyphs[win->unicode].width;
        break;
    }
    modified++;
}

/**
 * Position glyph
 */
void ctrl_pos(int idx, int layer, int dx, int dy)
{
    ui_win_t *win = &wins[idx];
    sfncont_t *cont;
    unsigned char *data;
    int i, n, w = ctx.glyphs[win->unicode].width, h = ctx.glyphs[win->unicode].height, s, e;
    if(layer < 0 || layer >= ctx.glyphs[win->unicode].numlayer) {
        s = 0; e = ctx.glyphs[win->unicode].numlayer;
    } else {
        s = layer; e = layer + 1;
    }
    for(n = s; n < e; n++)
        switch(ctx.glyphs[win->unicode].layers[n].type) {
            case SSFN_FRAG_CONTOUR:
                for(i = 0, cont = (sfncont_t*)ctx.glyphs[win->unicode].layers[n].data;
                    i < ctx.glyphs[win->unicode].layers[n].len; i++, cont++) {
                        if(dx < 0 && cont->px > 0) cont->px--;
                        if(dx > 0 && cont->px < w - 1) cont->px++;
                        if(dy < 0 && cont->py > 0) cont->py--;
                        if(dy > 0 && cont->py < h - 1) cont->py++;
                        if(cont->type >= SSFN_CONTOUR_QUAD) {
                            if(dx < 0 && cont->c1x > 0) cont->c1x--;
                            if(dx > 0 && cont->c1x < w - 1) cont->c1x++;
                            if(dy < 0 && cont->c1y > 0) cont->c1y--;
                            if(dy > 0 && cont->c1y < h - 1) cont->c1y++;
                            if(cont->type == SSFN_CONTOUR_CUBIC) {
                                if(dx < 0 && cont->c2x > 0) cont->c2x--;
                                if(dx > 0 && cont->c2x < w - 1) cont->c2x++;
                                if(dy < 0 && cont->c2y > 0) cont->c2y--;
                                if(dy > 0 && cont->c2y < h - 1) cont->c2y++;
                            }
                        }
                    }
            break;
            case SSFN_FRAG_BITMAP:
            case SSFN_FRAG_PIXMAP:
                data = malloc(w * h);
                if(!data) ui_error("coords", ERR_MEM);
                memcpy(data, ctx.glyphs[win->unicode].layers[n].data, w * h);
                if(dy < 0) {
                    memcpy(ctx.glyphs[win->unicode].layers[n].data, data + w, w * (h - 1));
                    memcpy(ctx.glyphs[win->unicode].layers[n].data + w * (h - 1), data, w);
                } else
                if(dy > 0) {
                    memcpy(ctx.glyphs[win->unicode].layers[n].data + w, data, w * (h - 1));
                    memcpy(ctx.glyphs[win->unicode].layers[n].data, data + w * (h - 1), w);
                }
                if(dx < 0) {
                    for(i = 0; i < h; i++) {
                        memcpy(ctx.glyphs[win->unicode].layers[n].data + i * w, data + i * w + 1, w - 1);
                        ctx.glyphs[win->unicode].layers[n].data[i * w + w - 1] = data[i * w];
                    }
                } else
                if(dx > 0) {
                    for(i = 0; i < h; i++) {
                        memcpy(ctx.glyphs[win->unicode].layers[n].data + i * w + 1, data + i * w, w - 1);
                        ctx.glyphs[win->unicode].layers[n].data[i * w] = data[i * w + w - 1];
                    }
                }
                free(data);
            break;
        }
    modified++;
}

/**
 * Italize glyph
 */
void ctrl_italize(int idx)
{
    ui_win_t *win = &wins[idx];
    sfncont_t *cont;
    unsigned char *data;
    int i, n, mx, w = ctx.glyphs[win->unicode].width, h = ctx.glyphs[win->unicode].height;
    int s = h / SSFN_ITALIC_DIV;
    ctx.glyphs[win->unicode].width += s;
    mx = ctx.glyphs[win->unicode].width;
    for(n = 0; n < ctx.glyphs[win->unicode].numlayer; n++)
        switch(ctx.glyphs[win->unicode].layers[n].type) {
            case SSFN_FRAG_CONTOUR:
                for(i = 0, cont = (sfncont_t*)ctx.glyphs[win->unicode].layers[n].data;
                    i < ctx.glyphs[win->unicode].layers[n].len; i++, cont++) {
                        cont->px += s * (h - cont->py) / h;
                        if(cont->px < mx) mx = cont->px;
                        if(cont->type >= SSFN_CONTOUR_QUAD) {
                            cont->c1x += s * (h - cont->c1y) / h;
                            if(cont->c1x < mx) mx = cont->c1x;
                            if(cont->type == SSFN_CONTOUR_CUBIC) {
                                cont->c2x += s * (h - cont->c2y) / h;
                                if(cont->c2x < mx) mx = cont->c2x;
                            }
                        }
                    }
            break;
            case SSFN_FRAG_BITMAP:
            case SSFN_FRAG_PIXMAP:
                data = malloc(ctx.glyphs[win->unicode].width * h);
                if(!data) ui_error("coords", ERR_MEM);
                memset(data, 0xFF, ctx.glyphs[win->unicode].width * h);
                for(i = 0; i < h; i++)
                    memcpy(data + i * ctx.glyphs[win->unicode].width + s * (h - i) / h,
                        ctx.glyphs[win->unicode].layers[n].data + i * w, w);
                free(ctx.glyphs[win->unicode].layers[n].data);
                ctx.glyphs[win->unicode].layers[n].data = data;
            break;
        }
    for(n = 0; n < ctx.glyphs[win->unicode].numlayer; n++)
        if(ctx.glyphs[win->unicode].layers[n].type == SSFN_FRAG_CONTOUR)
            for(i = 0, cont = (sfncont_t*)ctx.glyphs[win->unicode].layers[n].data;
                i < ctx.glyphs[win->unicode].layers[n].len; i++, cont++) {
                    cont->px -= mx;
                    if(cont->type >= SSFN_CONTOUR_QUAD) {
                        cont->c1x -= mx;
                        if(cont->type == SSFN_CONTOUR_CUBIC)
                            cont->c2x -= mx;
                    }
                }
    sfn_sanitize(win->unicode);
    win->rc = 1;
    modified++;
}

/**
 * Deitalize glyph
 */
void ctrl_deitalize(int idx)
{
    ui_win_t *win = &wins[idx];
    sfncont_t *cont;
    unsigned char *data;
    int i, n, mx = 0, w = ctx.glyphs[win->unicode].width, h = ctx.glyphs[win->unicode].height;
    int s = h / SSFN_ITALIC_DIV;
    ctx.glyphs[win->unicode].width -= s;
    for(n = 0; n < ctx.glyphs[win->unicode].numlayer; n++)
        switch(ctx.glyphs[win->unicode].layers[n].type) {
            case SSFN_FRAG_CONTOUR:
                for(i = 0, cont = (sfncont_t*)ctx.glyphs[win->unicode].layers[n].data;
                    i < ctx.glyphs[win->unicode].layers[n].len; i++, cont++) {
                        if(mx > cont->px - s * (h - cont->py) / h) mx = cont->px - s * (h - cont->py) / h;
                        if(cont->type >= SSFN_CONTOUR_QUAD) {
                            if(mx > cont->c1x - s * (h - cont->c1y) / h) mx = cont->c1x - s * (h - cont->c1y) / h;
                            if(cont->type == SSFN_CONTOUR_CUBIC &&
                               mx > cont->c2x - s * (h - cont->c2y) / h) mx = cont->c2x - s * (h - cont->c2y) / h;
                        }
                    }
            break;
            case SSFN_FRAG_BITMAP:
            case SSFN_FRAG_PIXMAP:
                data = malloc(ctx.glyphs[win->unicode].width * h);
                if(!data) ui_error("coords", ERR_MEM);
                for(i = 0; i < h; i++)
                    memcpy(data + i * ctx.glyphs[win->unicode].width,
                        ctx.glyphs[win->unicode].layers[n].data + i * w + s * (h - i) / h, w);
                free(ctx.glyphs[win->unicode].layers[n].data);
                ctx.glyphs[win->unicode].layers[n].data = data;
            break;
        }
    for(n = 0; n < ctx.glyphs[win->unicode].numlayer; n++)
        if(ctx.glyphs[win->unicode].layers[n].type == SSFN_FRAG_CONTOUR)
                for(i = 0, cont = (sfncont_t*)ctx.glyphs[win->unicode].layers[n].data;
                    i < ctx.glyphs[win->unicode].layers[n].len; i++, cont++) {
                        cont->px = cont->px - mx - s * (h - cont->py) / h;
                        if(cont->type >= SSFN_CONTOUR_QUAD) {
                            cont->c1x = cont->c1x - mx - s * (h - cont->c1y) / h;
                            if(cont->type == SSFN_CONTOUR_CUBIC)
                                cont->c2x = cont->c2x - mx - s * (h - cont->c2y) / h;
                        }
                    }
    sfn_sanitize(win->unicode);
    win->rc = 1;
    modified++;
}

/**
 * Flip glyph horizontally
 */
void ctrl_fliph(int idx, int layer)
{
    ui_win_t *win = &wins[idx];
    sfncont_t *cont;
    unsigned char d, *data;
    int i, j, k, n, w = ctx.glyphs[win->unicode].width, h = ctx.glyphs[win->unicode].height, s, e;
    if(layer < 0 || layer >= ctx.glyphs[win->unicode].numlayer) {
        s = 0; e = ctx.glyphs[win->unicode].numlayer;
    } else {
        s = layer; e = layer + 1;
    }
    for(n = s; n < e; n++)
        switch(ctx.glyphs[win->unicode].layers[n].type) {
            case SSFN_FRAG_CONTOUR:
                for(i = 0, cont = (sfncont_t*)ctx.glyphs[win->unicode].layers[n].data;
                    i < ctx.glyphs[win->unicode].layers[n].len; i++, cont++) {
                        cont->px = w - 1 - cont->px;
                        if(cont->type >= SSFN_CONTOUR_QUAD) {
                            cont->c1x = w - 1 - cont->c1x;
                            if(cont->type == SSFN_CONTOUR_CUBIC)
                                cont->c2x = w - 1 - cont->c2x;
                        }
                    }
            break;
            case SSFN_FRAG_BITMAP:
            case SSFN_FRAG_PIXMAP:
                data = ctx.glyphs[win->unicode].layers[n].data;
                for(j = k = 0; j < h; j++, k += w)
                    for(i = 0; i < w/2; i++) {
                        d = data[k + i];
                        data[k + i] = data[k + w - 1 - i];
                        data[k + w - 1 - i] = d;
                    }
            break;
        }
    modified++;
}

/**
 * Flip glyph vertically
 */
void ctrl_flipv(int idx, int layer)
{
    ui_win_t *win = &wins[idx];
    sfncont_t *cont;
    unsigned char d, *data;
    int i, j, n, w = ctx.glyphs[win->unicode].width, h = ctx.glyphs[win->unicode].height, s, e;
    if(layer < 0 || layer >= ctx.glyphs[win->unicode].numlayer) {
        s = 0; e = ctx.glyphs[win->unicode].numlayer;
    } else {
        s = layer; e = layer + 1;
    }
    for(n = s; n < e; n++)
        switch(ctx.glyphs[win->unicode].layers[n].type) {
            case SSFN_FRAG_CONTOUR:
                for(i = 0, cont = (sfncont_t*)ctx.glyphs[win->unicode].layers[n].data;
                    i < ctx.glyphs[win->unicode].layers[n].len; i++, cont++) {
                        cont->py = h - 1 - cont->py;
                        if(cont->type >= SSFN_CONTOUR_QUAD) {
                            cont->c1y = h - 1 - cont->c1y;
                            if(cont->type == SSFN_CONTOUR_CUBIC)
                                cont->c2y = h - 1 - cont->c2y;
                        }
                    }
            break;
            case SSFN_FRAG_BITMAP:
            case SSFN_FRAG_PIXMAP:
                data = ctx.glyphs[win->unicode].layers[n].data;
                for(j = 0; j < h/2; j++)
                    for(i = 0; i < w; i++) {
                        d = data[j*w + i];
                        data[j*w + i] = data[(h-1-j)*w + i];
                        data[(h-1-j)*w + i] = d;
                    }
            break;
        }
    modified++;
}

/**
 * On enter handler
 */
void ctrl_coords_onenter(int idx)
{
    ui_win_t *win = &wins[idx];
    switch(win->field) {
        case 11: ctrl_setadv(idx, 0); break;
        case 12: ctrl_setadv(idx, 1); break;
        case 13: ctrl_setadv(idx, 2); break;
        case 14: ctrl_pos(idx,-1, 0,-1); break;
        case 15: ctrl_pos(idx,-1,-1, 0); break;
        case 16: ctrl_pos(idx,-1, 1, 0); break;
        case 17: ctrl_pos(idx,-1, 0, 1); break;
        case 18: ctrl_italize(idx); break;
        case 19: ctrl_fliph(idx, -1); break;
        case 20: sfn_sanitize(win->unicode); win->rc = 1; modified++; break;
        case 21: ctrl_deitalize(idx); break;
        case 22: ctrl_flipv(idx, -1); break;
        case 23: sfn_chardel(win->unicode); win->rc = 1; modified++; break;
    }
}

/**
 * On key handler
 */
void ctrl_coords_onkey(int idx)
{
    ui_win_t *win = &wins[idx];
    switch(event.x) {
        case K_UP:
            switch(win->field) {
                case 4: ctrl_setsize(idx, 1, 0); break;
                case 5: ctrl_setsize(idx, 0, 1); break;
                case 6: ctrl_setbase(idx, 1); break;
                case 7: ctrl_setunder(idx, 1); break;
                case 8: ctrl_setox(idx, 1); break;
                case 9: ctrl_setax(idx, 1); break;
                case 10: ctrl_setay(idx, 1); break;
                default: ctrl_pos(idx,-1, 0, -1); break;
            }
        break;
        case K_LEFT: ctrl_pos(idx,-1, -1, 0); break;
        case K_DOWN:
            switch(win->field) {
                case 4: ctrl_setsize(idx, -1, 0); break;
                case 5: ctrl_setsize(idx, 0, -1); break;
                case 6: ctrl_setbase(idx, -1); break;
                case 7: ctrl_setunder(idx, -1); break;
                case 8: ctrl_setox(idx, -1); break;
                case 9: ctrl_setax(idx, -1); break;
                case 10: ctrl_setay(idx, -1); break;
                default: ctrl_pos(idx,-1, 0, 1); break;
            }
        break;
        case K_RIGHT: ctrl_pos(idx,-1, 1, 0); break;
        case 'h': case 'H': ctrl_fliph(idx, -1); break;
        case 'v': case 'V': ctrl_flipv(idx, -1); break;
    }
    ui_resizewin(win, win->w, win->h);
    ui_refreshwin(event.win, 0, 0, win->w, win->h - 24);
}

/**
 * On button press handler
 */
void ctrl_coords_onbtnpress(int idx)
{
    ui_win_t *win = &wins[idx];
    int x = win->w - 74, ox = win->zx > 0 ? win->zx : 0, oy = win->zy > 0 ? win->zy : 0;
    if(x < 0) x = 0;
    selfield = win->field = mousex = mousey = -1;
    if(event.x > x) {
        if(event.y > 26 && event.y < 48) {
            if(event.w & (1 << 3)) ctrl_setsize(idx, 1, 0); else
            if(event.w & (1 << 4)) ctrl_setsize(idx,-1, 0); else
            if(event.x >= x + 58) selfield = 0 + (event.y - 26 > 12 ? 1 : 0); else win->field = 4;
        } else
        if(event.y > 52 && event.y < 70) {
            if(event.w & (1 << 3)) ctrl_setsize(idx, 0, 1); else
            if(event.w & (1 << 4)) ctrl_setsize(idx, 0,-1); else
            if(event.x >= x + 58) selfield = 2 + (event.y - 52 > 9 ? 1 : 0); else win->field = 5;
        } else
        if(event.y > 74 && event.y < 92) {
            if(event.w & (1 << 3)) ctrl_setbase(idx, 1); else
            if(event.w & (1 << 4)) ctrl_setbase(idx,-1); else
            if(event.x >= x + 58) selfield = 4 + (event.y - 74 > 9 ? 1 : 0); else win->field = 6;
        } else
        if(event.y > 96 && event.y < 114) {
            if(event.w & (1 << 3)) ctrl_setunder(idx, 1); else
            if(event.w & (1 << 4)) ctrl_setunder(idx,-1); else
            if(event.x >= x + 58) selfield = 6 + (event.y - 96 > 9 ? 1 : 0); else win->field = 7;
        } else
        if(event.y > 140 && event.y < 158) {
            if(event.w & (1 << 3)) ctrl_setox(idx, 1); else
            if(event.w & (1 << 4)) ctrl_setox(idx,-1); else
            if(event.x >= x + 58) selfield = 8 + (event.y - 140 > 9 ? 1 : 0); else win->field = 8;
        } else
        if(event.y > 162 && event.y < 180) {
            if(event.w & (1 << 3)) ctrl_setax(idx, 1); else
            if(event.w & (1 << 4)) ctrl_setax(idx,-1); else
            if(event.x >= x + 58) selfield = 10 + (event.y - 162 > 9 ? 1 : 0); else win->field = 9;
        } else
        if(event.y > 184 && event.y < 204) {
            if(event.w & (1 << 3)) ctrl_setay(idx, 1); else
            if(event.w & (1 << 4)) ctrl_setay(idx,-1); else
            if(event.x >= x + 58) selfield = 12 + (event.y - 184 > 9 ? 1 : 0); else win->field = 10;
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
        ui_resizewin(win, win->w, win->h);
        ui_refreshwin(event.win, 0, 0, win->w, win->h - 24);
    } else
    if(event.y < win->h - 22) {
        if(event.w & (1 << 3)) ctrl_zoom_in(event.win, event.x, event.y); else
        if(event.w & (1 << 4)) ctrl_zoom_out(event.win, event.x, event.y); else
        if(event.x >= ox + 20 && event.y >= oy + 36 &&
            event.x <= ox + 20 + win->zoom * ctx.glyphs[win->unicode].width &&
            event.y <= oy + 36 + win->zoom * ctx.glyphs[win->unicode].height) {
                mousex = event.x; mousey = event.y;
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
            if(selfield == 4) ctrl_setbase(idx, 1); else
            if(selfield == 5) ctrl_setbase(idx,-1);
        } else
        if(event.y > 96 && event.y < 114 && event.x >= x + 58) {
            if(selfield == 6) ctrl_setunder(idx, 1); else
            if(selfield == 7) ctrl_setunder(idx,-1);
        } else
        if(event.y > 140 && event.y < 158 && event.x >= x + 58) {
            if(selfield == 8) ctrl_setox(idx, 1); else
            if(selfield == 9) ctrl_setox(idx,-1);
        } else
        if(event.y > 162 && event.y < 180 && event.x >= x + 58) {
            if(selfield == 10) ctrl_setax(idx, 1); else
            if(selfield == 11) ctrl_setax(idx,-1);
        } else
        if(event.y > 184 && event.y < 204 && event.x >= x + 58) {
            if(selfield == 12) ctrl_setay(idx, 1); else
            if(selfield == 13) ctrl_setay(idx,-1);
        } else
        if(event.y > 206 && event.y < 228) {
            if(event.x > x && event.x < x + 24 && selfield == 14) ctrl_setadv(idx, 0); else
            if(event.x > x + 24 && event.x < x + 48 && selfield == 15) ctrl_setadv(idx, 1); else
            if(event.x > x + 48 && event.x < x + 72 && selfield == 16) ctrl_setadv(idx, 2);
        } else
        if(event.y > 250 && event.y < 272 && event.x > x + 24 && event.x < x + 48 && selfield == 17) ctrl_pos(idx,-1, 0, -1); else
        if(event.y > 274 && event.y < 296) {
            if(event.x > x && event.x < x + 24 && selfield == 18) ctrl_pos(idx,-1, -1, 0); else
            if(event.x > x + 48 && event.x < x + 72 && selfield == 19) ctrl_pos(idx,-1, 1, 0);
        } else
        if(event.y > 298 && event.y < 340 && event.x > x + 24 && event.x < x + 48 && selfield == 20) ctrl_pos(idx,-1, 0, 1); else
        if(event.y > 342 && event.y < 364) {
            if(event.x > x && event.x < x + 24 && selfield == 21) ctrl_italize(idx); else
            if(event.x > x + 24 && event.x < x + 48 && selfield == 22) ctrl_fliph(idx, -1); else
            if(event.x > x + 48 && event.x < x + 72 && selfield == 23) { sfn_sanitize(win->unicode); win->rc = 1; modified++; }
        } else
        if(event.y > 366 && event.y < 388) {
            if(event.x > x && event.x < x + 24 && selfield == 24) ctrl_deitalize(idx); else
            if(event.x > x + 24 && event.x < x + 48 && selfield == 25) ctrl_flipv(idx, -1); else
            if(event.x > x + 48 && event.x < x + 72 && selfield == 26) { sfn_chardel(win->unicode); win->rc = 1; modified++; }
        }
    }
    cursor = CURSOR_PTR;
    selfield = mousex = mousey = -1;
}

/**
 * On mouse move handler
 */
void ctrl_coords_onmove(int idx)
{
    ui_win_t *win = &wins[idx];
    int x = win->w - 74, ox = win->zx > 0 ? win->zx : 0, oy = win->zy > 0 ? win->zy : 0;
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
            if(event.x > x + 24 && event.x < x + 48) status = lang[COORDS_UTD]; else
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
    } else
    if(event.x >= ox + 20 && event.y >= oy + 36 &&
        event.x <= ox + 20 + win->zoom * ctx.glyphs[win->unicode].width &&
        event.y <= oy + 36 + win->zoom * ctx.glyphs[win->unicode].height && event.y < win->h - 22) {
            posx = (event.x - ox - 20 - (win->zx < 0 ? win->zx : 0)) / win->zoom;
            if(posx >= ctx.glyphs[win->unicode].width) posx = -1;
            posy = (event.y - oy - 36 - (win->zy < 0 ? win->zy : 0)) / win->zoom;
            if(posy >= ctx.glyphs[win->unicode].height) posy = -1;
            if(mousex != -1 && mousey != -1) ctrl_move(event.win, event.x, event.y);
    }
}
