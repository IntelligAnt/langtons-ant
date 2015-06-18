#ifndef __LOGIC_H__
#define __LOGIC_H__

#include <stddef.h>

#define COLOR_COUNT 16

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
	short next[COLOR_COUNT-1], turn[COLOR_COUNT-1], n, first, last, def;
} Colors;

#define COLOR_NEXT(cs, c) cs->next[c]
#define COLOR_TURN(cs, c) cs->turn[c]
#define ENOUGH_COLORS(cs) (cs->n >= 2)

/*** Grid attributes and types ***/

#define GRID_MUL         3 // Do not change
#define GRID_SIZE_SMALL  2 // Allow user-changeable initial size (2, 3, 5)
#define GRID_SIZE_MEDIUM (GRID_SIZE_SMALL  * GRID_MUL)
#define GRID_SIZE_LARGE  (GRID_SIZE_MEDIUM * GRID_MUL)
#define IS_GRID_LARGE(s) (s >= GRID_SIZE_LARGE)
#define GRID_COLOR_AT(g, p) g->c[p.y][p.x]

typedef struct grid {
	unsigned char **c;
	size_t size;
} Grid;

/* logic.c */

Ant *ant_new(Grid *grid, Direction dir);
void ant_delete(Ant *ant);
bool ant_move(Ant *ant, Grid *grid, Colors *colors);
bool ant_out_of_bounds(Ant *ant, Grid *grid);

Colors *init_colors(short def);
void new_color(Colors *colors, short c, short turn);
void delete_color(Colors *colors, short c);
void set_color(Colors *colors, short index, short c, short turn);
//short prev_color(Colors *colors, short c);
//short next_color(Colors *colors, short c);
bool color_exists(Colors *colors, short c);
bool enough_colors(Colors *colors);

/* grid.c */

Grid *grid_new(size_t size);
void grid_delete(Grid *grid);
void expand_grid(Grid *grid, Ant *ant);

#endif
