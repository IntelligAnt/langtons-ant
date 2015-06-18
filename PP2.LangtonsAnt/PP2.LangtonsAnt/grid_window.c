#include <assert.h>
#include <stdlib.h>
#include "graphics.h"

WINDOW *gridw = NULL;
ScrollInfo gridscrl; // TODO implement scrolling
extern chtype fg_pair, bg_pair;

void init_grid_window(void)
{
	gridw = newwin(GRID_WINDOW_SIZE, GRID_WINDOW_SIZE, 0, 0);
	gridscrl = (ScrollInfo){
		.y = 0,
		.x = 0,
		.hcenter = 0,
		.vcenter = 0,
		.scale = 1
	};
	keypad(gridw, TRUE);
}

void end_grid_window(void)
{
	delwin(gridw);
	gridw = NULL;
}

static Vector2i pos2yx(Vector2i pos, int line_width, int cell_size, int offset)
{
	return (Vector2i){
		.y = offset + line_width*(pos.y+1) + cell_size*pos.y,
		.x = offset + line_width*(pos.x+1) + cell_size*pos.x
	};
}

static void draw_block(Vector2i top_left, int size)
{
	int i;
	if (size == 1) {
		mvwaddch(gridw, top_left.y, top_left.x, GRID_CELL);
		return;
	}
	for (i = 0; i < size; ++i) {
		mvwhline(gridw, top_left.y+i, top_left.x, GRID_CELL, size);
	}
}

static void adjust_scrollbars(Grid *grid)
{
	static bool first = TRUE;
	static size_t prev_size;
	if (first) {
		prev_size = grid->size;
		first = FALSE;
	} else if (grid->size > prev_size) {
		gridscrl.scale /= GRID_MUL;
		prev_size = grid->size;
	}
}

static void draw_scrollbars(chtype sb_fg_pair, chtype sb_bg_pair)
{
	const int n = GRID_WINDOW_SIZE-1, mid = n/2;
	int size = (int)((n-2)*gridscrl.scale);
	int h = mid + gridscrl.hcenter - size/2;
	int v = mid + gridscrl.vcenter - size/2;
	/* Scrollbar background */
	wattrset(gridw, sb_bg_pair);
	mvwhline(gridw, n, 0, GRID_CELL, n);
	mvwvline(gridw, 0, n, GRID_CELL, n);
	/* Scrollbar arrows */
	wattron(gridw, A_REVERSE);
	mvwaddch(gridw, n,   0,   ACS_LARROW);
	mvwaddch(gridw, n,   n-1, ACS_RARROW);
	mvwaddch(gridw, 0,   n,   ACS_UARROW);
	mvwaddch(gridw, n-1, n,   ACS_DARROW);
	/* Scrollbars themselves */
	wattrset(gridw, sb_fg_pair);
	mvwhline(gridw, n, h, GRID_CELL, size);
	mvwvline(gridw, v, n, GRID_CELL, size);
}

static void bordered(Grid *grid, int line_width)
{
	int gs = grid->size, n = GRID_WINDOW_SIZE, i, j;
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
			wattrset(gridw, get_pair_for(GRID_COLOR_AT(grid, pos)));
			draw_block(yx, cs);
		}
	}
}

static void borderless(Grid *grid)
{
	int gs = min(grid->size, GRID_WINDOW_SIZE-1), i, j;
	int cs = CELL_SIZE(gs, 0);
	int total = TOTAL_SIZE(gs, 0, cs);
	int o = OFFSET_SIZE(total);
	Vector2i pos, yx;

	/* Draw scrollbars in case of largest grid */
	if (cs == 1) {
		//if (total == GRID_WINDOW_SIZE) {
		//	total = TOTAL_SIZE(gs-1, 0, cs);
		//	o = OFFSET_SIZE(total);
		//}
		adjust_scrollbars(grid);
		draw_scrollbars(get_pair_for(COLOR_SILVER), get_pair_for(COLOR_GRAY));
	}

	/* Draw cells */
	for (i = 0; i < gs; ++i) {
		for (j = 0; j < gs; ++j) {
			pos.y = i, pos.x = j;
			yx = pos2yx(pos, 0, cs, o);
			pos.y += gridscrl.y, pos.x += gridscrl.x;
			wattrset(gridw, get_pair_for(GRID_COLOR_AT(grid, pos)));
			draw_block(yx, cs);
		}
	}
}

void draw_grid_full(Grid *grid)
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

void draw_grid_iter(Grid *grid, Vector2i oldp, short newc, Vector2i newp)
{
	int gs = grid->size, i;
	int lw = (gs == GRID_SIZE_SMALL)  ? LINE_WIDTH_SMALL
		   : (gs == GRID_SIZE_MEDIUM) ? LINE_WIDTH_MEDIUM
		   : LINE_WIDTH_LARGE;
	int cs = CELL_SIZE(gs, lw);
	int o = OFFSET_SIZE(TOTAL_SIZE(gs, lw, cs));

	assert(gridw);

	wattrset(gridw, get_pair_for(newc));
	draw_block(oldp, cs);
	// TODO draw ant transition
}
