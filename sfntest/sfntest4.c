/*
 * sfntest4.c
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
 * @brief testing Scalable Screen Font normal renderer with bitmap fonts
 *
 */

#include <stdio.h>
#include <stdint.h>
#define SSFN_IMPLEMENTATION
#define SSFN_CONSOLEBITMAP_TRUECOLOR
#include "../ssfn.h"
#if HAS_ZLIB
#include <zlib.h>
#endif

#include <SDL.h>

/**
 * Load a font
 */
ssfn_font_t *load_font(char *filename)
{
    char *fontdata = NULL;
    long int size;
    FILE *f;
#if HAS_ZLIB
    unsigned char hdr[2];
    gzFile g;
#endif

    f = fopen(filename, "rb");
    if(!f) { fprintf(stderr,"unable to load %s\n", filename); exit(3); }
    size = 0;
#if HAS_ZLIB
    fread(&hdr, 2, 1, f);
    if(hdr[0]==0x1f && hdr[1]==0x8b) {
        fseek(f, -4L, SEEK_END);
        fread(&size, 4, 1, f);
    } else {
        fseek(f, 0, SEEK_END);
        size = ftell(f);
    }
    fclose(f);
    g = gzopen(filename,"r");
#else
    fseek(f, 0, SEEK_END);
    size = ftell(f);
    fseek(f, 0, SEEK_SET);
#endif
    if(!size) { fprintf(stderr,"unable to load %s\n", filename); exit(3); }
    fontdata = malloc(size);
    if(!fontdata) { fprintf(stderr,"memory allocation error\n"); exit(2); }
#if HAS_ZLIB
    gzread(g, fontdata, size);
    gzclose(g);
#else
    fread(fontdata, size, 1, f);
    fclose(f);
#endif

    return (ssfn_font_t*)fontdata;
}

/**
 * testing the SSFN library (normal renderer)
 */
void do_test(SDL_Surface *screen, char *fontfn)
{
/*
    char *s, *str[] = {
        "Normal renderer with UNICODE VGA!", "Üdvözlet!", "¡Bienvenido!", "Здравствуйте!", "Καλως ηρθες!", "متعدد اللغات",
        NULL };
    char *str0 = "Bitmap fonts rendering to Bitmap";
    char *str1 = "Bitmap to Alpha channel rendering";
    char *str2 = "Bitmap Italic (not that bad)";
    char *str3 = "Bitmap Bold style";
    char *str4 = "Underline text pqg";
    char *str5 = "Strike-through test";
    char *str6 = "Color map mode test";
*/
    int err, x, y, i;
    ssfn_t ctx;
    ssfn_font_t *font;

    /* initialize the normal renderer */
    memset(&ctx, 0, sizeof(ssfn_t));
    ssfn_dst.ptr = (uint8_t*)screen->pixels;
    ssfn_dst.p = screen->pitch;
    ssfn_dst.w = screen->w;
    ssfn_dst.h = screen->h;
    ssfn_dst.fg = 0xFF202020;
    ssfn_dst.bg = 0;
    ssfn_dst.x = 4; ssfn_dst.y = 48;

    /* load and select a font */
    font = load_font(fontfn ? fontfn : (char*)"../fonts/VeraR.sfn");
    /*font = load_file(fontfn ? fontfn : "../fonts/u_vga16.sfn.gz", &size);*/
    /*font = load_file(fontfn ? fontfn : "../fonts/smilely.sfn", &size);*/
    err = ssfn_load(&ctx, font);
    if(err != SSFN_OK) { fprintf(stderr, "ssfn load error: err=%d %s\n", err, ssfn_error(err)); exit(2); }

    err = ssfn_select(&ctx, SSFN_FAMILY_ANY, NULL, SSFN_STYLE_REGULAR, 20);
    if(err != SSFN_OK) { fprintf(stderr, "ssfn select error: err=%d %s\n", err, ssfn_error(err)); exit(2); }

    for(y = 0; y < 19; y++) {
        for(x = 0; x < 48; x++) {
            i = (255 - ((y < 10 ? y : 20 - y) * 255 / 10)) & 0xff;
            ((uint32_t*)(screen->pixels))[(y+30)*screen->pitch/4+(x)] = 0xFF0000FF | (i << 16) | (i << 8);
        }
        for(x = 76; x < 110; x++) {
            i = (255 - ((y < 10 ? y : 20 - y) * 255 / 10)) & 0xff;
            ((uint32_t*)(screen->pixels))[(y+30)*screen->pitch/4+(x)] = 0xFF0000FF | (i << 16) | (i << 8);
        }
    }

    err = ssfn_render(&ctx, &ssfn_dst, "A");
    if(err < 0) { fprintf(stderr, "ssfn render error: err=%d %s\n", err, ssfn_error(err)); exit(2); }

    ssfn_dst.x = 28; ssfn_dst.bg = 0xFF00FF00;
    err = ssfn_render(&ctx, &ssfn_dst, "A");
    if(err < 0) { fprintf(stderr, "ssfn render error: err=%d %s\n", err, ssfn_error(err)); exit(2); }

    /* initialize the simple renderer */
    ssfn_src = load_font("../fonts/u_vga16.sfn.gz");
    ssfn_dst.bg = 0;

    ssfn_dst.x = 80; ssfn_dst.y = 32;
    ssfn_putc(65);

    ssfn_dst.x = 96; ssfn_dst.bg = 0xFF00FF00;
    ssfn_putc(65);

    /* zoom */
    for(y = 0; y < 64; y++) {
        for(x = 0; x < 128; x++) {
            ((uint32_t*)(screen->pixels))[(128+4*y+0)*screen->pitch/4+(4*x+0)] =
            ((uint32_t*)(screen->pixels))[(128+4*y+0)*screen->pitch/4+(4*x+1)] =
            ((uint32_t*)(screen->pixels))[(128+4*y+0)*screen->pitch/4+(4*x+2)] =
            ((uint32_t*)(screen->pixels))[(128+4*y+0)*screen->pitch/4+(4*x+3)] =
            ((uint32_t*)(screen->pixels))[(128+4*y+1)*screen->pitch/4+(4*x+0)] =
            ((uint32_t*)(screen->pixels))[(128+4*y+1)*screen->pitch/4+(4*x+1)] =
            ((uint32_t*)(screen->pixels))[(128+4*y+1)*screen->pitch/4+(4*x+2)] =
            ((uint32_t*)(screen->pixels))[(128+4*y+1)*screen->pitch/4+(4*x+3)] =
            ((uint32_t*)(screen->pixels))[(128+4*y+2)*screen->pitch/4+(4*x+0)] =
            ((uint32_t*)(screen->pixels))[(128+4*y+2)*screen->pitch/4+(4*x+1)] =
            ((uint32_t*)(screen->pixels))[(128+4*y+2)*screen->pitch/4+(4*x+2)] =
            ((uint32_t*)(screen->pixels))[(128+4*y+2)*screen->pitch/4+(4*x+3)] =
            ((uint32_t*)(screen->pixels))[(128+4*y+3)*screen->pitch/4+(4*x+0)] =
            ((uint32_t*)(screen->pixels))[(128+4*y+3)*screen->pitch/4+(4*x+1)] =
            ((uint32_t*)(screen->pixels))[(128+4*y+3)*screen->pitch/4+(4*x+2)] =
            ((uint32_t*)(screen->pixels))[(128+4*y+3)*screen->pitch/4+(4*x+3)] =
                ((uint32_t*)(screen->pixels))[y*screen->pitch/4+(x)];
        }
    }
    printf("Memory allocated: %d %d\n", ssfn_mem(&ctx), (int)sizeof(ssfn_t));
    ssfn_free(&ctx);
    free(font);
}

/**
 * Main procedure
 */
int main(int argc __attribute__((unused)), char **argv)
{
    SDL_Window *window;
    SDL_Surface *screen;
    SDL_Event event;

    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS)) {
        fprintf(stderr,"SDL error %s\n", SDL_GetError());
        return 2;
    }

    window = SDL_CreateWindow("SSFN normal renderer bitmap font test",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,800,600,0);
    screen = SDL_GetWindowSurface(window);
    memset(screen->pixels, 0xF8, screen->pitch*screen->h);

    do_test(screen, argv[1]);

    do{ SDL_UpdateWindowSurface(window); SDL_Delay(10); } while(SDL_WaitEvent(&event) && event.type != SDL_QUIT &&
        event.type != SDL_MOUSEBUTTONDOWN && event.type != SDL_KEYDOWN);

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
