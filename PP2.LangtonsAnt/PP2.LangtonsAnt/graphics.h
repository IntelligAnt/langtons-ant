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
#ifndef COLOR_EMPTY
#define COLOR_EMPTY -1
#endif

#define GET_PAIR_FOR(c)  (COLOR_PAIR(c+1))
#define GET_COLOR_FOR(p) (PAIR_NUMBER(p)-1)
#define AVAILABLE_COLOR(def, c, bk) (((def) != (c)) ? (c) : (bk))

/*** Grid window attributes and types ***/

#define GRID_WINDOW_SIZE    109
#define GRID_VIEW_SIZE      (GRID_WINDOW_SIZE - 1)
#define GRID_BUFFER_ZONE    2
#define LINE_WIDTH_SMALL    2
#define LINE_WIDTH_MEDIUM   1
#define LINE_WIDTH_LARGE    0
#define SLIDER_MIN_SIZE     1
#define SCROLL_STEP         (GRID_WINDOW_SIZE / 4)
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

/*** Menu window attributes and types ***/

#define MENU_WINDOW_WIDTH   42
#define MENU_WINDOW_HEIGHT  GRID_WINDOW_SIZE
#define MENU_LOGO_HEIGHT    16
#define MENU_BORDER_COLOR   COLOR_BLUE
#define MENU_BORDER_COLOR_S COLOR_MAROON

#define MENU_TILE_SIZE      7
#define MENU_TILE_HSEP      3
#define MENU_TILE_VSEP      2
#define MENU_TILE_COLUMNS   2
#define MENU_TILES_PER_COL  7
#define MENU_TILE_COUNT     (MENU_TILE_COLUMNS * MENU_TILES_PER_COL)
#define MENU_TILES_WIDTH    (MENU_TILE_COLUMNS*MENU_TILE_SIZE + (MENU_TILE_COLUMNS-1)*MENU_TILE_HSEP)
#define MENU_TILES_HEIGHT   (MENU_TILES_PER_COL*MENU_TILE_SIZE + (MENU_TILES_PER_COL+1)*MENU_TILE_VSEP+2)

#define MENU_COMMANDS_POS   88
#define MENU_BUTTON_WIDTH   11
#define MENU_BUTTON_HEIGHT  7
#define MENU_PLAY_COLOR     COLOR_GREEN
#define MENU_PAUSE_COLOR    COLOR_YELLOW
#define MENU_CLEAR_COLOR    COLOR_TEAL

#define KEY_ESC 0x1B

typedef struct settings {
	Colors *colors;
	size_t init_size;
	Simulation *linked_sim;
} Settings;

/*** Dialog window attributes ***/

#define DIALOG_TILE_SIZE     3
#define DIALOG_BUTTON_WIDTH  7
#define DIALOG_BUTTON_HEIGHT 3
#define DIALOG_BUTTON_COLOR  COLOR_YELLOW
#define DIALOG_DELETE_COLOR  COLOR_RED
#define DIALOG_ROWS          3
#define DIALOG_COLS          5
#define DIALOG_WINDOW_WIDTH  (DIALOG_COLS*DIALOG_TILE_SIZE + 2)
#define DIALOG_WINDOW_HEIGHT (DIALOG_ROWS*DIALOG_TILE_SIZE + DIALOG_BUTTON_HEIGHT*2 + 4)

#define CIDX_NEWCOLOR -1
#define CIDX_DEFAULT  -2

/*** Globals ***/

extern chtype fg_pair, bg_pair;
extern WINDOW *gridw, *menuw, *dialogw;
extern ScrollInfo gridscrl;
extern Settings stgs;
extern const Vector2i grid_pos, menu_pos;
extern Vector2i dialog_pos;
extern const Vector2i menu_isz_u_pos, menu_isz_d_pos;
extern const Vector2i menu_play_pos, menu_pause_pos, menu_clear_pos;
extern const Vector2i menu_load_pos, menu_save_pos;

/* graphics.c */

void init_graphics(short fg_color, short bg_color);
void end_graphics(void);
void init_def_pairs(short fg_color, short bg_color);
void draw_loop(void);
void exit_draw_loop(bool b);
void draw_box(WINDOW *w, Vector2i top_left, size_t size);
void draw_rect(WINDOW *w, Vector2i top_left, size_t width, size_t height);
void draw_bitmap(WINDOW *w, Vector2i top_left,
				 const unsigned char *bitmap, size_t width, size_t height,
				 bool overwrite);
Vector2i rel2abs(Vector2i rel, Vector2i origin);
Vector2i abs2rel(Vector2i abs, Vector2i origin);
bool area_contains(Vector2i top_left, size_t width, size_t height, Vector2i needle);
int sgn(int x);

/* grid_window.c */

void init_grid_window(void);
void end_grid_window(void);
void draw_grid_full(Grid *grid);
void draw_grid_iter(Grid *grid, Vector2i oldp);
void scroll_grid(Grid *grid, int dy, int dx);
void set_scroll(Grid *grid, int y, int x);

/* grid_controls.c */

int grid_key_command(Grid *grid, Ant *ant, int key);
void grid_mouse_command(Grid *grid);

/* menu_window.c */
void init_menu_window(void);
void end_menu_window(void);
void draw_menu(void);
Vector2i get_menu_tile_pos(int index);

/* menu_controls.c */
int menu_key_command(int key);
void menu_mouse_command();

/* dialog.c */
void open_dialog(Vector2i pos, int color_index);
void close_dialog(void);
void draw_dialog(void);
Vector2i get_dialog_tile_pos(int index);
Vector2i get_dialog_button_pos(int index);
Vector2i get_del_button_pos(void);
void dialog_mouse_command(MEVENT event);

#endif
