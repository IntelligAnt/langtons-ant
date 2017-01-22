#include <math.h>
#include <string.h>

#include "graphics.h"

WINDOW *menuw;
Settings stgs;
IOStatus load_status, save_status;

const Vector2i menu_pos = { 0, GRID_WINDOW_SIZE };
const Vector2i menu_isz_u_pos = { MENU_LOGO_HEIGHT+1,   MENU_WINDOW_WIDTH-9 };
const Vector2i menu_isz_d_pos = { MENU_LOGO_HEIGHT+4,   MENU_WINDOW_WIDTH-9 };
const Vector2i menu_dir_u_pos = { MENU_DIRECTION_POS,   MENU_WINDOW_WIDTH-8 };
const Vector2i menu_dir_r_pos = { MENU_DIRECTION_POS+2, MENU_WINDOW_WIDTH-4 };
const Vector2i menu_dir_d_pos = { MENU_DIRECTION_POS+5, MENU_WINDOW_WIDTH-8 };
const Vector2i menu_dir_l_pos = { MENU_DIRECTION_POS+2, MENU_WINDOW_WIDTH-11 };
const Vector2i menu_play_pos  = { MENU_CONTROLS_POS,    2 };
const Vector2i menu_pause_pos = { MENU_CONTROLS_POS,    MENU_BUTTON_WIDTH+4 };
const Vector2i menu_stop_pos  = { MENU_CONTROLS_POS,    2*MENU_BUTTON_WIDTH+6 };
const Vector2i menu_load_pos  = { MENU_CONTROLS_POS-2*MENU_BUTTON_HEIGHT-4,
								                        MENU_WINDOW_WIDTH-MENU_BUTTON_WIDTH-3 };
const Vector2i menu_save_pos  = { MENU_CONTROLS_POS-MENU_BUTTON_HEIGHT-2,
                                                        MENU_WINDOW_WIDTH-MENU_BUTTON_WIDTH-3 };

static const char *logo_msg   = " 14-COLOR 2D TURING MACHINE SIMULATOR ";
static const char *tiles_msg  = "RULES:";
static const char *isz_msg    = "INIT SIZE:";
static const char *dir_msg    = "DIRECTION:";
static const char *sparse_msg = "SPARSE MATRIX";
static const char *size_msg   = "GRID SIZE:";
static const char *steps_msg  = "STEPS:";

static const Vector2i logo_pos       = { 3,  1 };
static const Vector2i logo_msg_pos   = { 12, 2 };
static const Vector2i tiles_pos      = { MENU_LOGO_HEIGHT+6,    MENU_TILE_SIZE+MENU_TILE_HSEP+3 };
static const Vector2i tiles_msg_pos  = { MENU_LOGO_HEIGHT+1,    2 };
static const Vector2i isz_pos        = { MENU_LOGO_HEIGHT+1,    MENU_WINDOW_WIDTH-5 };
static const Vector2i isz_msg_pos    = { MENU_LOGO_HEIGHT+1,    MENU_WINDOW_WIDTH-20 };
static const Vector2i dir_msg_pos    = { MENU_DIRECTION_POS,    MENU_WINDOW_WIDTH-19 };
static const Vector2i status_msg_pos = { MENU_WINDOW_HEIGHT-12, 2 };
static const Vector2i size_pos       = { MENU_WINDOW_HEIGHT-10, MENU_WINDOW_WIDTH-2 };
static const Vector2i size_msg_pos   = { MENU_WINDOW_HEIGHT-10, 2 };
static const Vector2i steps_pos      = { MENU_WINDOW_HEIGHT-8,  9 };
static const Vector2i steps_msg_pos  = { MENU_WINDOW_HEIGHT-4,  2 };

static const byte logo_sprite[] = {
	0x70, 0x00, 0x02, 0x00, 0x10, 0x20, 0x00, 0x02,
	0x00, 0x10, 0x20, 0xEE, 0x3B, 0x99, 0xC6, 0x21,
	0x29, 0x4A, 0x25, 0x24, 0x25, 0x29, 0x4A, 0x25,
	0x22, 0x7C, 0xE9, 0x39, 0x99, 0x26, 0x00, 0x00,
	0x08, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00
};
static const byte logo_ant_sprite[] = {
	0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x02,
	0x00, 0x00, 0x00, 0xEE, 0x03, 0x80, 0x00, 0x01,
	0x29, 0x02, 0x00, 0x00, 0x01, 0x29, 0x02, 0x00,
	0x00, 0x00, 0xE9, 0x01, 0x80, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
static const byte uarrow_sprite[] = { 0x5C };
static const byte darrow_sprite[] = { 0xE8 };
static const byte larrow_sprite[] = { 0xB8 };
static const byte rarrow_sprite[] = { 0x74 };
static const byte button_sprites[][4] = {
	{ 0x43, 0x1C, 0xC4, 0x00 }, { 0x02, 0x94, 0xA0, 0x00 },
	{ 0x03, 0x9C, 0xE0, 0x00 }, { 0x47, 0x92, 0x17, 0x00 }
};
static const byte digit_sprites[][2] = {
	{ 0xF6, 0xDE }, { 0x24, 0x92 }, { 0xE7, 0xCE }, { 0xE7, 0x9E }, { 0xB7, 0x92 },
	{ 0xF3, 0x9E }, { 0xF3, 0xDE }, { 0xE4, 0x92 }, { 0xF7, 0xDE }, { 0xF7, 0x9E }
};
static const byte inf_sprite[] = {
	0x00, 0x00, 0x07, 0x1C, 0x00, 0x00, 0x11, 0x44, 0x00, 0x00,
	0x21, 0x08, 0x00, 0x00, 0x45, 0x10, 0x00, 0x00, 0x71, 0xC0
};

void init_menu_window(void)
{
	menuw = newwin(MENU_WINDOW_HEIGHT, MENU_WINDOW_WIDTH, menu_pos.y, menu_pos.x);
	wbkgd(menuw, fg_pair);
	keypad(menuw, TRUE);
	nodelay(menuw, TRUE);
}

void end_menu_window(void)
{
	delwin(menuw);
	menuw = NULL;
}

Vector2i get_menu_tile_pos(size_t index)
{
	Vector2i pos;
	size_t index_y, index_x;

	assert(index < MENU_TILE_COUNT);

	index_y = index % MENU_TILES_PER_COL;
	index_x = index / MENU_TILES_PER_COL;
	if (index_x % 2) {
		index_y = MENU_TILES_PER_COL - index_y - 1;
	}

	pos.y = tiles_pos.y + index_y*(MENU_TILE_SIZE+MENU_TILE_VSEP);
	pos.x = tiles_pos.x - index_x*(MENU_TILE_SIZE+MENU_TILE_HSEP);

	return pos;
}

Vector2i get_menu_cdef_pos(void)
{
	return (Vector2i) {
		.y = get_menu_tile_pos(min(stgs.colors->n, MENU_TILES_PER_COL)).y + MENU_TILE_SIZE + MENU_TILE_VSEP + 1,
		.x = tiles_pos.x - MENU_TILE_SIZE - MENU_TILE_HSEP + 1
	};
}

static void draw_edge(void)
{
	Simulation *sim = stgs.linked_sim;
	size_t h = MENU_WINDOW_WIDTH, v = MENU_WINDOW_HEIGHT;

	if (sim && is_grid_sparse(sim->grid)) {
		wattrset(menuw, GET_PAIR_FOR(MENU_EDGE_COLOR_S));
		mvwaddstr(menuw, status_msg_pos.y, status_msg_pos.x, sparse_msg);
	} else {
		wattrset(menuw, GET_PAIR_FOR(MENU_EDGE_COLOR));
		mvwhline(menuw, status_msg_pos.y, status_msg_pos.x, ' ', strlen(sparse_msg));
	}

	mvwhline(menuw, 0,   0,   ACS_BLOCK, h);
	mvwvline(menuw, 0,   0,   ACS_BLOCK, v);
	mvwhline(menuw, v-1, 0,   ACS_BLOCK, h);
	mvwvline(menuw, 0,   h-1, ACS_BLOCK, v);
}

static void draw_logo(void)
{
	wattrset(menuw, GET_PAIR_FOR(MENU_EDGE_COLOR));
	draw_sprite(menuw, logo_sprite, logo_pos, 40, 8, FALSE);
	wattrset(menuw, GET_PAIR_FOR(MENU_EDGE_COLOR_S)); // TODO add copyright window
	draw_sprite(menuw, logo_ant_sprite, logo_pos, 40, 8, FALSE);
	wattron(menuw, GET_PAIR_FOR(MENU_EDGE_COLOR) | A_REVERSE);
	mvwaddstr(menuw, logo_msg_pos.y, logo_msg_pos.x, logo_msg);
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
			mvwaddch(menuw, pos2.y-1,  pos1.x+o, ACS_DARROW);
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
			mvwvline(menuw, pos1.y-dy-1,  pos1.x+o, ACS_VLINE, dy+1);
			mvwhline(menuw, pos1.y-dy-1,  pos1.x+o, ACS_HLINE, dx);
			mvwvline(menuw, pos1.y-dy-1,  pos2.x+o, ACS_VLINE, dy+1);
			mvwaddch(menuw, pos1.y-dy-1,  pos1.x+o, ACS_ULCORNER);
			mvwaddch(menuw, pos1.y-dy-1,  pos2.x+o, ACS_URCORNER);
			mvwaddch(menuw, pos1.y-1,     pos2.x+o, ACS_DARROW);
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

	/* Draw tile */
	wattrset(menuw, pair = GET_PAIR_FOR(c));
	if (is_def) {
		wattron(menuw, A_REVERSE);
	}
	draw_square(menuw, top_left, s);

	/* Draw direction arrow */
	if (!is_def) {
		wattron(menuw, A_REVERSE);
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
	Vector2i pos1, pos2, cdef_pos;

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

	/* Draw default color picker message */
	cdef_pos = get_menu_cdef_pos();
	wattrset(menuw, GET_PAIR_FOR(MENU_ACTIVE_COLOR));
	mvwaddstr(menuw, cdef_pos.y, cdef_pos.x, dialog_cdef_msg);
}

static void draw_init_size(void)
{
	if (stgs.init_size < GRID_MIN_INIT_SIZE || stgs.init_size > GRID_MAX_INIT_SIZE) {
		return;
	}
	wattrset(menuw, GET_PAIR_FOR(MENU_ACTIVE_COLOR));
	draw_sprite(menuw, uarrow_sprite, menu_isz_u_pos, MENU_UDARROW_WIDTH, MENU_UDARROW_HEIGHT, FALSE);
	draw_sprite(menuw, darrow_sprite, menu_isz_d_pos, MENU_UDARROW_WIDTH, MENU_UDARROW_HEIGHT, FALSE);
	wattrset(menuw, fg_pair);
	draw_sprite(menuw, digit_sprites[stgs.init_size], isz_pos, 3, 5, TRUE);
}

static void draw_direction(void)
{
	wattrset(menuw, GET_PAIR_FOR(MENU_EDGE_COLOR));
	mvwaddstr(menuw, dir_msg_pos.y, dir_msg_pos.x, dir_msg);

	wattrset(menuw, fg_pair);
	mvwaddstr(menuw, menu_dir_u_pos.y+3, menu_dir_u_pos.x, "Ant");
	//draw_rect(menuw, (Vector2i) { menu_dir_u_pos.y+3, menu_dir_u_pos.x }, MENU_UDARROW_WIDTH, 1);

	//wattrset(menuw, GET_PAIR_FOR(!has_simulation_started(stgs.linked_sim)
	//							 ? MENU_ACTIVE_COLOR : MENU_INACTIVE_COLOR));
	wattrset(menuw, GET_PAIR_FOR(MENU_ACTIVE_COLOR));
	draw_sprite(menuw, uarrow_sprite, menu_dir_u_pos, MENU_UDARROW_WIDTH, MENU_UDARROW_HEIGHT, FALSE);
	draw_sprite(menuw, larrow_sprite, menu_dir_r_pos, MENU_RLARROW_WIDTH, MENU_RLARROW_HEIGHT, FALSE);
	draw_sprite(menuw, darrow_sprite, menu_dir_d_pos, MENU_UDARROW_WIDTH, MENU_UDARROW_HEIGHT, FALSE);
	draw_sprite(menuw, rarrow_sprite, menu_dir_l_pos, MENU_RLARROW_WIDTH, MENU_RLARROW_HEIGHT, FALSE);
}

static void draw_control_buttons(void)
{
	short bg = AVAILABLE_COLOR(GET_COLOR_FOR(bg_pair), COLOR_SILVER, COLOR_WHITE);
	Vector2i o = { (MENU_BUTTON_HEIGHT-5)/2, (MENU_BUTTON_WIDTH-5)/2 };
	Vector2i pos1 = { menu_play_pos.y  + o.y, menu_play_pos.x  + o.x };
	Vector2i pos2 = { menu_pause_pos.y + o.y, menu_pause_pos.x + o.x };
	Vector2i pos3 = { menu_stop_pos.y  + o.y, menu_stop_pos.x  + o.x };

	wattrset(menuw, GET_PAIR_FOR(bg));
	draw_rect(menuw, menu_play_pos,  MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT);
	draw_rect(menuw, menu_pause_pos, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT);
	draw_rect(menuw, menu_stop_pos,  MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT);

	wattrset(menuw, GET_PAIR_FOR(has_enough_colors(stgs.colors)
								 ? MENU_PLAY_COLOR : MENU_INACTIVE_COLOR));
	draw_sprite(menuw, button_sprites[0], pos1, 5, 5, FALSE);

	wattrset(menuw, GET_PAIR_FOR(is_simulation_running(stgs.linked_sim)
								 ? MENU_PAUSE_COLOR : MENU_INACTIVE_COLOR));
	draw_sprite(menuw, button_sprites[1], pos2, 5, 5, FALSE);
	
	if (has_simulation_started(stgs.linked_sim)) {
		wattrset(menuw, GET_PAIR_FOR(MENU_STOP_COLOR));
		draw_sprite(menuw, button_sprites[2], pos3, 5, 5, FALSE);
	} else {
		wattrset(menuw, GET_PAIR_FOR(!is_colors_empty(stgs.colors)
									 ? MENU_CLEAR_COLOR : MENU_INACTIVE_COLOR));
		draw_sprite(menuw, button_sprites[3], pos3, 5, 5, FALSE);
	}
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
	mvwaddstr(menuw, inner1.y+2, inner1.x+2, "RULES");
	mvwaddstr(menuw, inner1.y+3, inner1.x+4, "FILE");
	mvwaddstr(menuw, inner2.y+1, inner2.x+1, "SAVE");
	mvwaddstr(menuw, inner2.y+2, inner2.x+2, "RULES");
	mvwaddstr(menuw, inner2.y+3, inner2.x+4, "FILE");

	/* Draw status indicators */
	if (load_status != STATUS_NONE) {
		wattrset(menuw, GET_PAIR_FOR((load_status == STATUS_SUCCESS) ? COLOR_LIME : COLOR_RED));
		mvwvline(menuw, menu_load_pos.y, menu_load_pos.x+MENU_BUTTON_WIDTH, ACS_BLOCK, MENU_BUTTON_HEIGHT);
	}
	if (save_status != STATUS_NONE) {
		wattrset(menuw, GET_PAIR_FOR((save_status == STATUS_SUCCESS) ? COLOR_LIME : COLOR_RED));
		mvwvline(menuw, menu_save_pos.y, menu_save_pos.x+MENU_BUTTON_WIDTH, ACS_BLOCK, MENU_BUTTON_HEIGHT);
	}
}

static void draw_size(void)
{
	Simulation *sim = stgs.linked_sim;
	size_t size = sim ? sim->grid->size : 0;
	char size_str[29];
	sprintf(size_str, "%28d", size);
	wattrset(menuw, GET_PAIR_FOR(MENU_EDGE_COLOR));
	mvwaddstr(menuw, size_pos.y, size_pos.x-28, size_str);
}

static void draw_steps(void)
{
	static bool do_draw;
	Simulation *sim = stgs.linked_sim;
	size_t steps = sim ? sim->steps : 0;
	int digit, len = (int)log10(steps) + 1;
	char digits_str[9], *p;
	Vector2i tl = steps_pos;

	if (steps <= 1) {
		do_draw = TRUE;
	} else if (!do_draw) {
		return;
	}

	wattrset(menuw, fg_pair);
	if (len > 8) {
		draw_sprite(menuw, inf_sprite, tl, 31, 5, TRUE);
		do_draw = FALSE;
		return;
	}

	sprintf(digits_str, "%8d", steps);
	for (p = digits_str; p < digits_str+8; ++p) {
		if (*p != ' ') {
			digit = *p - '0';
			wattroff(menuw, A_REVERSE);
			draw_sprite(menuw, digit_sprites[digit], tl, 3, 5, TRUE);
		} else {
			wattron(menuw, A_REVERSE);
			draw_rect(menuw, tl, 3, 5);
		}
		tl.x += 4;
	}
}

void draw_menu_full(void)
{
	draw_edge();
	draw_logo();
	draw_color_list();
	draw_init_size();
	draw_direction();
	draw_control_buttons();
	draw_io_buttons();
	draw_size();
	draw_steps();

	/* Draw messages */
	wattrset(menuw, GET_PAIR_FOR(MENU_EDGE_COLOR));
	mvwaddstr(menuw, isz_msg_pos.y,   isz_msg_pos.x,   isz_msg);
	mvwaddstr(menuw, tiles_msg_pos.y, tiles_msg_pos.x, tiles_msg);
	mvwaddstr(menuw, size_msg_pos.y,  size_msg_pos.x,  size_msg);
	mvwaddstr(menuw, steps_msg_pos.y, steps_msg_pos.x, steps_msg);

	wnoutrefresh(menuw);

	if (dialogw) {
		draw_dialog();
	}
}

void draw_menu_iter(void)
{
	Simulation *sim = stgs.linked_sim;
	static bool sparse = FALSE;

#if OPT_STEPS
	static size_t prev_steps;
	if (sim->steps <= 1 || (double)prev_steps/sim->steps <= OPT_STEPS_THRESHOLD) {
#endif
		draw_steps();
#if OPT_STEPS
		prev_steps = sim->steps;
	}
#endif
	
	if (!sparse && is_grid_sparse(sim->grid)) {
		draw_edge();
		sparse = TRUE;
	}

	wnoutrefresh(menuw);
}
