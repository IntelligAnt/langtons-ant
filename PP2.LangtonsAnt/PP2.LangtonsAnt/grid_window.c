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

static Vector2i pos2yx(Vector2i pos, int line_width, int cell_size, int offset)
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
		wrefresh(gridw);
	}
}

static void bordered(Grid *grid, int line_width)
{
	int n = GRID_WINDOW_SIZE, gs = grid->size, i, j;
	int cs = CELL_SIZE(gs, line_width);
	int total = TOTAL_SIZE(gs, line_width, cs);
	int o = OFFSET_SIZE(total);
	Vector2i pos, yx;

	/* Draw background edge buffer zone */
	assert(o < GRID_BUFFER_ZONE);
	wattrset(gridw, COLOR_PAIR(bg_pair));
	for (i = 0; i < GRID_BUFFER_ZONE; ++i) {
		mvwhline(gridw, i,     i,     GRID_CELL, n-i);
		mvwhline(gridw, n-1-i, i,     GRID_CELL, n-i);
		mvwvline(gridw, i,     i,     GRID_CELL, n-i);
		mvwvline(gridw, i,     n-1-i, GRID_CELL, n-i);
	}

	/* Draw grid lines */
	wattrset(gridw, COLOR_PAIR(fg_pair));
	for (i = 0; i < n+1; i += cs+line_width) {
		for (j = 0; j < line_width; ++j) {
			mvwhline(gridw, o+i+j, o,     GRID_CELL, total);
			mvwvline(gridw, o,     o+i+j, GRID_CELL, total);
		}
	}
	
	/* Draw cells */
	for (i = 0; i < gs; ++i) {
		for (j = 0; j < gs; ++j) {
			pos.y = i, pos.x = j;
			yx = pos2yx(pos, line_width, cs, o);
			wattrset(gridw, get_pair_for(grid_get_color(grid, pos))); // TODO get color macro?
			draw_block(yx, cs);
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

void grid_draw_iter(Grid *grid, Vector2i oldp, short newc, Vector2i newp)
{
	int gs = grid->size, i;
	int lw = (gs == GRID_SIZE_SMALL)  ? LINE_WIDTH_SMALL
		   : (gs == GRID_SIZE_MEDIUM) ? LINE_WIDTH_MEDIUM
		   : LINE_WIDTH_LARGE;
	int cs = CELL_SIZE(gs, lw);
	int o = OFFSET_SIZE(TOTAL_SIZE(gs, lw, cs));

	assert(gridw);

	/* Draw */
	wattrset(gridw, grid_get_color(grid, oldp));
	draw_block(oldp, cs);

}
