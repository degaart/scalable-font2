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
#include "hist.h"

/*** used by UI drivers too ***/
enum {
    CURSOR_LOADING,         /* cursor to indicate that sfnedit is working */
    CURSOR_PTR,             /* the default normal cursor pointer */
    CURSOR_CROSS,           /* a cross-hair, used when selecting pixels and contour points */
    CURSOR_MOVE,            /* used when user drags the layers and moves it around */
    CURSOR_GRAB             /* used for links and the color picker */
};

#define E_NONE          0   /* no event */
#define E_CLOSE         1   /* window close button clicked */
#define E_RESIZE        2   /* window is resized */
#define E_REFRESH       3   /* window has to be updated (expose) */
#define E_KEY           4   /* key press event */
#define E_MOUSEMOVE     5   /* mouse pointer is moving */
#define E_BTNPRESS      6   /* mouse button is pressed */
#define E_BTNRELEASE    7   /* mouse button is released */

#define K_DOWN          1   /* cursor down arrow */
#define K_UP            2   /* cursor up arrow */
#define K_LEFT          3   /* cursor left arrow */
#define	K_RIGHT         4   /* cursor right arrow */
#define K_PGDN          5   /* page down key */
#define	K_PGUP          6   /* page up key */
#define K_DEL           7   /* delete key */
#define K_BACKSPC       8   /* backspace key */
#define K_TAB           9   /* tab key */
#define	K_HOME          11  /* home key */
#define	K_END           12  /* end key */
#define K_ENTER         13  /* enter or return key */
#define K_F1            14  /* function one (help) key */
#define K_ESC           27  /* escape key */

typedef struct {
    int win;                /* internal numerical window id where the event happened. 0 is the main window */
    int type;               /* one of E_x defines */
    long x;                 /* for key press, K_x define or UTF-8 character sequence */
    int y;
    int w;                  /* for button events, bit 0: left button, 1: middle, 2: right, 3: wheel up, 4: wheel down */
    int h;                  /* for key and button events, bit 0: shift, 1: ctrl, 2: alt/meta */
} ui_event_t;

typedef struct {
    /* available for drivers */
    void *winid;            /* transparent, driver specific window id */
    void *surface;          /* available to driver (SDL_Surface or XImage for example) */
    uint32_t *data;         /* backstore pixel buffer */
    int w, h, p;            /* window width, height, pitch (number of bytes in a line) */
    /* don't care, internal window state */
    uint32_t unicode;
    char *uniname;
    int field, tool, help;
    int zoom, zx, zy, rc;
    int histmin, histmax;
    hist_t *hist;
} ui_win_t;

/* driver specific, these are the functions that must be implemented in every UI driver, see ui_dummy.c */
void ui_copy(char *s);
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

/*** common definitions and prototypes for ui ***/
#ifdef __WIN32__
# define DIRSEP '\\'
# define DIRSEPS "\\"
#else
# define DIRSEP '/'
# define DIRSEPS "/"
#endif

#define MAIN_W          800
#define MAIN_H          600

#define WINTYPE_MAIN    -1U

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
    THEME_SELFG,
    THEME_CURSOR,
    THEME_UNDEF,

    THEME_AXIS,
    THEME_GRID,
    THEME_DIM,

    THEME_BASE,
    THEME_UNDER,
    THEME_OVL,
    THEME_ADV,
    THEME_HINT,
    THEME_POINT,
    THEME_CPOINT,
    THEME_CURVE,
    THEME_GUIDE,

    THEME_LAST
};

enum {
    ICON_ABOUT,
    ICON_LOAD,
    ICON_SAVE,
    ICON_PROPS,
    ICON_RANGES,
    ICON_GLYPHS,
    ICON_MEASURES,
    ICON_LAYERS,
    ICON_KERNING,
    ICON_FOLDER,
    ICON_FILE,
    ICON_SEARCH,
    ICON_ZOOMOUT,
    ICON_ZOOMIN,
    ICON_CUT,
    ICON_COPY,
    ICON_PASTE,
    ICON_DELETE,
    ICON_UARR,
    ICON_LARR,
    ICON_RARR,
    ICON_DARR,
    ICON_RHORIZ,
    ICON_VERT,
    ICON_HORIZ,
    ICON_SHEAR,
    ICON_UNSHEAR,
    ICON_BOLDER,
    ICON_UNBOLD,
    ICON_HFLIP,
    ICON_VFLIP,
    ICON_VECTOR,
    ICON_BITMAP,
    ICON_PIXMAP,
    ICON_PICKER
};

enum {
    MAIN_TOOL_ABOUT,
    MAIN_TOOL_LOAD,
    MAIN_TOOL_SAVE,
    MAIN_TOOL_PROPS,
    MAIN_TOOL_RANGES,
    MAIN_TOOL_GLYPHS,
    MAIN_TOOL_DOSAVE,
    MAIN_TOOL_NEW
};

enum {
    GLYPH_TOOL_COORD,
    GLYPH_TOOL_LAYER,
    GLYPH_TOOL_KERN,
    GLYPH_TOOL_COLOR
};

extern char verstr[], ws[], *status, *errstatus;
extern uint32_t theme[];
extern int numwin, cursor, zip, ascii, selfield, rs, re, modified, posx, posy, scrolly;
extern ui_win_t *wins;
extern ui_event_t event;
extern uint8_t *icon16, *icon64, *tools, *bga;
extern int input_maxlen, input_refresh;
extern char *input_str;

/* copy'n'paste functions */
void copypaste_start(int minunicode);
void copypaste_copy(int fromunicode, int layer);
void copypaste_paste(int tounicode, int oneunicode);
void copypaste_fini();

/* history functions */
void hist_free(ui_win_t *win);

/* ui widgets */
void ui_toolbox(int idx);
void ui_rect(ui_win_t *win, int x, int y, int w, int h, uint32_t l, uint32_t d);
void ui_box(ui_win_t *win, int x, int y, int w, int h, uint32_t l, uint32_t b, uint32_t d);
void ui_grid(ui_win_t *win, int w, int h);
void ui_gridbg(ui_win_t *win, int x, int y, int w, int h, int a, uint32_t *d, int gx, int gy);
void ui_icon(ui_win_t *win, int x, int y, int icon, int inactive);
int ui_textwidth(char *str);
void ui_text(ui_win_t *win, int x, int y, char *str);
char *ui_input(ui_win_t *win, int x, int y, int w, char *str, int active, int maxlen, int callback);
void ui_button(ui_win_t *win, int x, int y, int w, char *str, int pressed, int active);
void ui_bool(ui_win_t *win, int x, int y, char *s, int state, int active);
void ui_tri(ui_win_t *win, int x, int y, int up);
void ui_num(ui_win_t *win, int x, int y, int num, int active, int sel);
void ui_number(ui_win_t *win, int x, int y, int n, uint32_t c);
void ui_hex(ui_win_t *win, char c);
void ui_argb(ui_win_t *win, int x, int y, int w, int h, uint32_t c);
void ui_glyph(ui_win_t *win, int x, int y, int size, uint32_t unicode, int layer);
void ui_edit(ui_win_t *win, int x, int y, uint32_t unicode, int layer);
void ui_hscrbar(ui_win_t *win, int x, int y, int w, int h, int scroll, int page, int num, int pressed);

/* common */
void ui_gettheme(char *fn);
int ui_casecmp(char *a, char *b, int l);
void ui_error(char *subsystem, int fmt, ...);
void ui_openwin(uint32_t unicode);
void ui_updatetitle(int idx);
void ui_closewin(int idx);
int ui_getwin(void *wid);
void ui_pb(int step, int numstep, int curr, int total, int msg);
void ui_chrinfo(int unicode);
void ui_refreshwin(int idx, int wx, int wy, int ww, int wh);
void ui_refreshall();
void ui_main(char *fn);
void ui_quit(int sig);
void ui_inputfinish();

/* views */
void view_help();
void view_about();
void view_fileops(int save);
void view_dosave();
void view_new();
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
void ctrl_fileops_onmove();
void ctrl_dosave_onenter();
void ctrl_dosave_onbtnpress();
void ctrl_dosave_onclick();
void ctrl_new_onenter();
void ctrl_new_onkey();
void ctrl_new_onbtnpress();
void ctrl_new_onclick();
void ctrl_props_onenter();
void ctrl_props_onkey();
void ctrl_props_onbtnpress();
void ctrl_props_onclick();
void ctrl_ranges_onenter();
void ctrl_ranges_onkey();
void ctrl_ranges_onbtnpress();
void ctrl_ranges_onmove();
void ctrl_glyphs_onenter();
void ctrl_glyphs_onkey();
void ctrl_glyphs_onbtnpress();
void ctrl_glyphs_onclick();
void ctrl_glyphs_onmove();

void ctrl_coords_onenter(int idx);
void ctrl_coords_onkey(int idx);
void ctrl_coords_onbtnpress(int idx);
void ctrl_coords_onclick(int idx);
void ctrl_coords_onmove(int idx);
void ctrl_layers_onenter(int idx);
void ctrl_layers_onkey(int idx);
void ctrl_layers_onbtnpress(int idx);
void ctrl_layers_onclick(int idx);
void ctrl_layers_onmove(int idx);
void ctrl_kern_onenter(int idx);
void ctrl_kern_onkey(int idx);
void ctrl_kern_onbtnpress(int idx);
void ctrl_kern_onclick(int idx);
void ctrl_kern_onmove(int idx);
void ctrl_colors_onenter(int idx);
void ctrl_colors_onbtnpress(int idx);
void ctrl_colors_onclick(int idx);
void ctrl_colors_onmove(int idx);
