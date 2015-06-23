#include <stdlib.h>
#include <math.h>
#include "graphics.h"

WINDOW *gridw;
ScrollInfo gridscrl;
const Vector2i grid_origin = { 0, 0 };

void init_grid_window(void)
{
	gridw = newwin(GRID_WINDOW_SIZE, GRID_WINDOW_SIZE, grid_origin.y, grid_origin.x);
	keypad(gridw, TRUE);
	nodelay(gridw, TRUE);
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

static void draw_buffer_zone(int width)
{
	const int n = GRID_WINDOW_SIZE;
	int i;
	for (i = 0; i < width; ++i) {
		mvwhline(gridw, i,     i,     ACS_BLOCK, n-i);
		mvwhline(gridw, n-1-i, i,     ACS_BLOCK, n-i);
		mvwvline(gridw, i,     i,     ACS_BLOCK, n-i);
		mvwvline(gridw, i,     n-1-i, ACS_BLOCK, n-i);
	}
}

static void draw_scrollbars(short sb_fg_color, short sb_bg_color)
{
	int n = GRID_VIEW_SIZE, mid = n/2;
	int size = (int)(max((n-2)*gridscrl.scale, SLIDER_MIN_SIZE));
	int h = mid + gridscrl.hcenter - size/2;
	int v = mid + gridscrl.vcenter - size/2;

	h -= h > mid;
	v -= v > mid;

	/* Scrollbar background */
	wattrset(gridw, get_pair_for(sb_bg_color));
	mvwhline(gridw, n, 0, ACS_BLOCK, n);
	mvwvline(gridw, 0, n, ACS_BLOCK, n);

	/* Scrollbar arrows */
	wattron(gridw, A_REVERSE);
	mvwaddch(gridw, 0,   n,   ACS_UARROW);
	mvwaddch(gridw, n-1, n,   ACS_DARROW);
	mvwaddch(gridw, n,   0,   ACS_LARROW);
	mvwaddch(gridw, n,   n-1, ACS_RARROW);

	/* Scrollbar sliders */
	wattrset(gridw, get_pair_for(sb_fg_color)); // TODO fix sliders drawing over right/bottom arrows
	mvwhline(gridw, n, h, ACS_BLOCK, size);
	mvwvline(gridw, v, n, ACS_BLOCK, size);
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
	wattrset(gridw, bg_pair);
	draw_buffer_zone(GRID_BUFFER_ZONE);

	/* Draw grid lines */
	wattrset(gridw, fg_pair);
	for (i = 0; i < GRID_WINDOW_SIZE; i += cs+line_width) {
		for (j = 0; j < line_width; ++j) {
			mvwhline(gridw, o+i+j, o,     ACS_BLOCK, total);
			mvwvline(gridw, o,     o+i+j, ACS_BLOCK, total);
		}
	}
	
	/* Draw cells */
	for (i = 0; i < gs; ++i) {
		for (j = 0; j < gs; ++j) {
			pos.y = i, pos.x = j;
			yx = pos2yx(pos, line_width, cs, o);
			wattrset(gridw, get_pair_for(GRID_COLOR_AT(grid, pos)));
			draw_box(gridw, yx, cs);
		}
	}
}

static void borderless(Grid *grid)
{
	int gs = grid->size, vgs = min(gs, GRID_VIEW_SIZE), i, j;
	int cs = CELL_SIZE(vgs, 0);
	int o = OFFSET_SIZE(TOTAL_SIZE(vgs, 0, cs));
	Vector2i pos, yx, origin = { 0, 0 };
	short sb_fg_color = AVAILABLE_FG_COLOR(grid->def_color, COLOR_WHITE);
	short sb_bg_color = AVAILABLE_FG_COLOR(grid->def_color, COLOR_GRAY);

	/* Draw background edge buffer zone */
	wattrset(gridw, get_pair_for(grid->def_color));
	draw_buffer_zone(o + 1);

	/* Draw scrollbars in case of largest grid */
	gridscrl.enabled = cs == 1;
	if (gridscrl.enabled) {
		origin = ORIGIN_POS(gs, vgs, gridscrl.y, gridscrl.x);
		gridscrl.scale = GRID_VIEW_SIZE / (double)gs;
		gridscrl.hcenter = (int)(gridscrl.scale * gridscrl.x);
		gridscrl.vcenter = (int)(gridscrl.scale * gridscrl.y);
		draw_scrollbars(sb_fg_color, sb_bg_color);
	}

	/* Draw cells */
	for (i = 0; i < vgs; ++i) {
		for (j = 0; j < vgs; ++j) {
			pos.y = i, pos.x = j;
			yx = pos2yx(pos, 0, cs, o);
			wattrset(gridw, get_pair_for(GRID_COLOR_AT(grid, rel2abs(pos, origin))));
			draw_box(gridw, yx, cs);
		}
	}
}

void draw_grid_full(Grid *grid)
{
	int i;

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
		wattrset(gridw, bg_pair);
		for (i = 0; i < GRID_WINDOW_SIZE; ++i) {
			mvwhline(gridw, i, 0, ACS_BLOCK, GRID_WINDOW_SIZE);
		}
	}

	wrefresh(gridw);
}

void draw_grid_iter(Grid *grid, Vector2i oldp, bool refresh)
{
	int gs = grid->size, vgs = min(gs, GRID_VIEW_SIZE);
	int lw = (gs == GRID_SIZE_SMALL(grid))  ? LINE_WIDTH_SMALL
		   : (gs == GRID_SIZE_MEDIUM(grid)) ? LINE_WIDTH_MEDIUM
		   : LINE_WIDTH_LARGE;
	int cs = CELL_SIZE(vgs, lw);
	int o = OFFSET_SIZE(TOTAL_SIZE(vgs, lw, cs));
	Vector2i origin = ORIGIN_POS(gs, vgs, gridscrl.y, gridscrl.x), pos, yx;

	pos = abs2rel(oldp, origin);
	yx = pos2yx(pos, lw, cs, o);

	if (yx.y < 0 || yx.y >= GRID_VIEW_SIZE || yx.x < 0 || yx.x >= GRID_VIEW_SIZE) {
		return;
	}

	wattrset(gridw, get_pair_for(GRID_COLOR_AT(grid, oldp)));
	draw_box(gridw, yx, cs);
	// TODO draw ant transition

	if (refresh) {
		wrefresh(gridw);
	}
}

void scroll_grid(Grid *grid, int dy, int dx)
{
	set_scroll(grid, gridscrl.y+dy, gridscrl.x+dx);
}

void set_scroll(Grid *grid, int y, int x)
{
	int gs = grid->size, n = GRID_VIEW_SIZE, clamp = max(gs/2-n/2, 0);

	if (!gridscrl.enabled) {
		return;
	}

	y = sgn(y) * min(abs(y), clamp);
	x = sgn(x) * min(abs(x), clamp);

	gridscrl.y = y;
	gridscrl.x = x;
	gridscrl.hcenter = (int)(gridscrl.scale * x);
	gridscrl.vcenter = (int)(gridscrl.scale * y);
}
