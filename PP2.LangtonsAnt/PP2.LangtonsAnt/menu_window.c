#include <math.h>
#include <string.h>
#include "graphics.h"

WINDOW *menuw;
Settings stgs;

const Vector2i menu_pos = { 0, GRID_WINDOW_SIZE };
const Vector2i menu_isz_u_pos = { MENU_LOGO_HEIGHT,   MENU_WINDOW_WIDTH-9 };
const Vector2i menu_isz_d_pos = { MENU_LOGO_HEIGHT+3, MENU_WINDOW_WIDTH-9 };
const Vector2i menu_play_pos  = { MENU_CONTROLS_POS,  2 };
const Vector2i menu_pause_pos = { MENU_CONTROLS_POS,  MENU_BUTTON_WIDTH+4 };
const Vector2i menu_clear_pos = { MENU_CONTROLS_POS,  2*MENU_BUTTON_WIDTH+6 };
const Vector2i menu_load_pos  = { MENU_CONTROLS_POS-2*MENU_BUTTON_HEIGHT-4,
                                  MENU_WINDOW_WIDTH-MENU_BUTTON_WIDTH-3 };
const Vector2i menu_save_pos  = { MENU_CONTROLS_POS-MENU_BUTTON_HEIGHT-2,
                                  MENU_WINDOW_WIDTH-MENU_BUTTON_WIDTH-3 };

const char *tiles_msg  = "RULES:";
const char *isz_msg    = "INIT SIZE:";
const char *sparse_msg = "SPARSE MATRIX";
const char *size_msg   = "GRID SIZE:";
const char *steps_msg  = "STEPS:";

const Vector2i tiles_pos      = { MENU_LOGO_HEIGHT+5,    MENU_TILE_SIZE+MENU_TILE_HSEP+4 };
const Vector2i tiles_msg_pos  = { MENU_LOGO_HEIGHT,      2 };
const Vector2i isz_pos        = { MENU_LOGO_HEIGHT,      MENU_WINDOW_WIDTH-5 };
const Vector2i isz_msg_pos    = { MENU_LOGO_HEIGHT,      MENU_WINDOW_WIDTH-20 };
const Vector2i status_msg_pos = { MENU_WINDOW_HEIGHT-12, 2 };
const Vector2i size_pos       = { MENU_WINDOW_HEIGHT-10, MENU_WINDOW_WIDTH-2 };
const Vector2i size_msg_pos   = { MENU_WINDOW_HEIGHT-10, 2 };
const Vector2i steps_pos      = { MENU_WINDOW_HEIGHT-8,  9 };
const Vector2i steps_msg_pos  = { MENU_WINDOW_HEIGHT-4,  2 };

const unsigned char logo_bitmap[] = {
	0x1C, 0x00, 0x00, 0x80, 0x00, 0x08, 0x00, 0x00,
	0x80, 0x00, 0x08, 0x3B, 0x8E, 0xE6, 0x70, 0x08,
	0x4A, 0x52, 0x89, 0x48, 0x09, 0x4A, 0x52, 0x89,
	0x48, 0x1F, 0x3A, 0x4E, 0x66, 0x48, 0x00, 0x00,
	0x02, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00
};
const unsigned char isz_bitmaps[][1] = {
	{ 0x5C }, { 0xE8 }
};
const unsigned char button_bitmaps[][3] = {
	{ 0x43, 0x1C, 0xC4 }, { 0x52, 0x94, 0xA5 }, { 0x47, 0x92, 0x17 }
};
const unsigned char digit_bitmaps[][2] = {
	{ 0xF6, 0xDE }, { 0x24, 0x92 }, { 0xE7, 0xCE }, { 0xE7, 0x9E }, { 0xB7, 0x92 },
	{ 0xF3, 0x9E }, { 0xF3, 0xDE }, { 0xE4, 0x92 }, { 0xF7, 0xDE }, { 0xF7, 0x9E }
};
const unsigned char inf_bitmap[] = {
	0x00, 0x00, 0x07, 0x1C, 0x00, 0x00, 0x11, 0x44, 0x00, 0x00,
	0x21, 0x08, 0x00, 0x00, 0x45, 0x10, 0x00, 0x00, 0x71, 0xC0
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
	wattrset(menuw, pair = GET_PAIR_FOR(c));
	if (is_def) {
		wattron(menuw, A_REVERSE);
	}
	draw_box(menuw, top_left, s);

	/* Draw direction arrow */
	if (!is_def) {
		fg = AVAILABLE_COLOR(GET_COLOR_FOR(bg_pair), c, COLOR_GRAY);
		if (fg == c) {
			wattron(menuw, A_REVERSE);
		} else {
			wattrset(menuw, GET_PAIR_FOR(fg));
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

	pos1.y = tiles_pos.y - MENU_TILE_VSEP - 1;
	pos1.x = tiles_pos.x - MENU_TILE_HSEP - MENU_TILE_SIZE;
	wattrset(menuw, bg_pair);
	draw_rect(menuw, pos1, MENU_TILES_WIDTH, MENU_TILES_HEIGHT);
	
	if (!stgs.colors) {
		return;
	}

	/* Draw color tiles */
	for (c = stgs.colors->first; do_for; c = stgs.colors->next[c]) {
		pos1 = pos2 = get_menu_tile_pos(i++);
		if (c == COLOR_NONE) {
			break;
		}
		if (i < MENU_TILE_COUNT) {
			pos2 = get_menu_tile_pos(i);
			draw_color_arrow(pos1, pos2);
		}
		draw_color_tile(pos1, c);
		do_for = c != stgs.colors->last;
	}
	
	/* Draw placeholder tile */
	if (i < MENU_TILE_COUNT) {
		draw_color_tile(pos2, stgs.colors->def);
	}

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

static void draw_init_size(void)
{
	if (stgs.init_size < GRID_MIN_INIT_SIZE || stgs.init_size > GRID_MAX_INIT_SIZE) {
		return;
	}
	wattrset(menuw, GET_PAIR_FOR(MENU_BORDER_COLOR));
	draw_bitmap(menuw, menu_isz_u_pos, isz_bitmaps[0], 3, 2, FALSE);
	draw_bitmap(menuw, menu_isz_d_pos, isz_bitmaps[1], 3, 2, FALSE);
	wattrset(menuw, fg_pair);
	draw_bitmap(menuw, isz_pos, digit_bitmaps[stgs.init_size], 3, 5, TRUE);
}

static void draw_control_buttons(void)
{
	short bg = AVAILABLE_COLOR(GET_COLOR_FOR(bg_pair), COLOR_SILVER, COLOR_WHITE);
	Vector2i o = { (MENU_BUTTON_HEIGHT-5)/2, (MENU_BUTTON_WIDTH-5)/2 };
	Vector2i pos1 = { menu_play_pos.y + o.y, menu_play_pos.x + o.x };
	Vector2i pos2 = { menu_pause_pos.y + o.y, menu_pause_pos.x + o.x };
	Vector2i pos3 = { menu_clear_pos.y + o.y, menu_clear_pos.x + o.x };

	wattrset(menuw, GET_PAIR_FOR(bg));
	draw_rect(menuw, menu_play_pos, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT);
	draw_rect(menuw, menu_pause_pos, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT);
	draw_rect(menuw, menu_clear_pos, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT);

	wattrset(menuw, GET_PAIR_FOR(MENU_PLAY_COLOR));
	draw_bitmap(menuw, pos1, button_bitmaps[0], 5, 5, FALSE);

	wattrset(menuw, GET_PAIR_FOR(MENU_PAUSE_COLOR));
	draw_bitmap(menuw, pos2, button_bitmaps[1], 5, 5, FALSE);

	wattrset(menuw, GET_PAIR_FOR(MENU_CLEAR_COLOR));
	draw_bitmap(menuw, pos3, button_bitmaps[2], 5, 5, FALSE);
}

static void draw_io_buttons(void)
{
	short bg = AVAILABLE_COLOR(GET_COLOR_FOR(bg_pair), COLOR_SILVER, COLOR_WHITE);
	Vector2i inner1 = { menu_load_pos.y+1, menu_load_pos.x+1 };
	Vector2i inner2 = { menu_save_pos.y+1, menu_save_pos.x+1 };

	wattrset(menuw, GET_PAIR_FOR(bg));
	draw_rect(menuw, menu_load_pos, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT);
	draw_rect(menuw, menu_save_pos, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT);
	wattron(menuw, A_REVERSE);
	draw_rect(menuw, inner1, MENU_BUTTON_WIDTH-2, MENU_BUTTON_HEIGHT-2);
	draw_rect(menuw, inner2, MENU_BUTTON_WIDTH-2, MENU_BUTTON_HEIGHT-2);

	wattrset(menuw, fg_pair);
	mvwaddstr(menuw, inner1.y+1, inner1.x+1, "LOAD");
	mvwaddstr(menuw, inner1.y+2, inner1.x+2, "FROM");
	mvwaddstr(menuw, inner1.y+3, inner1.x+4, "FILE");
	mvwaddstr(menuw, inner2.y+1, inner2.x+1, "SAVE");
	mvwaddstr(menuw, inner2.y+2, inner2.x+3, "TO");
	mvwaddstr(menuw, inner2.y+3, inner2.x+4, "FILE");
}

static void draw_size(void)
{
	Simulation *sim = stgs.linked_sim;
	size_t size = is_simulation_valid(sim) ? sim->grid->size : 0;
	char size_str[29];
	sprintf(size_str, "%28d", size);
	wattrset(menuw, GET_PAIR_FOR(MENU_BORDER_COLOR));
	mvwaddstr(menuw, size_pos.y, size_pos.x-28, size_str);
}

static void draw_steps(void)
{
	static bool do_draw;
	Simulation *sim = stgs.linked_sim;
	size_t steps = sim ? sim->steps : 0;
	int digit, len = (int)log10(steps) + 1;
	char digits_str[9], *p;
	Vector2i top_left = steps_pos;

	if (steps == 0) {
		do_draw = TRUE;
	} else if (!do_draw) {
		return;
	}

	wattrset(menuw, fg_pair);
	if (len > 8) {
		draw_bitmap(menuw, top_left, inf_bitmap, 31, 5, TRUE);
		do_draw = FALSE;
		return;
	}

	sprintf(digits_str, "%8d", steps);
	for (p = digits_str; p < digits_str+8; ++p) {
		if (*p != ' ') {
			digit = *p - '0';
			wattroff(menuw, A_REVERSE);
			draw_bitmap(menuw, top_left, digit_bitmaps[digit], 3, 5, TRUE);
		} else {
			wattron(menuw, A_REVERSE);
			draw_rect(menuw, top_left, 4, 5);
		}
		top_left.x += 4;
	}
}

void draw_menu(void)
{
	Simulation *sim = stgs.linked_sim;
	size_t h = MENU_WINDOW_WIDTH, v = MENU_WINDOW_HEIGHT;

	if (is_simulation_valid(sim) && is_grid_sparse(sim->grid)) {
		wattrset(menuw, GET_PAIR_FOR(MENU_BORDER_COLOR_S));
		mvwaddstr(menuw, status_msg_pos.y, status_msg_pos.x, sparse_msg);
	} else {
		wattrset(menuw, GET_PAIR_FOR(MENU_BORDER_COLOR));
		mvwhline(menuw, status_msg_pos.y, status_msg_pos.x, ' ', MENU_WINDOW_WIDTH-2);
	}
	mvwhline(menuw, 0,   0,   ACS_BLOCK, h);
	mvwvline(menuw, 0,   0,   ACS_BLOCK, v);
	mvwhline(menuw, v-1, 0,   ACS_BLOCK, h);
	mvwvline(menuw, 0,   h-1, ACS_BLOCK, v);

	wattrset(menuw, GET_PAIR_FOR(MENU_BORDER_COLOR));
	mvwaddstr(menuw, isz_msg_pos.y,   isz_msg_pos.x,   isz_msg);
	mvwaddstr(menuw, tiles_msg_pos.y, tiles_msg_pos.x, tiles_msg);
	mvwaddstr(menuw, size_msg_pos.y,  size_msg_pos.x,  size_msg);
	mvwaddstr(menuw, steps_msg_pos.y, steps_msg_pos.x, steps_msg);

	//draw_bitmap(menuw, (Vector2i) { 4, 1 }, logo_bitmap, 40, 8, FALSE);

	draw_init_size();
	draw_color_list();
	draw_control_buttons();
	draw_io_buttons();
	draw_size();
	draw_steps();

	wnoutrefresh(menuw);

	if (dialogw) {
		draw_dialog();
	}
}
