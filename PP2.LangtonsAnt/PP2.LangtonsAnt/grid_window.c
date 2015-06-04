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

static void grid_draw_small(Grid *grid)
{

}

static void grid_draw_medium(Grid *grid)
{

}

static void grid_draw_large(Grid *grid)
{

}

void grid_draw_full(Grid *grid)
{
	extern short bg_pair;
	int y, x;

	assert(gridw);
	if (grid) {
		switch (grid->size) {
		case GRID_SIZE_SMALL:
			grid_draw_small(grid);
			break;
		case GRID_SIZE_MEDIUM:
			grid_draw_medium(grid);
			break;
		default:
			assert(IS_GRID_LARGE(grid->size));
			grid_draw_large(grid);
		}
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
