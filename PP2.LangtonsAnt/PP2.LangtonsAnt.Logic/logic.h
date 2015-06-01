#ifndef __LOGIC_H__
#define __LOGIC_H__

#include "graphics.h"

#define INIT_SIZE 3

typedef enum { UP, RIGHT, DOWN, LEFT } Direction;

typedef struct colors {
	short next[COLOR_COUNT-1], turn[COLOR_COUNT-1], n, first, last;
} Colors;

typedef struct vector2i {
	int x, y;
} Vector2i;

typedef struct grid {
	unsigned char **c;
	unsigned size;
} Grid;

typedef struct ant {
	Vector2i p;
	Direction dir;
} Ant;

Colors *init_colors(void);
void new_color(Colors *colors, short c, short turn);
void delete_color(Colors *colors, short c);
void set_color(Colors *colors, short index, short c, short turn);
//short prev_color(Colors *colors, short c);
//short next_color(Colors *colors, short c);
bool color_exists(Colors *colors, short c);
bool enough_colors(Colors *colors);

Ant *ant_new(Grid *grid, Direction dir);
bool ant_move(Ant *ant, Grid *grid, Colors *colors);
//int ant_overboard(Ant *ant, Grid *grid);

Grid *grid_new(unsigned size);
void expand_grid(Grid *grid, Ant *ant);

#endif