#include <assert.h>

#include "graphics.h"

WINDOW *gridw = NULL;

void init_grid_window(void)
{
	gridw = newwin(GRID_WINDOW_SIZE, GRID_WINDOW_SIZE, 0, 0);
	keypad(gridw, TRUE);
}

void end_grid_window(void)
{
	delwin(gridw);
	gridw = NULL;
}

void grid_draw_full(Grid *grid)
{
	extern short bg_pair;
	int y, x;

	assert(gridw);
	if (grid) {
		// TODO
	} else {
		wattrset(gridw, COLOR_PAIR(bg_pair));
		for (y = 0; y < GRID_WINDOW_SIZE; ++y) {
			for (x = 0; x < GRID_WINDOW_SIZE; ++x) {
				mvwaddch(gridw, y, x, GRID_CELL);
			}
		}
	}

	wrefresh(gridw);
}

void grid_draw_iter(Vector2i oldp, short oldc, Vector2i newp, short newc)
{
	// TODO
}
