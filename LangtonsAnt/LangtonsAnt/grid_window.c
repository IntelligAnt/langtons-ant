#include <math.h>
#include <stdlib.h>

#include "graphics.h"

WINDOW *gridw;
ScrollInfo gridscrl;
const Vector2i grid_pos = { 0, 0 };

void init_grid_window(void)
{
	gridw = newwin(GRID_WINDOW_SIZE, GRID_WINDOW_SIZE, grid_pos.y, grid_pos.x);
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

static void draw_buffer_zone(int total, int offset)
{
	int n = GRID_WINDOW_SIZE, i;
	for (i = 0; i < offset; ++i) {
		mvwhline(gridw, i,     i,     ACS_BLOCK, n-i);
		mvwvline(gridw, i,     i,     ACS_BLOCK, n-i);
		mvwhline(gridw, n-1-i, i,     ACS_BLOCK, n-i);
		mvwvline(gridw, i,     n-1-i, ACS_BLOCK, n-i);
	}
	if ((n-total-2*offset) % 2) {
		mvwhline(gridw, n-1-i, i, ACS_BLOCK, n-i);
		mvwvline(gridw, i, n-1-i, ACS_BLOCK, n-i);
	}
}

static void draw_scrollbars(short def)
{
	int n = GRID_VIEW_SIZE, mid = n/2, step = n-2;
	int size = (int)(max(step*gridscrl.scale, 1));
	int h = mid + gridscrl.hcenter - size/2;
	int v = mid + gridscrl.vcenter - size/2;
	short sb_fg_color = AVAILABLE_COLOR(def, COLOR_WHITE, COLOR_SILVER);
	short sb_bg_color = AVAILABLE_COLOR(def, COLOR_GRAY,  COLOR_SILVER);

	h -= h > mid;
	v -= v > mid;

	/* Scrollbar background */
	wattrset(gridw, GET_PAIR_FOR(sb_bg_color));
	mvwhline(gridw, n, 0, ACS_BLOCK, n);
	mvwvline(gridw, 0, n, ACS_BLOCK, n);

	/* Scrollbar arrows */
	wattron(gridw, A_REVERSE);
	mvwaddch(gridw, 0,   n,   ACS_UARROW);
	mvwaddch(gridw, n-1, n,   ACS_DARROW);
	mvwaddch(gridw, n,   0,   ACS_LARROW);
	mvwaddch(gridw, n,   n-1, ACS_RARROW);

	/* Scrollbar sliders */
	wattrset(gridw, GET_PAIR_FOR(sb_fg_color));
	mvwhline(gridw, n, h, ACS_BLOCK, size);
	mvwvline(gridw, v, n, ACS_BLOCK, size);
}

static void draw_cell(Vector2i yx, int cs, short c, Ant *ant)
{
	if (yx.y < 0 || yx.y >= GRID_VIEW_SIZE || yx.x < 0 || yx.x >= GRID_VIEW_SIZE) {
		return;
	}
	wattrset(gridw, GET_PAIR_FOR(c));
	draw_square(gridw, yx, cs);
	if (ant) {
		const byte *ant_sprite;
		if (ant_sprite = get_ant_sprite(cs, ant->dir)) {
			wattrset(gridw, fg_pair);
			draw_sprite(gridw, ant_sprite, yx, cs, cs, FALSE);
		} else {
			div_t d = div(ant->dir, 2);
			chtype arrow = (d.rem ? ACS_RARROW : ACS_UARROW) + d.quot; // Really awful hack
			mvwaddch(gridw, yx.y+cs/2, yx.x+cs/2, arrow | A_REVERSE);
		}
	}
}

static void bordered(Grid *grid, Ant *ant, int line_width)
{
	int gs = grid->size, i, j;
	int cs = CELL_SIZE(gs, line_width);
	int t = TOTAL_SIZE(gs, line_width, cs);
	int o = OFFSET_SIZE(t);
	Vector2i pos, yx;

	/* Draw background edge buffer zone */
	wattrset(gridw, bg_pair);
	draw_buffer_zone(t, o);

	/* Draw grid lines */
	wattrset(gridw, fg_pair);
	for (i = 0; i < GRID_WINDOW_SIZE; i += cs+line_width) {
		for (j = 0; j < line_width; ++j) {
			mvwhline(gridw, o+i+j, o,     ACS_BLOCK, t);
			mvwvline(gridw, o,     o+i+j, ACS_BLOCK, t);
		}
	}
	
	/* Draw cells */
	for (i = 0; i < gs; ++i) {
		for (j = 0; j < gs; ++j) {
			pos.y = i, pos.x = j;
			yx = pos2yx(pos, line_width, cs, o);
			draw_cell(yx, cs, GRID_COLOR_AT(grid, pos),
					  (ant && VECTOR_EQ(pos, ant->pos)) ? ant : NULL);
		}
	}
}

static void borderless(Grid *grid, Ant *ant)
{
	int gs = grid->size, vgs = min(gs, GRID_VIEW_SIZE), i, j;
	int cs = CELL_SIZE(vgs, 0);
	int t = TOTAL_SIZE(vgs, 0, cs);
	int o = OFFSET_SIZE(t);
	Vector2i rel, pos, yx, origin = grid_pos;

	/* Draw background edge buffer zone */
	wattrset(gridw, GET_PAIR_FOR(grid->def_color));
	draw_buffer_zone(t, o);

	/* Draw scrollbars in case of largest grid */
	gridscrl.enabled = gs > vgs;
	if (gridscrl.enabled) {
		origin = ORIGIN_POS(gs, vgs, gridscrl.y, gridscrl.x);
		gridscrl.scale = GRID_VIEW_SIZE / (double)gs;
		gridscrl.hcenter = (int)(gridscrl.scale * gridscrl.x);
		gridscrl.vcenter = (int)(gridscrl.scale * gridscrl.y);
		draw_scrollbars(grid->def_color);
	}

	/* Draw cells */
	for (i = 0; i < vgs; ++i) {
		for (j = 0; j < vgs; ++j) {
			rel.y = i, rel.x = j;
			yx = pos2yx(rel, 0, cs, o);
			pos = rel2abs(rel, origin);
			draw_cell(yx, cs, GRID_COLOR_AT(grid, pos),
					  (ant && VECTOR_EQ(pos, ant->pos)) ? ant : NULL);
		}
	}
}

void draw_grid_full(Grid *grid, Ant *ant)
{
	if (grid) {
		if (grid->size == GRID_SIZE_SMALL(grid)) {
			bordered(grid, ant, LINE_WIDTH_SMALL);
		} else if (grid->size == GRID_SIZE_MEDIUM(grid)) {
			bordered(grid, ant, LINE_WIDTH_MEDIUM);
		} else {
			assert(IS_GRID_LARGE(grid));
			borderless(grid, ant);
		}
	} else {
		int i;
		wattrset(gridw, bg_pair);
		for (i = 0; i < GRID_WINDOW_SIZE; ++i) {
			mvwhline(gridw, i, 0, ACS_BLOCK, GRID_WINDOW_SIZE);
		}
	}
	wnoutrefresh(gridw);
}

void draw_grid_iter(Grid *grid, Ant *ant, Vector2i oldp)
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
	draw_cell(yx, cs, GRID_COLOR_AT(grid, oldp), NULL);

	if (ant) {
		pos = abs2rel(ant->pos, origin);
		yx = pos2yx(pos, lw, cs, o);
		draw_cell(yx, cs, GRID_COLOR_AT(grid, ant->pos), ant);
	}
	
	wnoutrefresh(gridw);
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

void reset_scroll(void)
{
	if (gridscrl.enabled) {
		gridscrl.y = 0;
		gridscrl.x = 0;
		gridscrl.hcenter = 0;
		gridscrl.vcenter = 0;
		gridscrl.scale = 0.0;
	}
}
