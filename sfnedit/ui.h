/*
 * sfnedit/ui.h
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
 * @brief Common user interface definitions and prototypes
 *
 */

#include <stdint.h>
/*#include "hist.h"*/

#ifdef __WIN32__
# define DIRSEP '\\'
# define DIRSEPS "\\"
#else
# define DIRSEP '/'
# define DIRSEPS "/"
#endif

enum {
    THEME_BG,
    THEME_FG,
    THEME_LIGHT,
    THEME_DARK,
    THEME_LIGHTER,
    THEME_DARKER,
    THEME_INPBG,
    THEME_BTNB,
    THEME_BTN0L,
    THEME_BTN0BL,
    THEME_BTN0BD,
    THEME_BTN0D,
    THEME_BTN1L,
    THEME_BTN1BL,
    THEME_BTN1BD,
    THEME_BTN1D,
    THEME_SELBG,
    THEME_CURSOR
};

#define MAIN_W          800
#define MAIN_H          600

enum {
    CURSOR_LOADING,
    CURSOR_PTR,
    CURSOR_CROSS,
    CURSOR_MOVE,
    CURSOR_GRAB
};

enum {
    ICON_ABOUT,
    ICON_LOAD,
    ICON_SAVE,
    ICON_PROPS,
    ICON_RANGES,
    ICON_GLYPHS,
    ICON_MEASURES,
    ICON_KERNING,
    ICON_LAYERS,
    ICON_FOLDER,
    ICON_FILE,
    ICON_SEARCH,
    ICON_BOUNDING,
    ICON_HINTING,
    ICON_VERT,
    ICON_HORIZ,
    ICON_VECTOR,
    ICON_BITMAP,
    ICON_PIXMAP,
    ICON_DELETE
};

#define WINTYPE_MAIN  -1U

#define E_NONE          0
#define E_CLOSE         1
#define E_RESIZE        2
#define E_REFRESH       3
#define E_KEY           4
#define E_MOUSEMOVE     5
#define E_BTNPRESS      6
#define E_BTNRELEASE    7

#define K_DOWN          1
#define K_UP            2
#define K_LEFT          3
#define	K_RIGHT         4
#define K_PGDN          5
#define	K_PGUP          6
#define K_DEL           7
#define K_BACKSPC       8
#define K_TAB           9
#define	K_HOME          11
#define	K_END           12
#define K_ENTER         13
#define K_F1            14
#define K_ESC           27

enum {
    MAIN_TOOL_ABOUT,
    MAIN_TOOL_LOAD,
    MAIN_TOOL_SAVE,
    MAIN_TOOL_PROPS,
    MAIN_TOOL_RANGES,
    MAIN_TOOL_GLYPHS,
    MAIN_TOOL_DOSAVE
};

enum {
    GLYPH_TOOL_COORD,
    GLYPH_TOOL_KERN,
    GLYPH_TOOL_LAYER,
    GLYPH_TOOL_COLOR
};

typedef struct {
    void *winid;
    void *surface;
    uint32_t *data;
    uint32_t unicode;
    char *uniname;
    int w;
    int h;
    int p;
    int field;
    int tool;
    int zoom;
    int histmin, histmax;
/*    hist_t *hist;*/
} ui_win_t;

typedef struct {
    int win;
    int type;
    long x;
    int y;
    int w;
    int h;
} ui_event_t;

extern char verstr[], ws[], *status;
extern uint32_t theme[];
extern int numwin, cursor, zip, ascii, selfield, rs, re;
extern ui_win_t *wins;
extern ui_event_t event;
extern uint8_t *icon16, *icon32, *tools, *numbers, *bga;
extern int input_maxlen, input_refresh;
extern char *input_str;

/* driver specific */
void *ui_createwin(int w, int h);
void ui_titlewin(ui_win_t *win, char *title);
void ui_resizewin(ui_win_t *win, int w, int h);
void ui_flushwin(ui_win_t *win, int x, int y, int w, int h);
void ui_destroywin(ui_win_t *win);
void ui_focuswin(ui_win_t *win);
void ui_cursorwin(ui_win_t *win, int cursor);
void ui_init();
void ui_fini();
void ui_getevent();

/* ui widgets */
void ui_toolbox(int idx);
void ui_rect(ui_win_t *win, int x, int y, int w, int h, uint32_t l, uint32_t d);
void ui_box(ui_win_t *win, int x, int y, int w, int h, uint32_t l, uint32_t b, uint32_t d);
void ui_icon(ui_win_t *win, int x, int y, int icon, int inactive);
int ui_textwidth(char *str);
void ui_text(ui_win_t *win, int x, int y, char *str);
char *ui_input(ui_win_t *win, int x, int y, int w, char *str, int active, int maxlen, int callback);
void ui_button(ui_win_t *win, int x, int y, int w, char *str, int pressed, int active);
void ui_bool(ui_win_t *win, int x, int y, char *s, int state, int active);
void ui_tri(ui_win_t *win, int x, int y, int up);
void ui_num(ui_win_t *win, int x, int y, int num, int active, int sel);
void ui_number(ui_win_t *win, int x, int y, int n, uint32_t c);
void ui_glyph(ui_win_t *win, int x, int y, int size, int unicode, int layer);

/* common */
int ui_casecmp(char *a, char *b, int l);
void ui_error(char *subsystem, int fmt, ...);
void ui_openwin(uint32_t unicode);
void ui_updatetitle(int idx);
void ui_closewin(int idx);
int ui_getwin(void *wid);
void ui_refreshwin(int idx, int wx, int wy, int ww, int wh);
void ui_refreshall();
void ui_main(char *fn);
void ui_quit(int sig);
void ui_inputfinish();

/* views */
void view_about();
void view_fileops(int save);
void view_dosave();
void view_props();
void view_ranges();
void view_glyphs();
void view_coords(int idx);
void view_layers(int idx);
void view_kern(int idx);
void view_color(int idx);

/* controllers */
void ctrl_about_onmove();
void ctrl_about_onclick();
void ctrl_about_onenter();
void ctrl_fileops_onenter(int save);
void ctrl_fileops_onkey();
void ctrl_fileops_onbtnpress(int save);
void ctrl_fileops_onclick(int save);
void ctrl_props_onenter();
void ctrl_props_onkey();
void ctrl_props_onbtnpress();
void ctrl_props_onclick();
void ctrl_ranges_onenter();
void ctrl_ranges_onkey();
void ctrl_ranges_onbtnpress();
void ctrl_glyphs_onenter();
void ctrl_glyphs_onkey();
void ctrl_glyphs_onbtnpress();
void ctrl_glyphs_onclick();
void ctrl_glyphs_onmove();
