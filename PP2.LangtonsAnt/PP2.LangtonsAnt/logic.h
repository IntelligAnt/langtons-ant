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
	short next[COLOR_COUNT], turn[COLOR_COUNT], n, first, last, def;
} Colors;

#define COLOR_NEXT(cs, c) cs->next[c]
#define COLOR_TURN(cs, c) cs->turn[c]
#define ENOUGH_COLORS(cs) (cs->n >= 2)

/*** Grid attributes and types ***/

#define GRID_MUL 3 // Do not change
#define GRID_SIZE_SMALL(g) (g)->init_size  // Allow user-changeable initial size (2, 3, 5)
#define GRID_SIZE_MEDIUM(g) (GRID_SIZE_SMALL(g) * GRID_MUL)
#define GRID_SIZE_LARGE(g) (GRID_SIZE_MEDIUM(g) * GRID_MUL)
#define IS_GRID_LARGE(g) (g->size >= GRID_SIZE_LARGE(g))
#define GRID_COLOR_AT(g, p) (is_sparse(g) ? get_color_s(g, p) : (g)->c[(p).y][(p).x])

typedef struct element {
	size_t column;
	unsigned char c;
	struct element *next;
}Element;

typedef struct grid {
	unsigned char **c, def_color;
	Element **rows /*, **last_visit*/; //TO DO
	size_t size, init_size;
} Grid;

/* logic.c */

Ant *ant_new(Grid *grid, Direction dir);
void ant_delete(Ant *ant);
bool ant_out_of_bounds(Ant *ant, Grid *grid);
bool ant_move(Ant *ant, Grid *grid, Colors *colors);


Colors *init_colors(short def);
void new_color(Colors *colors, short c, short turn);
void delete_color(Colors *colors, short c);
void set_color(Colors *colors, short index, short c, short turn);
bool color_exists(Colors *colors, short c);
bool enough_colors(Colors *colors);

/* grid.c */

Grid *grid_new(size_t size, Colors *colors);
void new_element(Element **cur, unsigned column, unsigned char c);
void grid_to_sparse(Grid *grid);
bool is_sparse(Grid *grid);
void grid_delete(Grid *grid);
void grid_expand(Grid *grid, Ant *ant);
unsigned char get_color_s(Grid *grid, Vector2i p);

#endif
