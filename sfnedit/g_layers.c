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
#include <stdlib.h>
#include <string.h>
#include "libsfn.h"
#include "ui.h"
#include "lang.h"

extern int colorsel, mousex, mousey;
int sellayers = 0, scrolllayers = 0, pagelayers = 0, isclick = 0, issel = 0, ispicker = 0, selcmd = -1, isfrc = 0, cx, cy;
void ctrl_zoom_in(int idx, int mx, int my);
void ctrl_zoom_out(int idx, int mx, int my);
void ctrl_move(int idx, int mx, int my);
void ctrl_pos(int idx, int layer, int dx, int dy);
void ctrl_fliph(int idx, int layer);
void ctrl_flipv(int idx, int layer);

/**
 * Layer editor window
 */
void view_layers(int idx)
{
    ui_win_t *win = &wins[idx];
    sfncont_t *cont;
    int i, j, k, x = win->w - 74, w = win->zoom * ctx.glyphs[win->unicode].width, h = win->zoom * ctx.glyphs[win->unicode].height;
    int gx = -1, gy = -1, d;
    uint32_t c, *uc = (uint32_t*)&ctx.cpal;

    if(x < 0) x = 0;
    pagelayers = (win->h - 29 - 49) / 66; if(pagelayers < 1) pagelayers = 1;
    if(sellayers >= ctx.glyphs[win->unicode].numlayer) sellayers = ctx.glyphs[win->unicode].numlayer - 1;
    if(sellayers < 0) sellayers = 0;
    if(scrolllayers + pagelayers > ctx.glyphs[win->unicode].numlayer) scrolllayers = ctx.glyphs[win->unicode].numlayer-pagelayers;
    if(sellayers + 1 > scrolllayers + pagelayers - 1) scrolllayers = sellayers - pagelayers + 1;
    if(sellayers >= 0 && sellayers < scrolllayers) scrolllayers = sellayers;
    if(scrolllayers < 0) scrolllayers = 0;
    if(sellayers >= ctx.glyphs[win->unicode].numlayer)
        sellayers = 0;
    ssfn_dst.w = x - 6;
    ssfn_dst.h = win->h - 24;
    i = win->zoom/2; if(i < 2) i = 2;
    ui_box(win, 20+win->zx-i, 36, i, win->h - 36, theme[THEME_BG],theme[THEME_BG],theme[THEME_BG]);
    ui_box(win, 20+win->zx+w, 36, i, win->h - 36, theme[THEME_BG],theme[THEME_BG],theme[THEME_BG]);
    if(win->zy-i >= 0)
        ui_box(win, 20, 36+win->zy-i, x - 20, i, theme[THEME_BG],theme[THEME_BG],theme[THEME_BG]);
    if(win->zy+h < win->h)
        ui_box(win, 20, 36+win->zy+h, x - 20, i, theme[THEME_BG],theme[THEME_BG],theme[THEME_BG]);
    ui_grid(win, ctx.glyphs[win->unicode].width, ctx.glyphs[win->unicode].height);
    if(selcmd != -1 && issel && posx != -1 && posy != -1 && sellayers < ctx.glyphs[win->unicode].numlayer) {
        cont = (sfncont_t*)ctx.glyphs[win->unicode].layers[sellayers].data;
        for(i = 0; i < ctx.glyphs[win->unicode].layers[sellayers].len && (gx == -1 || gy == -1); i++, cont++)
            if(i != (selcmd >> 2)) {
                if(cont->px == posx) gx = posx;
                if(cont->py == posy) gy = posy;
                if(cont->type >= SSFN_CONTOUR_QUAD) {
                    if(cont->c1x == posx) gx = posx;
                    if(cont->c1y == posy) gy = posy;
                    if(cont->type == SSFN_CONTOUR_CUBIC) {
                        if(cont->c2x == posx) gx = posx;
                        if(cont->c2y == posy) gy = posy;
                    }
                }
            }
    }
    ui_gridbg(win, 20 + (win->zx > 0 ? win->zx : 0), 36 + (win->zy > 0 ? win->zy : 0), w, h, 1, win->data, gx, gy);
    ui_edit(win, 20 + win->zx, 36 + win->zy, win->unicode, sellayers);
    ssfn_dst.w = win->w - 1;
    ssfn_dst.h = win->h - 20;

    if(sellayers < ctx.glyphs[win->unicode].numlayer)
        colorsel = ctx.glyphs[win->unicode].layers[sellayers].color;
    else colorsel = 0xFE;
    c = colorsel < ctx.numcpal ? uc[colorsel] : (colorsel == 0xFF ? 0 : 0xFF000000 | theme[THEME_FG]);

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
    ui_icon(win, x+27, win->h - 39, ICON_PICKER, sellayers >= ctx.glyphs[win->unicode].numlayer ||
        ctx.glyphs[win->unicode].layers[sellayers].type != SSFN_FRAG_PIXMAP);

    ui_box(win, x+48, win->h - 42, 22, 22, theme[win->field == 15 ? THEME_CURSOR : (selfield == 10 ? THEME_DARKER : THEME_LIGHT)],
        theme[win->field == 15 ? THEME_LIGHT : THEME_BG],
        theme[win->field == 15 ? THEME_CURSOR : (selfield == 10 ? THEME_LIGHT : THEME_DARKER)]);
    ui_icon(win, x+51, win->h - 39, ICON_DELETE, sellayers >= ctx.glyphs[win->unicode].numlayer);

    ui_box(win, x, 26, 70, win->h - 26 - 46, theme[win->field == 12 ? THEME_CURSOR : THEME_DARKER], theme[THEME_BG],
        theme[win->field == 12 ? THEME_CURSOR : THEME_LIGHT]);
    ssfn_dst.w = x + 68; ssfn_dst.h = win->h - 49; j = 29; x += 3;
    for(i = scrolllayers; i < ctx.glyphs[win->unicode].numlayer && j < ssfn_dst.h; i++, j += 66) {
        if(i == sellayers) {
            c = theme[THEME_SELBG]; d = -2;
            ssfn_dst.fg = theme[THEME_SELFG];
        } else {
            c = theme[THEME_DARK]; d = -1;
            ssfn_dst.fg = theme[THEME_FG];
        }
        ui_box(win, x, j, 64, 64, c, c, c);
        ui_icon(win, x, j+48, ctx.glyphs[win->unicode].layers[i].type + ICON_VECTOR, d);
        if(ctx.glyphs[win->unicode].layers[i].color == 0xFF) { k=1; ctx.glyphs[win->unicode].layers[i].color = 0xFE; } else k=0;
        ui_glyph(win, x, j, 64, win->unicode, i);
        if(k) ctx.glyphs[win->unicode].layers[i].color = 0xFF;
    }
    ssfn_dst.fg = theme[THEME_FG];
    ssfn_dst.bg = 0;
}

/**
 * Add a new empty layer
 */
void ctrl_layers_add(int idx, int t)
{
    int l;
    ui_win_t *win = &wins[idx];
    sfnlayer_t *lyr;
    ctx.glyphs[win->unicode].layers = (sfnlayer_t*)realloc(ctx.glyphs[win->unicode].layers,
        (ctx.glyphs[win->unicode].numlayer + 1) * sizeof(sfnlayer_t));
    if(!ctx.glyphs[win->unicode].layers) ui_error("layers", ERR_MEM);
    sellayers = ctx.glyphs[win->unicode].numlayer;
    lyr = &ctx.glyphs[win->unicode].layers[ctx.glyphs[win->unicode].numlayer++];
    memset(lyr, 0, sizeof(sfnlayer_t));
    lyr->type = t;
    lyr->color = 0xFE;
    if(t != SSFN_FRAG_CONTOUR) {
        l = ctx.glyphs[win->unicode].width * ctx.glyphs[win->unicode].height;
        lyr->data = (unsigned char*)malloc(l);
        if(!lyr->data) ui_error("layers", ERR_MEM);
        memset(lyr->data, 0xFF, l);
    }
    modified++;
}

/**
 * Delete the last contour command from active layer
 */
void ctrl_layers_delcmd(int idx)
{
    ui_win_t *win = &wins[idx];
    if(sellayers >= 0 && sellayers < ctx.glyphs[win->unicode].numlayer &&
        ctx.glyphs[win->unicode].layers[sellayers].type == SSFN_FRAG_CONTOUR  && ctx.glyphs[win->unicode].layers[sellayers].len)
            ctx.glyphs[win->unicode].layers[sellayers].len--;
}

/**
 * Add a contour command to layer
 */
void ctrl_layers_addcmd(int idx, int t, int x, int y)
{
    ui_win_t *win = &wins[idx];
    int c1x = 0, c1y = 0, c2x = 0, c2y = 0, i, px, py;
    if(sellayers >= 0 && sellayers < ctx.glyphs[win->unicode].numlayer &&
        ctx.glyphs[win->unicode].layers[sellayers].type == SSFN_FRAG_CONTOUR) {

            if(!ctx.glyphs[win->unicode].layers[sellayers].len) {
                t = SSFN_CONTOUR_MOVE;
            } else {
                i = ctx.glyphs[win->unicode].layers[sellayers].len - 1;
                px = ((sfncont_t*)ctx.glyphs[win->unicode].layers[sellayers].data)[i].px;
                py = ((sfncont_t*)ctx.glyphs[win->unicode].layers[sellayers].data)[i].py;
                if(t == SSFN_CONTOUR_QUAD) {
                    c1x = (x - px) / 2 + px;
                    c1y = (y - py) / 2 + py;
                } else if(t == SSFN_CONTOUR_CUBIC) {
                c1x = (x - px) / 3 + px;
                c1y = (y - py) / 3 + py;
                c2x = (x - px) * 2 / 3 + px;
                c2y = (y - py) * 2 / 3 + py;
                }
            }
            sfn_contadd(&ctx.glyphs[win->unicode].layers[sellayers], t | 0x100, x, y, c1x, c1y, c2x, c2y);
        }
}

/**
 * On enter handler
 */
void ctrl_layers_onenter(int idx)
{
    ui_win_t *win = &wins[idx];
    int x = win->w - 74;
    if(x < 0) x = 0;
    if(sellayers < 0 || sellayers >= ctx.glyphs[win->unicode].numlayer) sellayers = 0;
    switch(win->field) {
        case 4: ctrl_zoom_out(idx, (x-20)/2 + 20, (win->h-36-24)/2 + 36); break;
        case 5: ctrl_zoom_in(idx, (x-20)/2 + 20, (win->h-36-24)/2 + 36); break;
        case 6:
            if(sellayers < ctx.glyphs[win->unicode].numlayer) {
                copypaste_start(win->unicode);
                copypaste_copy(win->unicode, sellayers);
                sfn_layerdel(win->unicode, sellayers);
                if(sellayers >= ctx.glyphs[win->unicode].numlayer) sellayers = ctx.glyphs[win->unicode].numlayer - 1;
                if(sellayers < 0) sellayers = 0;
            }
        break;
        case 7:
            if(sellayers < ctx.glyphs[win->unicode].numlayer) {
                copypaste_start(win->unicode); copypaste_copy(win->unicode, sellayers);
            }
        break;
        case 8: copypaste_paste(win->unicode, 1); break;
        case 9: ctrl_layers_add(idx, SSFN_FRAG_CONTOUR); break;
        case 10: ctrl_layers_add(idx, SSFN_FRAG_BITMAP); break;
        case 11: ctrl_layers_add(idx, SSFN_FRAG_PIXMAP); break;
        case 13:
            win->field = -1;
            if(sellayers < ctx.glyphs[win->unicode].numlayer &&
                ctx.glyphs[win->unicode].layers[sellayers].color == 0xFF) { colorsel = 0xFE; ctrl_colors_onenter(idx); } else
                    win->tool = GLYPH_TOOL_COLOR;
        break;
        case 14:
            win->field = -1;
            if(sellayers < ctx.glyphs[win->unicode].numlayer &&
                ctx.glyphs[win->unicode].layers[sellayers].type == SSFN_FRAG_PIXMAP) ispicker = 1;
        break;
        case 15:
            win->field = -1;
            if(sellayers < ctx.glyphs[win->unicode].numlayer) { sfn_layerdel(win->unicode, sellayers); modified++; }
        break;
    }
}

/**
 * On key handler
 */
void ctrl_layers_onkey(int idx)
{
    ui_win_t *win = &wins[idx];
    if(sellayers < 0 || sellayers >= ctx.glyphs[win->unicode].numlayer) sellayers = 0;
    if(event.h & (3 << 1)) {
        switch(event.x) {
            case 'x': case 'X': win->field = 6; ctrl_layers_onenter(idx); break;
            case 'c': case 'C': win->field = 7; ctrl_layers_onenter(idx); break;
            case 'v': case 'V': win->field = 8; ctrl_layers_onenter(idx); break;
        }
        win->field = -1;
    } else
    if(event.x == 'h' || event.x == 'H') ctrl_fliph(idx, sellayers); else
    if(event.x == 'v' || event.x == 'V') ctrl_flipv(idx, sellayers); else
    if(event.x == K_BACKSPC) ctrl_layers_delcmd(idx); else
    if(win->field == 12) {
        switch(event.x) {
            case K_UP: if(sellayers > 0) sellayers--; break;
            case K_DOWN: if(sellayers < ctx.glyphs[win->unicode].numlayer - 1) sellayers++; break;
        }
    } else {
        switch(event.x) {
            case K_UP: ctrl_pos(idx, sellayers, 0, -1); break;
            case K_LEFT: ctrl_pos(idx, sellayers, -1, 0); break;
            case K_DOWN: ctrl_pos(idx, sellayers, 0, 1); break;
            case K_RIGHT: ctrl_pos(idx, sellayers, 1, 0); break;
            case K_DEL: win->field = 15; ctrl_layers_onenter(idx); break;
            case K_PGUP: if(sellayers > 0) sellayers--; break;
            case K_PGDN: if(sellayers < ctx.glyphs[win->unicode].numlayer - 1) sellayers++; break;
        }
    }
    ui_refreshwin(event.win, 20, 36, win->w - 20, win->h - 36 - 24);
}

/**
 * On button press handler
 */
void ctrl_layers_onbtnpress(int idx)
{
    ui_win_t *win = &wins[idx];
    int x = win->w - 74, ox = win->zx > 0 ? win->zx : 0, oy = win->zy > 0 ? win->zy : 0;
    if(x < 0) x = 0;
    selfield = win->field = mousex = mousey = -1; issel = isclick = isfrc = 0;
    if(sellayers < 0 || sellayers >= ctx.glyphs[win->unicode].numlayer) sellayers = 0;
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
    if(event.x > x) {
        if(event.y > 26 && event.y < win->h - 42 && event.x > x && event.x < x + 70) {
            if(event.w & 1) sellayers = (event.y - 28) / 64 + scrolllayers; else
            if(event.w & (1 << 3)) sellayers--; else
            if(event.w & (1 << 4)) sellayers++;
            if(sellayers < 0 || sellayers >= ctx.glyphs[win->unicode].numlayer) sellayers = 0;
        } else
        if(event.y > win->h - 42 && event.x > x && sellayers < ctx.glyphs[win->unicode].numlayer) {
            if(event.x > x && event.x < x + 24) selfield = 8; else
            if(event.x > x + 24 && event.x < x + 48 && ctx.glyphs[win->unicode].layers[sellayers].type == SSFN_FRAG_PIXMAP)
                selfield = 9; else
            if(event.x > x + 48 && event.x < x + 72) selfield = 10;
        }
    } else
    if(event.y < win->h - 22) {
        if(event.w & (1 << 3)) ctrl_zoom_in(event.win, event.x, event.y); else
        if(event.w & (1 << 4)) ctrl_zoom_out(event.win, event.x, event.y); else
        if(event.x >= ox + 20 && event.y >= oy + 36 &&
            event.x <= ox + 20 + win->zoom * ctx.glyphs[win->unicode].width &&
            event.y <= oy + 36 + win->zoom * ctx.glyphs[win->unicode].height) {
                if(ispicker) {
                    if(posx != -1 && posy != -1 && sellayers < ctx.glyphs[win->unicode].numlayer &&
                        ctx.glyphs[win->unicode].layers[sellayers].type == SSFN_FRAG_PIXMAP)
                            ctx.glyphs[win->unicode].layers[sellayers].color =
                                ctx.glyphs[win->unicode].layers[sellayers].data[ctx.glyphs[win->unicode].width * posy + posx];
                    ispicker = 0;
                } else {
                    if(event.h && posx != -1 && posy != -1 && sellayers < ctx.glyphs[win->unicode].numlayer &&
                        ctx.glyphs[win->unicode].layers[sellayers].type == SSFN_FRAG_CONTOUR) {
                            selcmd = ctx.glyphs[win->unicode].layers[sellayers].len << 2;
                            ctrl_layers_addcmd(idx, event.h & 1 ? SSFN_CONTOUR_LINE : (event.w & 1 ? SSFN_CONTOUR_CUBIC :
                                SSFN_CONTOUR_QUAD), posx, posy);
                            issel = 1;
                    }
                    if(selcmd == -1) { mousex = event.x; mousey = event.y; issel = 0; }
                    else issel = 1;
                    isclick = 1; cx = event.x; cy = event.y;
                    isfrc = event.w & 4;
                }
        }
    }
}

/**
 * On click (button release) handler
 */
void ctrl_layers_onclick(int idx)
{
    ui_win_t *win = &wins[idx];
    uint32_t c;
    int x = win->w - 74, ox = win->zx > 0 ? win->zx : 0, oy = win->zy > 0 ? win->zy : 0;
    if(x < 0) x = 0;
    ispicker = 0; cursor = CURSOR_PTR;
    if(sellayers < 0 || sellayers >= ctx.glyphs[win->unicode].numlayer) sellayers = 0;
    if(event.y < 26) {
        if(event.x > x && event.x < x + 24 && selfield == 5) { win->field = 9; ctrl_layers_onenter(idx); } else
        if(event.x > x + 24 && event.x < x + 48 && selfield == 6) { win->field = 10; ctrl_layers_onenter(idx); } else
        if(event.x > x + 48 && event.x < x + 64 && selfield == 7) { win->field = 11; ctrl_layers_onenter(idx); } else
        if(event.x > 144 && event.x < 166 && selfield == 0) { win->field = 4; ctrl_layers_onenter(idx); } else
        if(event.x > 168 && event.x < 190 && selfield == 1) { win->field = 5; ctrl_layers_onenter(idx); } else
        if(event.x > 198 && event.x < 220 && selfield == 2) { win->field = 6; ctrl_layers_onenter(idx); } else
        if(event.x > 224 && event.x < 246 && selfield == 3) { win->field = 7; ctrl_layers_onenter(idx); } else
        if(event.x > 248 && event.x < 270 && selfield == 4) { win->field = 8; ctrl_layers_onenter(idx); }
    } else
    if(event.x > x && event.y > win->h - 42) {
        if(event.x > x && event.x < x + 24 && selfield == 8) { win->field = 13; ctrl_layers_onenter(idx); } else
        if(event.x > x + 24 && event.x < x + 48 && selfield == 9) { win->field = 14; ctrl_layers_onenter(idx); } else
        if(event.x > x + 48 && event.x < x + 72 && selfield == 10) { win->field = 15; ctrl_layers_onenter(idx); }
    } else
    if(event.x >= ox + 20 && event.y >= oy + 36 &&
        event.x <= ox + 20 + win->zoom * ctx.glyphs[win->unicode].width &&
        event.y <= oy + 36 + win->zoom * ctx.glyphs[win->unicode].height && sellayers < ctx.glyphs[win->unicode].numlayer) {
            if(ctx.glyphs[win->unicode].layers[sellayers].type == SSFN_FRAG_CONTOUR && selcmd != -1) cursor = CURSOR_CROSS;
            if((isclick || isfrc) && !ispicker && (ctx.glyphs[win->unicode].layers[sellayers].type == SSFN_FRAG_BITMAP ||
                ctx.glyphs[win->unicode].layers[sellayers].type == SSFN_FRAG_PIXMAP)) {
                    cursor = CURSOR_CROSS;
                    c = ctx.glyphs[win->unicode].layers[sellayers].type == SSFN_FRAG_BITMAP ? 0xFE :
                        ctx.glyphs[win->unicode].layers[sellayers].color;
                    if(ctx.glyphs[win->unicode].layers[sellayers].data[ctx.glyphs[win->unicode].width * posy + posx] == c)
                        ctx.glyphs[win->unicode].layers[sellayers].data[ctx.glyphs[win->unicode].width * posy + posx] = 0xFF;
                    else
                        ctx.glyphs[win->unicode].layers[sellayers].data[ctx.glyphs[win->unicode].width * posy + posx] = c;
                    modified++;
            }
        }
    isclick = issel = isfrc = 0;
    selfield = win->field = mousex = mousey = -1;
}

/**
 * On mouse move handler
 */
void ctrl_layers_onmove(int idx)
{
    ui_win_t *win = &wins[idx];
    sfncont_t *cont;
    int i, j, x = win->w - 74, ox = win->zx > 0 ? win->zx : 0, oy = win->zy > 0 ? win->zy : 0;
    i = event.x > cx ? event.x - cx : cx - event.x; j = event.y > cy ? event.y - cy : cy - event.y; if(j > i) i = j;
    if(isfrc || (isclick && i < 3)) return;
    if(x < 0) x = 0;
    posx = posy = -1; isclick = 0;
    if(sellayers < 0 || sellayers >= ctx.glyphs[win->unicode].numlayer) sellayers = 0;
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
    if(event.x > x && event.y > win->h - 42) {
        if(event.x > x && event.x < x + 24) status = lang[LAYERS_FOREGROUND]; else
        if(event.x > x + 24 && event.x < x + 48) status = lang[LAYERS_PICKER]; else
        if(event.x > x + 48 && event.x < x + 72) status = lang[LAYERS_DELETE];
    } else
    if(event.x >= ox + 20 && event.y >= oy + 36 &&
        event.x <= ox + 20 + win->zoom * ctx.glyphs[win->unicode].width &&
        event.y <= oy + 36 + win->zoom * ctx.glyphs[win->unicode].height && event.y < win->h - 22) {
            if(sellayers < ctx.glyphs[win->unicode].numlayer &&
                ctx.glyphs[win->unicode].layers[sellayers].type == SSFN_FRAG_CONTOUR) {
                    event.x += win->zoom/2; event.y += win->zoom/2;
            }
            posx = (event.x - ox - 20 - (win->zx < 0 ? win->zx : 0)) / win->zoom;
            if(posx >= ctx.glyphs[win->unicode].width) posx = -1;
            posy = (event.y - oy - 36 - (win->zy < 0 ? win->zy : 0)) / win->zoom;
            if(posy >= ctx.glyphs[win->unicode].height) posy = -1;
            if(!ispicker && mousex != -1 && mousey != -1) ctrl_move(event.win, event.x, event.y); else
            if(sellayers < ctx.glyphs[win->unicode].numlayer) {
                cursor = CURSOR_CROSS;
                if(!ispicker) {
                    if(selcmd != -1 && issel) {
                        /* normally we would use -1 to indicate we are off-layer, but with move we must clamp */
                        if(posx == -1 || posx >= ctx.glyphs[win->unicode].width) posx = ctx.glyphs[win->unicode].width - 1;
                        if(posy == -1 || posy >= ctx.glyphs[win->unicode].height) posy = ctx.glyphs[win->unicode].height - 1;
                        cont = (sfncont_t*)ctx.glyphs[win->unicode].layers[sellayers].data;
                        switch(selcmd & 3) {
                            case 0: cont[selcmd >> 2].px = posx; cont[selcmd >> 2].py = posy; break;
                            case 1: cont[selcmd >> 2].c1x = posx; cont[selcmd >> 2].c1y = posy; break;
                            case 2: cont[selcmd >> 2].c2x = posx; cont[selcmd >> 2].c2y = posy; break;
                        }
                        modified++;
                        ui_refreshwin(event.win, 20, 36, win->w - 20 - x, win->h - 36 - 24);
                    } else
                    if(ctx.glyphs[win->unicode].layers[sellayers].type == SSFN_FRAG_CONTOUR) {
                        selcmd = -1;
                        cont = (sfncont_t*)ctx.glyphs[win->unicode].layers[sellayers].data;
                        for(i = 0; i < ctx.glyphs[win->unicode].layers[sellayers].len; i++, cont++) {
                            if(cont->px == posx && cont->py == posy) { selcmd = i << 2; break; }
                            if(cont->type >= SSFN_CONTOUR_QUAD && cont->c1x == posx && cont->c1y == posy)
                                { selcmd = (i << 2)|1; break; }
                            if(cont->type == SSFN_CONTOUR_CUBIC && cont->c2x == posx && cont->c2y == posy)
                                { selcmd = (i << 2)|2; break; }
                        }
                        if(selcmd == -1) cursor = CURSOR_PTR;
                    }
                } else
                    cursor = CURSOR_GRAB;
            }
    }
}
