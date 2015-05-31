#ifndef __LOGIC_H__
#define __LOGIC_H__

#define COLOR_COUNT 16
#define INIT_SIZE 20

typedef enum { UP, RIGHT, DOWN, LEFT } Direction;

typedef struct colors {
	short turn[COLOR_COUNT - 1], next[COLOR_COUNT - 1], numb, first, last;
} Colors;

typedef struct vector2i {
	int x, y;
} Vector2i;

typedef struct grid{
	char **g;
	int size;
} Grid;

typedef struct ant {
	Vector2i v;
	Direction dir;
} Ant;

Grid *grid_new(int size);
Colors *init_colors(void);
int color_exists(Colors *colors, short c);
int new_color(Colors *colors, short c, short turn);
int delete_color(Colors *colors, short c);
int enough_colors(Colors *colors);
Ant *ant_new(Grid *grid, Direction dir);
int ant_move(Ant *ant, Grid *grid, Colors *colors);

#endif