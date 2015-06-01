#ifndef __LOGIC_H__
#define __LOGIC_H__

#include "graphics.h"

#define INIT_SIZE 3

typedef enum { UP, RIGHT, DOWN, LEFT } Direction;

typedef struct vector2i {
	int x, y;
} Vector2i;

typedef struct ant {
	Vector2i p;
	Direction dir;
} Ant;

typedef struct colors {
	short next[COLOR_COUNT-1], turn[COLOR_COUNT-1], n, first, last;
} Colors;

typedef struct grid {
	unsigned char **c;
	unsigned size;
} Grid;

Ant *ant_new(Grid *grid, Direction dir);
void ant_delete(Ant *ant);
void ant_move(Ant *ant, Grid *grid, Colors *colors);
bool ant_out_of_bounds(Ant *ant, Grid *grid);

Colors *init_colors(void);
void new_color(Colors *colors, short c, short turn);
void delete_color(Colors *colors, short c);
void set_color(Colors *colors, short index, short c, short turn);
//short prev_color(Colors *colors, short c);
//short next_color(Colors *colors, short c);
bool color_exists(Colors *colors, short c);
bool enough_colors(Colors *colors);

/* grid_handler.c */
Grid *grid_new(unsigned size);
void grid_delete(Grid *grid);
void expand_grid(Grid *grid, Ant *ant);

#endif