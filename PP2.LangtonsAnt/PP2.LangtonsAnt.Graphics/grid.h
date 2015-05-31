#ifndef __GRID_H__
#define __GRID_H__

#include "logic.h"

void init_grid(void);

void grid_draw_full(Grid *grid);

void grid_draw_iter(Vector2i oldp, short oldc, Vector2i newp, short newc);

#endif
