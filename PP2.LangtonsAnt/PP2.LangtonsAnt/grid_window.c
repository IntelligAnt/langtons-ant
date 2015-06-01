#include "logic.h"
#include "graphics.h"

WINDOW *gridw = NULL;

void init_grid(void)
{
	// TODO
}

void grid_draw_full(Grid *grid)
{
	int y, x;

	if (!grid) {
		setattr(COLOR_PAIR(BG_PAIR));
		for (y = 0; y < GRID_WINDOW_SIZE; ++y) {
			for (x = 0; x < GRID_WINDOW_SIZE; ++x) {
				mvaddch(y, x, GRID_CELL);
			}
		}
		return;
	}

	// TODO
}

void grid_draw_iter(Vector2i oldp, short oldc, Vector2i newp, short newc)
{
	// TODO
}