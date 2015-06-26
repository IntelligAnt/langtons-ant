/**
 * @file graphics.h
 * @author IntelligAnt
 * @brief File containing all graphics functions
 */
#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include "logic.h"
#include "include/curses.h"

/**
 * @defgroup GRAPHICS Graphics functions
 * @{
 */

/** Standard max/min macros */
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

/** 4-bit color macros (HTML names) */
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

/** Utility color macros */
#define GET_PAIR_FOR(c)  (COLOR_PAIR(c+1))
#define GET_COLOR_FOR(p) (PAIR_NUMBER(p)-1)
#define AVAILABLE_COLOR(def, c, bk) (((def) != (c)) ? (c) : (bk))

/** Grid window attributes */
#define GRID_WINDOW_SIZE    109
#define GRID_VIEW_SIZE      (GRID_WINDOW_SIZE - 1)
#define GRID_BUFFER_ZONE    2
#define LINE_WIDTH_SMALL    2
#define LINE_WIDTH_MEDIUM   1
#define LINE_WIDTH_LARGE    0
#define SLIDER_MIN_SIZE     1
#define SCROLL_STEP         (GRID_WINDOW_SIZE / 4)
#define SCROLL_STEP_BIG     GRID_VIEW_SIZE

/** Generic size calculations for grid functions */
#define CELL_SIZE(gs, lw)      ((GRID_WINDOW_SIZE-(lw))/(gs) - (lw))
#define TOTAL_SIZE(gs, lw, cs) (((gs)+1)*(lw) + (gs)*(cs))
#define OFFSET_SIZE(total)     ((GRID_WINDOW_SIZE-(total)) / 2)

/** Scroll utility macros */
#define ORIGIN_COORD(gs, vs, sc)     max((gs)/2-(vs)/2+(sc), 0)
#define ORIGIN_POS(gs, vs, scy, scx) (Vector2i) { ORIGIN_COORD(gs, vs, scy), ORIGIN_COORD(gs, vs, scx) }

/**
 * @brief Structure for managing scroll data
 */
typedef struct scroll_info {
	bool enabled;		  /**< Scrolling is enabled */
	int y, x;             /**< Current view pos relative to (0,0) */
	int hcenter, vcenter; /**< Scrollbar slider positions */
	double scale;         /**< Scaling factor */
} ScrollInfo;

/** Menu window attributes */
#define MENU_WINDOW_WIDTH   42
#define MENU_WINDOW_HEIGHT  GRID_WINDOW_SIZE
#define MENU_LOGO_HEIGHT    16
#define MENU_BORDER_COLOR   COLOR_BLUE
#define MENU_BORDER_COLOR_S COLOR_MAROON

/** Menu color tiles attributes */
#define MENU_TILE_SIZE      7
#define MENU_TILE_HSEP      3
#define MENU_TILE_VSEP      2
#define MENU_TILE_COLUMNS   2
#define MENU_TILES_PER_COL  7
#define MENU_TILE_COUNT     (MENU_TILE_COLUMNS * MENU_TILES_PER_COL)
#define MENU_TILES_WIDTH    (MENU_TILE_COLUMNS*MENU_TILE_SIZE + (MENU_TILE_COLUMNS-1)*MENU_TILE_HSEP)
#define MENU_TILES_HEIGHT   (MENU_TILES_PER_COL*MENU_TILE_SIZE + (MENU_TILES_PER_COL+1)*MENU_TILE_VSEP+2)

/** Menu buttons attributes */
#define MENU_COMMANDS_POS   88
#define MENU_BUTTON_WIDTH   11
#define MENU_BUTTON_HEIGHT  7
#define MENU_PLAY_COLOR     COLOR_GREEN
#define MENU_PAUSE_COLOR    COLOR_YELLOW
#define MENU_CLEAR_COLOR    COLOR_TEAL

/** Escape key literal for input handling */
#define KEY_ESC 0x1B

/** Structure containing all relevant menu settings */
typedef struct settings {
	Colors *colors;
	size_t init_size;
	Simulation *linked_sim;
} Settings;

/** Dialog window attributes */
#define DIALOG_TILE_SIZE     3
#define DIALOG_TILE_ROWS     3
#define DIALOG_TILE_COLS     5
#define DIALOG_BUTTON_WIDTH  7
#define DIALOG_BUTTON_HEIGHT 3
#define DIALOG_BUTTON_COLOR  COLOR_YELLOW
#define DIALOG_DELETE_COLOR  COLOR_RED
#define DIALOG_WINDOW_WIDTH  (DIALOG_TILE_COLS*DIALOG_TILE_SIZE + 2)
#define DIALOG_WINDOW_HEIGHT (DIALOG_TILE_ROWS*DIALOG_TILE_SIZE + DIALOG_BUTTON_HEIGHT*2 + 4)

/** Designates which colors are to be set in the dialog */
#define CIDX_NEWCOLOR -1
#define CIDX_DEFAULT  -2

/** Globals */
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

/**
 * @brief Initializes graphics and all windows
 * @param fg_color Foreground color
 * @param bg_color Background color
 */
void init_graphics(short fg_color, short bg_color);

/**
 * @brief Closes windows and ends drawing
 */
void end_graphics(void);

/**
 * @brief Initializes all color pairs (c[0..15], bg_color)
 * @param fg_color Foreground color
 * @param bg_color Background color
 */
void init_def_pairs(short fg_color, short bg_color);

/**
 * @brief Main draw/update loop
 */
void draw_loop(void);

/**
 * @brief Stops the main draw/update loop
 * @param exit Should loop stop drawing?
 */
void exit_draw_loop(bool exit);

/**
 * @brief Utility function for drawing square boxes
 * @param w Window to draw to
 * @param top_left Box origin
 * @param size Box size
 */
void draw_box(WINDOW *w, Vector2i top_left, size_t size);

/**
 * @brief Utility function for drawing rectangular boxes
 * @param w Window to draw to
 * @param top_left Box origin
 * @param width Box width
 * @param height Box height
 */
void draw_rect(WINDOW *w, Vector2i top_left, size_t width, size_t height);

/**
 * @brief Utility function for drawing monochrome bitmaps
 * @param w Window to draw to
 * @param top_left Box origin
 * @param bitmap Bitmap to be drawn (byte array)
 * @param width Box width
 * @param height Box height
 * @param overwrite Should existing content be overwritten?
 */
void draw_bitmap(WINDOW *w, Vector2i top_left,
				 const unsigned char *bitmap, size_t width, size_t height,
				 bool overwrite);

/**
 * @brief Utility function for turning relative coords to absolute
 * @param rel Relative vector
 * @param origin Point of reference
 * @return Absolute vector
 */
Vector2i rel2abs(Vector2i rel, Vector2i origin);

/**
 * @brief Utility function for turning absolute coords to relative
 * @param abs Absolute vector
 * @param origin Point of reference
 * @return Relative vector
 */
Vector2i abs2rel(Vector2i abs, Vector2i origin);

/**
 * @brief Checks if a vector is contained within an area
 * @param top_left Area origin
 * @param width Area width
 * @param height Area height
 * @param needle Vector to be checked
 * @return Does area contain the vector?
 */
bool area_contains(Vector2i top_left, size_t width, size_t height, Vector2i needle);

/**
 * @brief Standard signum function
 * @param x Number whose sign is to be taken
 * @return Sign of x
 */
int sgn(int x);


/* grid_window.c */

/**
 * @brief Initializes grid window and related components
 */
void init_grid_window(void);

/**
 * @brief Closes grid window and ends drawing
 */
void end_grid_window(void);

/**
 * @brief Draws the entire grid (the portion shown by gridscrl)
 * @param grid Grid from which to draw
 */
void draw_grid_full(Grid *grid);

/**
 * @brief Draws the given pixel in the grid (the portion shown by gridscrl)
 * @param grid Grid from which to draw
 * @param oldp Position of pixel that has changed and should be drawn
 */
void draw_grid_iter(Grid *grid, Vector2i oldp);

/**
 * @brief Scrolls the grid relative to the current gridscrl position
 * @param grid Grid from which to draw
 * @param dy Relative y offset
 * @param dx Relative x offset
 */
void scroll_grid(Grid *grid, int dy, int dx);

/**
 * @brief Sets the absolute scroll of gridscrl
 * @param y Absolute y offset
 * @param x Absolute x offset
 */
void set_scroll(Grid *grid, int y, int x);

/**
* @brief Resets gridscrl to its initial state
*/
void reset_scroll(void);


/* grid_controls.c */

/**
 * @brief Handles key command passed to the grid window
 * @param grid Grid to be acted upon
 * @param ant Ant to be acted upon
 * @param key Key passed to the grid
 * @return key if successful; ERR otherwise
 */
int grid_key_command(Grid *grid, Ant *ant, int key);

/**
 * @brief Handles mouse command passed to the grid window
 * @param grid Grid to be acted upon
 */
void grid_mouse_command(Grid *grid);


/* menu_window.c */

/**
 * @brief Initializes menu window and related components
 */
void init_menu_window(void);

/**
 * @brief Closes menu window and ends drawing
 */
void end_menu_window(void);

/**
 * @brief Draws the menu window
 */
void draw_menu(void);

/**
 * @brief Finds the relative pos of a color tile in the menu
 * @param index Index in the color list
 * @return Relative pos of found tile
 */
Vector2i get_menu_tile_pos(int index);


/* menu_controls.c */

/**
 * @brief Handles key command passed to the menu
 * @param key Key passed to the grid
 * @return key if successful; ERR otherwise
 */
int menu_key_command(int key);

/**
 * @brief Handles mouse command passed to the menu
 */
void menu_mouse_command(void);


/* dialog.c */

/**
 * @brief Opens a temporary dialog window for picking colors
 * @param pos Dialog origin relative to menu
 * @param color_index Index of the color that is to be set
 */
void open_dialog(Vector2i pos, int color_index);

/**
 * @brief Closes dialog and ends drawing
 */
void close_dialog(void);

/**
 * @brief Draws the dialog window
 */
void draw_dialog(void);

/**
 * @brief Finds the relative pos of a color tile in the dialog
 * @param index Color index
 * @return Relative pos of found tile
 */
Vector2i get_dialog_tile_pos(int index);

/**
 * @brief Finds the relative pos of a button in the dialog
 * @param index Button index
 * @return Relative pos of found button
 */
Vector2i get_dialog_button_pos(int index);

/**
 * @brief Handles mouse command passed to the dialog
 * @param event Mouse event
 */
void dialog_mouse_command(MEVENT event);

/** @} */

#endif
