#ifndef __GRID_H__
#define __GRID_H__

#include "logic.h"

/*** Grid window attributes ***/

#define GRID_WINDOW_SIZE 82
#define GRID_BUFFER_ZONE 2
#define GRID_CELL ACS_BLOCK

void init_grid(void);

void grid_draw_full(Grid *grid);

void grid_draw_iter(Vector2i oldp, short oldc, Vector2i newp, short newc);

#endif
