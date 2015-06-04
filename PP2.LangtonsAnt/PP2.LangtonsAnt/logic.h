#ifndef __LOGIC_H__
#define __LOGIC_H__

#define COLOR_COUNT 16

typedef unsigned char bool;

typedef enum { UP, RIGHT, DOWN, LEFT } Direction;

typedef struct vector2i {
	int y, x;
} Vector2i;

typedef struct ant {
	Vector2i p;
	Direction dir;
} Ant;

typedef struct colors {
	short next[COLOR_COUNT-1], turn[COLOR_COUNT-1], n, first, last, def;
} Colors;

typedef struct grid {
	unsigned char **c;
	unsigned size;
} Grid;

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
short color_next(Colors *colors, short c);
short color_turn(Colors *colors, short c);
bool color_exists(Colors *colors, short c);
bool enough_colors(Colors *colors);

/* grid.c */

#define GRID_MUL         3 // Do not change
#define GRID_SIZE_SMALL  2 // Allow user-changeable initial size
#define GRID_SIZE_MEDIUM (GRID_SIZE_SMALL * GRID_MUL)
#define GRID_SIZE_LARGE  (GRID_SIZE_MEDIUM * GRID_MUL)
#define IS_GRID_LARGE(s) (s >= GRID_SIZE_LARGE)

Grid *grid_new(unsigned size);
void grid_delete(Grid *grid);
void expand_grid(Grid *grid, Ant *ant);

#endif
