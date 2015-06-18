#include <assert.h>
#include <stdlib.h>
#include "graphics.h"

WINDOW *gridw = NULL;
ScrollInfo gridscrl; // TODO implement scrolling
bool is_scrl_on;
extern chtype fg_pair, bg_pair;

void init_grid_window(void)
{
	gridw = newwin(GRID_WINDOW_SIZE, GRID_WINDOW_SIZE, 0, 0);
	gridscrl = (ScrollInfo) {
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
	return (Vector2i) {
		.y = offset + line_width*(pos.y+1) + cell_size*pos.y,
		.x = offset + line_width*(pos.x+1) + cell_size*pos.x
	};
}

static Vector2i rel2abs(Vector2i rel, Vector2i origin)
{
	return (Vector2i) {
		.y = origin.y + rel.y,
		.x = origin.x + rel.x
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

static void draw_buffer_zone(int width)
{
	const int n = GRID_WINDOW_SIZE;
	int i;
	for (i = 0; i < width; ++i) {
		mvwhline(gridw, i, i, GRID_CELL, n-i);
		mvwhline(gridw, n-1-i, i, GRID_CELL, n-i);
		mvwvline(gridw, i, i, GRID_CELL, n-i);
		mvwvline(gridw, i, n-1-i, GRID_CELL, n-i);
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
	int size = (int)(max((n-2)*gridscrl.scale, SLIDER_MIN_SIZE));
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

	/* Scrollbar sliders */
	wattrset(gridw, sb_fg_pair);
	mvwhline(gridw, n, h, GRID_CELL, size);
	mvwvline(gridw, v, n, GRID_CELL, size);
}

static void bordered(Grid *grid, int line_width)
{
	int gs = grid->size, i, j;
	int cs = CELL_SIZE(gs, line_width);
	int total = TOTAL_SIZE(gs, line_width, cs);
	int o = OFFSET_SIZE(total);
	Vector2i pos, yx;

	/* Draw background edge buffer zone */
	assert(o <= GRID_BUFFER_ZONE);
	wattrset(gridw, COLOR_PAIR(bg_pair));
	draw_buffer_zone(GRID_BUFFER_ZONE);

	/* Draw grid lines */
	wattrset(gridw, COLOR_PAIR(fg_pair));
	for (i = 0; i < GRID_WINDOW_SIZE; i += cs+line_width) {
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
	Vector2i pos, yx, origin = { 0, 0 };

	/* Draw scrollbars in case of largest grid */
	is_scrl_on = cs == 1;
	if (is_scrl_on) {
		wattrset(gridw, get_pair_for(grid->def_color));
		draw_buffer_zone(o+1);
		origin = (Vector2i) { // TODO correct slight shift with init size 2 grid
			.y = grid->size/2 - gs/2 + gridscrl.y,
			.x = grid->size/2 - gs/2 + gridscrl.x
		};
		adjust_scrollbars(grid);
		draw_scrollbars(get_pair_for(COLOR_SILVER), get_pair_for(COLOR_GRAY));
	} else {
		assert(o <= GRID_BUFFER_ZONE);
		wattrset(gridw, COLOR_PAIR(bg_pair));
		draw_buffer_zone(GRID_BUFFER_ZONE);
	}

	/* Draw cells */
	for (i = 0; i < gs; ++i) {
		for (j = 0; j < gs; ++j) {
			pos.y = i, pos.x = j;
			yx = pos2yx(pos, 0, cs, o);
			wattrset(gridw, get_pair_for(GRID_COLOR_AT(grid, rel2abs(pos, origin))));
			draw_block(yx, cs);
		}
	}
}

void draw_grid_full(Grid *grid)
{
	int i;

	assert(gridw);
	if (grid) {
		if (grid->size == GRID_SIZE_SMALL(grid)){
			bordered(grid, LINE_WIDTH_SMALL);
		} else if (grid->size == GRID_SIZE_MEDIUM(grid)){
			bordered(grid, LINE_WIDTH_MEDIUM);
		} else {
			assert(IS_GRID_LARGE(grid));
			borderless(grid);
		}
	} else {
		wattrset(gridw, COLOR_PAIR(bg_pair));
		for (i = 0; i < GRID_WINDOW_SIZE; ++i) {
			mvwhline(gridw, i, 0, GRID_CELL, GRID_WINDOW_SIZE);
		}
	}

	wrefresh(gridw);
}

void draw_grid_iter(Grid *grid, Vector2i oldp, short newc, Vector2i newp)
{
	int gs = grid->size;
	int lw = (gs == GRID_SIZE_SMALL(grid))  ? LINE_WIDTH_SMALL
		   : (gs == GRID_SIZE_MEDIUM(grid)) ? LINE_WIDTH_MEDIUM
		   : LINE_WIDTH_LARGE;
	int cs = CELL_SIZE(gs, lw);
	int o = OFFSET_SIZE(TOTAL_SIZE(gs, lw, cs));

	assert(gridw);

	wattrset(gridw, get_pair_for(newc));
	draw_block(oldp, cs);
	// TODO draw ant transition
}
