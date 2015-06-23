#include <math.h>
#include <string.h>
#include "graphics.h"

WINDOW *menuw;
Settings stgs = { .init_size = 4 };
const Vector2i menu_pos = { 0, MENU_WINDOW_HEIGHT };
const Vector2i menu_isz_u_pos = { MENU_LOGO_WIDTH,   MENU_WINDOW_WIDTH-7 };
const Vector2i menu_isz_d_pos = { MENU_LOGO_WIDTH+3, MENU_WINDOW_WIDTH-7 };

const char *isz_msg    = "INITIAL SIZE:";
const char *steps_msg  = "STEPS:";
const char *tiles_msg  = "RULES:";
const char *size_msg   = "GRID SIZE:";
const char *sparse_msg = "SPARSE MATRIX";

const Vector2i isz_pos        = { MENU_LOGO_WIDTH+3,     MENU_WINDOW_WIDTH-5 };
const Vector2i isz_msg_pos    = { MENU_LOGO_WIDTH+3,     MENU_WINDOW_WIDTH-21 };
const Vector2i steps_pos      = { MENU_WINDOW_HEIGHT-8,  MENU_WINDOW_WIDTH-5 };
const Vector2i steps_msg_pos  = { MENU_WINDOW_HEIGHT-4,  2 };
const Vector2i tiles_pos      = { MENU_LOGO_WIDTH+5,     MENU_TILE_SIZE+MENU_TILE_HSEP+4 };
const Vector2i tiles_msg_pos  = { MENU_LOGO_WIDTH,       2 };
const Vector2i sparse_msg_pos = { MENU_WINDOW_HEIGHT-12, 2 };
const Vector2i size_pos       = { MENU_WINDOW_HEIGHT-10, MENU_WINDOW_WIDTH-2 };
const Vector2i size_msg_pos   = { MENU_WINDOW_HEIGHT-10, 2 };

//unsigned char logo_bitmap[] = {
//	0xE0, 0x00, 0x04, 0x00, 0x10, 0x40, 0x00, 0x04, 0x00,
//	0x13, 0x41, 0xDC, 0x77, 0x33, 0x84, 0x42, 0x52, 0x94,
//	0x4A, 0x47, 0x4A, 0x52, 0x94, 0x4A, 0x41, 0xF9, 0xD2,
//	0x73, 0x32, 0x46, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00,
//	0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
//};

const unsigned char digit_bitmaps[][2] = {
	{ 0xF6, 0xDE }, { 0x24, 0x92 }, { 0xE7, 0xCE }, { 0xE7, 0x9E }, { 0xB7, 0x92 },
	{ 0xF3, 0x9E }, { 0xF3, 0xDE }, { 0xE4, 0x92 }, { 0xF7, 0xDE }, { 0xF7, 0x9E }
};

const unsigned char inf_bitmap[] = {
	0x00, 0x00, 0x03, 0x8e, 0x00, 0x00, 0x04, 0x51, 0x00, 0x00,
	0x04, 0x21, 0x00, 0x00, 0x04, 0x51, 0x00, 0x00, 0x03, 0x8e
};

void init_menu_window(void)
{
	menuw = newwin(MENU_WINDOW_HEIGHT, MENU_WINDOW_WIDTH, menu_pos.y, menu_pos.x);
	wbkgd(menuw, bg_pair);
	keypad(menuw, TRUE);
	nodelay(menuw, TRUE);
}

void end_menu_window(void)
{
	delwin(menuw);
	menuw = NULL;
}

Vector2i get_menu_tile_pos(int index)
{
	int index_y, index_x;
	Vector2i pos;

	assert(index >= 0 && index < MENU_TILE_COUNT);

	index_y = index % MENU_TILES_PER_COL;
	index_x = index / MENU_TILES_PER_COL;
	if (index_x % 2) {
		index_y = MENU_TILES_PER_COL - index_y - 1;
	}

	pos.y = tiles_pos.y + index_y*(MENU_TILE_SIZE+MENU_TILE_VSEP);
	pos.x = tiles_pos.x - index_x*(MENU_TILE_SIZE+MENU_TILE_HSEP);

	return pos;
}

static void draw_color_arrow(Vector2i pos1, Vector2i pos2)
{
	int ts = MENU_TILE_SIZE, o = MENU_TILE_SIZE/2, dy, dx;
	
	assert(pos1.y != pos2.y || pos1.x != pos2.x);
	wattrset(menuw, fg_pair);
	
	if (pos1.x == pos2.x) {
		dy = abs(pos1.y - pos2.y) - ts;
		mvwvline(menuw, min(pos1.y, pos2.y)+ts, pos1.x+o, ACS_VLINE, dy);
		if (pos1.y < pos2.y) {
			mvwaddch(menuw, pos2.y-1, pos1.x+o, ACS_DARROW);
		} else {
			mvwaddch(menuw, pos2.y+ts, pos1.x+o, ACS_UARROW);
		}
	} else if (pos1.y == pos2.y) {
		dx = abs(pos1.x - pos2.x);
		dy = MENU_TILE_VSEP;
		if (pos1.x > pos2.x) {
			mvwvline(menuw, pos1.y+ts,    pos1.x+o, ACS_VLINE, dy);
			mvwhline(menuw, pos1.y+ts+dy, pos2.x+o, ACS_HLINE, dx);
			mvwvline(menuw, pos1.y+ts,    pos2.x+o, ACS_VLINE, dy);
			mvwaddch(menuw, pos1.y+ts+dy, pos1.x+o, ACS_LRCORNER);
			mvwaddch(menuw, pos1.y+ts+dy, pos2.x+o, ACS_LLCORNER);
			mvwaddch(menuw, pos1.y+ts,    pos2.x+o, ACS_UARROW);
		} else {
			mvwvline(menuw, pos1.y-dy-1, pos1.x+o, ACS_VLINE, dy+1);
			mvwhline(menuw, pos1.y-dy-1, pos1.x+o, ACS_HLINE, dx);
			mvwvline(menuw, pos1.y-dy-1, pos2.x+o, ACS_VLINE, dy+1);
			mvwaddch(menuw, pos1.y-dy-1, pos1.x+o, ACS_ULCORNER);
			mvwaddch(menuw, pos1.y-dy-1, pos2.x+o, ACS_URCORNER);
			mvwaddch(menuw, pos1.y-1,    pos2.x+o, ACS_DARROW);
		}
	} else {
		dy = pos1.y - pos2.y;
		mvwvline(menuw, pos2.y, pos1.x+o, ACS_VLINE, dy);
		pos1.y = pos2.y;
		draw_color_arrow(pos1, pos2);
	}
}

static void draw_color_tile(Vector2i top_left, short c)
{
	chtype pair;
	bool is_def = c == stgs.colors->def;
	int y = top_left.y, x = top_left.x, s = MENU_TILE_SIZE;
	short fg;

	/* Draw tile */
	wattrset(menuw, pair = get_pair_for(c));
	if (is_def) {
		wattron(menuw, A_REVERSE);
	}
	draw_box(menuw, top_left, s);

	/* Draw direction arrow*/
	if (!is_def) {
		fg = AVAILABLE_FG_COLOR(get_color_for(bg_pair), c, COLOR_GRAY);
		if (fg == c) {
			wattron(menuw, A_REVERSE);
		} else {
			wattrset(menuw, get_pair_for(fg));
		}
		mvwaddch(menuw, y+s/2, x+s/2, (stgs.colors->turn[c] == TURN_LEFT) ? '<' : '>');
	}

	/* Draw frame */
	wattrset(menuw, is_def ? pair : fg_pair);
	mvwhline(menuw, y,     x,     ACS_BLOCK, s);
	mvwvline(menuw, y,     x,     ACS_BLOCK, s);
	mvwhline(menuw, y+s-1, x,     ACS_BLOCK, s);
	mvwvline(menuw, y,     x+s-1, ACS_BLOCK, s);
}

static void draw_color_list(void)
{
	short c, i = 0;
	bool do_for = TRUE;
	Vector2i pos1, pos2;

	if (!stgs.colors) {
		return;
	}

	/* Draw color tiles */
	for (c = stgs.colors->first; do_for; c = stgs.colors->next[c]) {
		pos1 = pos2 = get_menu_tile_pos(i++);
		if (c == COLOR_EMPTY) {
			break;
		}
		pos2 = get_menu_tile_pos(i);
		draw_color_arrow(pos1, pos2);
		draw_color_tile(pos1, c);
		do_for = c != stgs.colors->last;
	}

	/* Draw placeholder tile */
	draw_color_tile(pos2, stgs.colors->def);

	/* Draw arrow back to first tile */
	if (i >= MENU_TILES_PER_COL) {
		draw_color_arrow(pos2, get_menu_tile_pos(0));
	} else {
		pos1.y = pos2.y, pos1.x = pos2.x-MENU_TILE_SIZE-MENU_TILE_HSEP;
		draw_color_arrow(pos2, pos1);
		pos1.y += MENU_TILE_SIZE+1;
		draw_color_arrow(pos1, get_menu_tile_pos(0));
	}
}

static void draw_size(void)
{
	char size_str[29];
	int len = (int)log10(stgs.size)+1;
	wattrset(menuw, get_pair_for(MENU_BORDER_COLOR));
	sprintf(size_str, "%28d", stgs.size);
	mvwaddstr(menuw, size_pos.y, size_pos.x-28, size_str);
}

static void draw_init_size(void)
{
	assert(stgs.init_size >= 2 && stgs.init_size <= 6);
	wattrset(menuw, get_pair_for(MENU_BORDER_COLOR));
	mvwaddch(menuw, menu_isz_u_pos.y,   menu_isz_u_pos.x, ACS_UARROW);
	mvwaddch(menuw, menu_isz_u_pos.y+1, menu_isz_u_pos.x, ACS_VLINE);
	mvwaddch(menuw, menu_isz_d_pos.y,   menu_isz_d_pos.x, ACS_VLINE);
	mvwaddch(menuw, menu_isz_d_pos.y+1, menu_isz_d_pos.x, ACS_DARROW);
	wattrset(menuw, fg_pair);
	draw_bitmap(menuw, isz_pos, digit_bitmaps[stgs.init_size], 3, 5);
}

static void draw_steps(void)
{
	static bool do_draw = TRUE;
	char digits_str[9], *p;
	int digit, len = (int)log10(stgs.steps)+1;
	Vector2i top_left = steps_pos;

	if (!do_draw) {
		return;
	}

	wattrset(menuw, fg_pair);
	if (len >= 9) {
		top_left.x -= 29;
		draw_bitmap(menuw, top_left, inf_bitmap, 32, 5);
		do_draw = FALSE;
		return;
	}

	sprintf(digits_str, "%8d", stgs.steps);
	for (p = digits_str+7; p >= digits_str && *p != ' '; --p) {
		digit = *p - '0';
		draw_bitmap(menuw, top_left, digit_bitmaps[digit], 3, 5);
		top_left.x -= 4;
	}
}

void draw_menu(void)
{
	size_t h = MENU_WINDOW_WIDTH, v = MENU_WINDOW_HEIGHT;

	if (stgs.is_sparse) {
		wattrset(menuw, get_pair_for(MENU_BORDER_COLOR_S));
		mvwaddstr(menuw, sparse_msg_pos.y, sparse_msg_pos.x, sparse_msg);
	} else {
		wattrset(menuw, get_pair_for(MENU_BORDER_COLOR));
	}
	mvwhline(menuw, 0,   0,   ACS_BLOCK, h);
	mvwvline(menuw, 0,   0,   ACS_BLOCK, v);
	mvwhline(menuw, v-1, 0,   ACS_BLOCK, h);
	mvwvline(menuw, 0,   h-1, ACS_BLOCK, v);

	wattrset(menuw, get_pair_for(MENU_BORDER_COLOR));
	mvwaddstr(menuw, isz_msg_pos.y,   isz_msg_pos.x,   isz_msg);
	mvwaddstr(menuw, tiles_msg_pos.y, tiles_msg_pos.x, tiles_msg);
	mvwaddstr(menuw, size_msg_pos.y,  size_msg_pos.x,  size_msg);
	mvwaddstr(menuw, steps_msg_pos.y, steps_msg_pos.x, steps_msg);

	//wattron(menuw, A_REVERSE);
	//draw_bitmap(menuw, (Vector2i) { 1, 1 }, logo_bitmap, 40, 9);

	draw_init_size();
	draw_color_list();
	draw_size();
	draw_steps();

	wnoutrefresh(menuw);
}
