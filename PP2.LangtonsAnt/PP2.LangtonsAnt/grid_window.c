#include <assert.h>
#include "graphics.h"

WINDOW *gridw = NULL;
extern short fg_pair, bg_pair;

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

static Vector2i get_cell_yx()
{
	// TODO
}

static void bordered(Grid *grid, int grid_size, int line_width)
{
	int n = GRID_WINDOW_SIZE, i, j;
	int cell = CELL_SIZE(grid_size, line_width);
	int total = TOTAL_SIZE(grid_size, cell, line_width);
	int o = (n - total) / 2;
	
	/* Draw white edge buffer zone */
	wattrset(gridw, COLOR_PAIR(bg_pair));
	for (i = 0; i < GRID_BUFFER_ZONE; ++i) {
		mvwhline(gridw, i,     i,     GRID_CELL, n-i);
		mvwhline(gridw, n-1-i, i,     GRID_CELL, n-i);
		mvwvline(gridw, i,     i,     GRID_CELL, n-i);
		mvwvline(gridw, i,     n-1-i, GRID_CELL, n-i);
	}

	/* Draw grid lines */
	wattrset(gridw, COLOR_PAIR(fg_pair));
	for (i = 0; i < n+1; i += cell+line_width) {
		for (j = 0; j < line_width; ++j) {
			mvwhline(gridw, o+i+j, o,     GRID_CELL, total);
			mvwvline(gridw, o,     o+i+j, GRID_CELL, total);
		}
	}

	/* Draw cells */


	wrefresh(gridw);
}

static void borderless(Grid *grid)
{
	// TODO
}

void grid_draw_full(Grid *grid)
{
	int y, x;

	assert(gridw);
	if (grid) {
		switch (grid->size) {
		case GRID_SIZE_SMALL:
			bordered(grid, GRID_SIZE_SMALL, LINE_WIDTH_SMALL);
			break;
		case GRID_SIZE_MEDIUM:
			bordered(grid, GRID_SIZE_MEDIUM, LINE_WIDTH_MEDIUM);
			break;
		default:
			assert(IS_GRID_LARGE(grid->size));
			borderless(grid);
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
