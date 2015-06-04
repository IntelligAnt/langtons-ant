#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include "include/curses.h"
#include "logic.h"

#undef COLOR_BLACK
#undef COLOR_RED
#undef COLOR_GREEN
#undef COLOR_BLUE
#undef COLOR_CYAN
#undef COLOR_MAGENTA
#undef COLOR_YELLOW
#undef COLOR_WHITE

/*** 4-bit color macros (HTML names) ***/

#define COLOR_BLACK   0
#define COLOR_GRAY    8

#ifdef PDC_RGB        /* RGB */
#define COLOR_MAROON  1
#define COLOR_GREEN   2
#define COLOR_NAVY    4

#define COLOR_RED     9
#define COLOR_LIME    10
#define COLOR_BLUE    12
#else                 /* BGR */
#define COLOR_NAVY    1
#define COLOR_GREEN   2
#define COLOR_MAROON  4

#define COLOR_BLUE    9
#define COLOR_LIME    10
#define COLOR_RED     12
#endif

#define COLOR_TEAL    (COLOR_NAVY | COLOR_GREEN)
#define COLOR_PURPLE  (COLOR_MAROON | COLOR_NAVY)
#define COLOR_OLIVE   (COLOR_MAROON | COLOR_GREEN)

#define COLOR_AQUA    (COLOR_BLUE | COLOR_LIME)
#define COLOR_FUCHSIA (COLOR_RED | COLOR_BLUE)
#define COLOR_YELLOW  (COLOR_RED | COLOR_LIME)

#define COLOR_SILVER  7
#define COLOR_WHITE   15

#ifndef COLOR_COUNT
#define COLOR_COUNT 16
#endif

/*** Grid window attributes ***/

#define GRID_WINDOW_SIZE 82
#define GRID_BUFFER_ZONE 2
#define GRID_CELL ACS_BLOCK

/*** Menu window attributes ***/

#define MENU_WINDOW_SIZE 18

void init_graphics(short fg_color, short bg_color);
void end_graphics(void);
void init_def_pairs(short fg_color, short bg_color);
short get_pair_for(short color);

/* grid_window.c */

#define LINE_WIDTH_SMALL  2
#define LINE_WIDTH_MEDIUM 1
#define LINE_WIDTH_LARGE  0
#define CELL_SIZE(gs, lw) ((GRID_WINDOW_SIZE-lw)/gs - lw)
#define TOTAL_SIZE(n, cs, lw) (n*cs + (n+1)*lw)

void init_grid_window(void);
void end_grid_window(void);
void grid_draw_full(Grid *grid);
void grid_draw_iter(Vector2i oldp, short oldc, Vector2i newp, short newc);

#endif
