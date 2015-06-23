#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include "logic.h"
#include "include/curses.h"

#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))

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

/*** Grid window attributes and types ***/

#define GRID_WINDOW_SIZE    109
#define GRID_VIEW_SIZE      (GRID_WINDOW_SIZE - 1)
#define GRID_BUFFER_ZONE    2
#define LINE_WIDTH_SMALL    2
#define LINE_WIDTH_MEDIUM   1
#define LINE_WIDTH_LARGE    0
#define SLIDER_MIN_SIZE     1
#define SCROLL_STEP         10
#define SCROLL_STEP_BIG     GRID_VIEW_SIZE

#define CELL_SIZE(gs, lw)      ((GRID_WINDOW_SIZE-(lw))/(gs) - (lw))
#define TOTAL_SIZE(gs, lw, cs) (((gs)+1)*(lw) + (gs)*(cs))
#define OFFSET_SIZE(total)     ((GRID_WINDOW_SIZE-(total)) / 2)

#define ORIGIN_COORD(gs, vs, sc)     max((gs)/2-(vs)/2+(sc), 0)
#define ORIGIN_POS(gs, vs, scy, scx) (Vector2i) { ORIGIN_COORD(gs, vs, scy), ORIGIN_COORD(gs, vs, scx) }

typedef struct scroll_info {
	bool enabled;
	int y, x, hcenter, vcenter;
	double scale;
} ScrollInfo;

/*** Menu window attributes ***/

#define MENU_WINDOW_SIZE    42
#define MENU_BORDER_COLOR   COLOR_BLUE
#define MENU_TILE_SIZE      6
#define MENU_TILE_HSEP      4
#define MENU_TILE_VSEP      2
#define MENU_TILE_COLUMNS   2
#define MENU_TILES_PER_COL  7
#define MENU_TILE_COUNT     (MENU_TILE_COLUMNS * MENU_TILES_PER_COL)

#define KEY_ESC 0x1B

typedef struct settings {
	Colors *colors;
	size_t steps;
} Settings;

/*** Globals ***/

extern chtype fg_pair, bg_pair;
extern WINDOW *gridw, *menuw;
extern ScrollInfo gridscrl;
extern Settings stgs;

/* graphics.c */

void init_graphics(short fg_color, short bg_color);
void end_graphics(void);
void init_def_pairs(short fg_color, short bg_color);
chtype get_pair_for(short color);
short get_color_for(chtype pair);
void draw_box(WINDOW *w, Vector2i top_left, int size);
void draw_bitmap(WINDOW *w, Vector2i top_left,
				 unsigned char *bitmap, size_t width, size_t height);
Vector2i rel2abs(Vector2i rel, Vector2i origin);
Vector2i abs2rel(Vector2i abs, Vector2i origin);
int sgn(int x);

/* grid_window.c */

void init_grid_window(void);
void end_grid_window(void);
void draw_grid_full(Grid *grid);
void draw_grid_iter(Grid *grid, Vector2i oldp, bool refresh);
void scroll_grid(Grid *grid, int dy, int dx);
void set_scroll(Grid *grid, int y, int x);

/* grid_controls.c */

int grid_key_command(Grid *grid, Ant *ant, int key);
void grid_mouse_command(Grid *grid);

/* menu_window.c */
void init_menu_window(void);
void end_menu_window(void);
void draw_menu(void);

#endif
