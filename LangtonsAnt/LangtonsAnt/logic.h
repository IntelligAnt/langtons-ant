/**
 * @file logic.h
 * File containing all logic members and functions
 * @author IntelligAnt
 */
#ifndef __LOGIC_H__
#define __LOGIC_H__

#include <assert.h>
#include <stddef.h>

///@{
/** Curses boolean literal */
#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif
///@}

/** Curses boolean type */
typedef unsigned char bool;

/** Vector container */
typedef struct vector2i {         ///@{
	int y, x; /**< Coordinates */ ///@}
} Vector2i;

/** Ant directions enum */
typedef enum { DIR_UP, DIR_RIGHT, DIR_DOWN, DIR_LEFT } Direction;

/** Ant container */
typedef struct ant {
	Vector2i pos;
	Direction dir;
} Ant;

/** @name Color struct constants */
///@{
#define COLOR_COUNT 16
#define COLOR_NONE  -1
#define TURN_LEFT   -1
#define TURN_NONE   0
#define TURN_RIGHT  1
///@}

/** @name Color utility macros */
///@{
#define COLOR_NEXT(cs, c) (cs)->next[c]
#define COLOR_TURN(cs, c) (cs)->turn[c]
///@}

/** Colors container */
typedef struct colors {
	short next[COLOR_COUNT], turn[COLOR_COUNT], n, first, last, def; // TODO Finish logic docs
} Colors;

/** @name Grid attributes and types */
///{@
#define GRID_MUL                3
#define GRID_SIZE_THRESHOLD     19682 // 3^9 - 1
#define GRID_USAGE_THRESHOLD    0.5
#define GRID_DEF_INIT_SIZE      4
#define GRID_MAX_INIT_SIZE      6
#define GRID_MIN_INIT_SIZE      2
#define GRID_MAX_SILENT_EXPAND  (GRID_SIZE_THRESHOLD + 1) // TODO add a dynamic silent expand step

#define GRID_SIZE_SMALL(g)      (g)->init_size // 2, 3, 4, 5, 6
#define GRID_SIZE_MEDIUM(g)     (GRID_SIZE_SMALL(g) * GRID_MUL)
#define GRID_SIZE_LARGE(g)      (GRID_SIZE_MEDIUM(g) * GRID_MUL)
#define IS_GRID_LARGE(g)        ((g)->size >= GRID_SIZE_LARGE(g))
#define GRID_EFFICIENCY(g)      ((g)->size*(g)->size / ((double)sizeof(Cell)*(g)->colored))
#define GRID_COLOR_AT(g, p)     (is_grid_sparse(g) ? color_at_s(g, p) : (g)->c[(p).y][(p).x])
///@}

/** @name Sparse matrix bit packing macros */
///@{
#define CELL_COLOR_MASK         (0xF << 28)
#define CELL_GET_COLOR(c)	    (((c)->col_packed & CELL_COLOR_MASK) >> 28)
#define CELL_SET_COLOR(c, col)	((c)->col_packed = (c)->col_packed & ~CELL_COLOR_MASK | ((col) << 28))
#define CELL_GET_COLUMN(c)	    ((c)->col_packed & ~CELL_COLOR_MASK)
#define CELL_SET_COLUMN(c, col)	((c)->col_packed = (c)->col_packed & CELL_COLOR_MASK | (col) & ~CELL_COLOR_MASK)
///@}

/** Sparse matrix cell container */
typedef struct cell {
	size_t col_packed;
	struct cell *next;
} Cell;

/** Grid container */
typedef struct grid {
	unsigned char **c, **tmp, def_color;
	Cell **csr;
	size_t init_size, size, tmp_size, colored;
	Vector2i top_left, bottom_right;
} Grid;

/** Simulation attributes and types */
typedef struct simulation {
	Colors *colors;
	Grid *grid;
	Ant *ant;
	size_t steps;
	bool is_running;
} Simulation;


/* ant.c */

Ant *ant_new(Grid *grid, Direction dir);
void ant_delete(Ant *ant);
bool ant_move(Ant *ant, Grid *grid, Colors *colors);
bool is_ant_out_of_bounds(Ant *ant, Grid *grid);


/* colors.c */

Colors *colors_new(short def);
void colors_delete(Colors *colors);
void add_color(Colors *colors, short c, short turn);
void remove_color(Colors *colors, short c);
void remove_all_colors(Colors *colors);
void set_color(Colors *colors, short index, short c, short turn);
void set_turn(Colors *colors, short index, short turn);
short get_color_at(Colors *colors, int index);
bool color_exists(Colors *colors, short c);
bool is_color_special(Colors *colors, short c);
bool is_colors_empty(Colors *colors);
bool has_enough_colors(Colors *colors);


/* grid.c */

Grid *grid_new(Colors *colors, size_t init_size);
void grid_delete(Grid *grid);
void grid_silent_expand(Grid*);
void grid_expand(Grid *grid, Ant *ant);
void grid_make_sparse(Grid *grid);
bool is_grid_sparse(Grid *grid);
void new_cell(Cell **cur, unsigned column, unsigned char c);
unsigned char color_at_s(Grid *grid, Vector2i p);


/* simulation.c */

Simulation *simulation_new(Colors *colors, size_t init_size);
void simulation_delete(Simulation *sim);
void run_simulation(Simulation *sim);
void halt_simulation(Simulation *sim);
bool is_simulation_running(Simulation *sim);
bool has_simulation_started(Simulation *sim);

#endif
