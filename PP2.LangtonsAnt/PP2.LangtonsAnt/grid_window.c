#include <assert.h>
#include "graphics.h"

WINDOW *gridw = NULL;
extern chtype fg_pair, bg_pair;

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

static Vector2i get_cell_yx(Vector2i pos, int cell_size, int line_width, int offset)
{
	Vector2i yx = {
		.y = offset + line_width*(pos.y+1) + cell_size*pos.y,
		.x = offset + line_width*(pos.x+1) + cell_size*pos.x
	};
	return yx;
}

static void draw_block(Vector2i top_left, int size)
{
	int i;
	for (i = 0; i < size; ++i) {
		mvwhline(gridw, top_left.y+i, top_left.x, GRID_CELL, size);
	}
}

static void bordered(Grid *grid, int line_width)
{
	int n = GRID_WINDOW_SIZE, grid_size = grid->size, i, j;
	int cell_size = CELL_SIZE(grid_size, line_width);
	int total = TOTAL_SIZE(grid_size, cell_size, line_width);
	int o = (n - total) / 2;
	Vector2i yx;
	
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
	for (i = 0; i < n+1; i += cell_size+line_width) {
		for (j = 0; j < line_width; ++j) {
			mvwhline(gridw, o+i+j, o,     GRID_CELL, total);
			mvwvline(gridw, o,     o+i+j, GRID_CELL, total);
		}
	}
	
	/* Draw cells */
	for (i = 0; i < grid_size; ++i) {
		for (j = 0; j < grid_size; ++j) {
			yx = get_cell_yx((Vector2i){ i, j }, cell_size, line_width, o);
			wattrset(gridw, get_pair_for(grid->c[i][j]));
			draw_block(yx, cell_size);
		}
	}
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
			bordered(grid, LINE_WIDTH_SMALL);
			break;
		case GRID_SIZE_MEDIUM:
			bordered(grid, LINE_WIDTH_MEDIUM);
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
