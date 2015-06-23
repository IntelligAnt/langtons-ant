#ifndef __LOGIC_H__
#define __LOGIC_H__

#include <stddef.h>

#define DEBUG 1
#if DEBUG
#include <assert.h>
#else
#define assert(e) 1
#endif

#define COLOR_COUNT 16
#define COLOR_EMPTY -1
#define TURN_LEFT -1
#define TURN_RIGHT 1

#define COLOR_NEXT(cs, c) (cs)->next[c]
#define COLOR_TURN(cs, c) (cs)->turn[c]

typedef unsigned char bool;

typedef struct vector2i {
	int y, x;
} Vector2i;

typedef enum { UP, RIGHT, DOWN, LEFT } Direction;

typedef struct ant {
	Vector2i pos;
	Direction dir;
} Ant;

typedef struct colors {
	short next[COLOR_COUNT], turn[COLOR_COUNT], n, first, last, def;
} Colors;

/*** Grid attributes and types ***/

#define GRID_MUL                3
#define GRID_SIZE_THRESHOLD     19682 // 3^9 - 1
#define GRID_USAGE_THRESHOLD    0.5
#define GRID_MAX_SILENT_EXPAND  (GRID_SIZE_THRESHOLD + 1) // TODO add a dynamic silent expand step

#define GRID_SIZE_SMALL(g)      (g)->init_size // 2, 3, 4, 5, 6
#define GRID_SIZE_MEDIUM(g)     (GRID_SIZE_SMALL(g) * GRID_MUL)
#define GRID_SIZE_LARGE(g)      (GRID_SIZE_MEDIUM(g) * GRID_MUL)
#define IS_GRID_LARGE(g)        ((g)->size >= GRID_SIZE_LARGE(g))
#define GRID_EFFICIENCY(g)      ((g)->size*(g)->size / ((double)sizeof(Cell)*(g)->used))
#define GRID_COLOR_AT(g, p)     (is_grid_sparse(g) ? color_at_s(g, p) : (g)->c[(p).y][(p).x])

#define CELL_COLOR_MASK         (0xF << 28)
#define CELL_GET_COLOR(c)	    (((c)->col_packed & CELL_COLOR_MASK) >> 28)
#define CELL_SET_COLOR(c, col)	((c)->col_packed = (c)->col_packed & ~CELL_COLOR_MASK | ((col) << 28))
#define CELL_GET_COLUMN(c)	    ((c)->col_packed & ~CELL_COLOR_MASK)
#define CELL_SET_COLUMN(c, col)	((c)->col_packed = (c)->col_packed & CELL_COLOR_MASK | (col) & ~CELL_COLOR_MASK)

typedef struct cell {
	size_t col_packed;
	struct cell *next;
} Cell;

typedef struct grid {
	unsigned char **c, **tmp, def_color;
	Cell **csr;
	size_t size, init_size, tmp_size, used;
	Vector2i top_left, bottom_right;
} Grid;

/* logic.c */

Ant *ant_new(Grid *grid, Direction dir);
void ant_delete(Ant *ant);
bool ant_move(Ant *ant, Grid *grid, Colors *colors);
bool is_ant_out_of_bounds(Ant *ant, Grid *grid);

Colors *colors_new(short def);
void colors_delete(Colors *colors);
void add_color(Colors *colors, short c, short turn);
void remove_color(Colors *colors, short c);
void set_color(Colors *colors, short index, short c, short turn);
bool color_exists(Colors *colors, short c);
bool has_enough_colors(Colors *colors);

/* grid.c */

Grid *grid_new(size_t size, Colors *colors);
void grid_delete(Grid *grid);
void grid_silent_expand(Grid*);
void grid_expand(Grid *grid, Ant *ant);
void grid_make_sparse(Grid *grid);
bool is_grid_sparse(Grid *grid);
void new_cell(Cell **cur, unsigned column, unsigned char c);
unsigned char color_at_s(Grid *grid, Vector2i p);

/* simulation.c */

void run_simulation(Ant *ant, Grid *grid, Colors *colors);
void stop_simulation(void);

#endif
