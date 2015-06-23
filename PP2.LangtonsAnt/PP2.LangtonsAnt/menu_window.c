#include <math.h>
#include <string.h>
#include "graphics.h"

WINDOW *menuw;
Settings stgs;
const Vector2i menu_origin = { 0, GRID_WINDOW_SIZE };

const Vector2i steps_origin = { GRID_WINDOW_SIZE-8, MENU_WINDOW_SIZE-1 };
const Vector2i steps_msg_origin = { GRID_WINDOW_SIZE-4, 2 };
const Vector2i tiles_origin = { 20, 2*MENU_TILE_HSEP+MENU_TILE_SIZE+1 };

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
	menuw = newwin(GRID_WINDOW_SIZE, MENU_WINDOW_SIZE, menu_origin.y, menu_origin.x);
	wbkgd(menuw, bg_pair);
	keypad(gridw, TRUE);
	nodelay(gridw, TRUE);
}

void end_menu_window(void)
{
	delwin(gridw);
	gridw = NULL;
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

	pos.y = tiles_origin.y + index_y*(MENU_TILE_SIZE+MENU_TILE_VSEP);
	pos.x = tiles_origin.x - index_x*(MENU_TILE_SIZE+MENU_TILE_HSEP);

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

	/* Draw tile */
	wattrset(menuw, pair = get_pair_for(c));
	if (is_def) {
		wattron(menuw, A_REVERSE);
	}
	draw_box(menuw, top_left, s);

	/* Draw frame */
	wattrset(menuw, is_def ? pair : fg_pair);
	mvwhline(menuw, y, x, ACS_BLOCK, s);
	mvwvline(menuw, y, x, ACS_BLOCK, s);
	mvwhline(menuw, y+s-1, x, ACS_BLOCK, s);
	mvwvline(menuw, y, x+s-1, ACS_BLOCK, s);
}

static void draw_color_list(void)
{
	short c, i = 0;
	Vector2i pos1, pos2;

	if (!stgs.colors) {
		return;
	}

	/* Draw color tiles */
	c = stgs.colors->first;
	do {
		pos1 = pos2 = get_menu_tile_pos(i++);
		if (c == COLOR_EMPTY) {
			break;
		}
		pos2 = get_menu_tile_pos(i);
		draw_color_arrow(pos1, pos2);
		draw_color_tile(pos1, c);
		c = stgs.colors->next[c];
	} while (c != stgs.colors->last);

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

static void draw_steps(void)
{
	char digits[9], *p;
	size_t digit;
	Vector2i top_left = steps_origin;

	if (stgs.steps == 10000000) {
		top_left.x -= 33;
		draw_bitmap(menuw, top_left, inf_bitmap, 32, 5);
		goto exit;
	} else if (stgs.steps > 10000000) {
		return;
	}

	sprintf(digits, "%8d", stgs.steps);
	wattrset(menuw, fg_pair);
	for (p = digits+7; p > 0 && *p != ' '; --p) {
		top_left.x -= 4;
		digit = *p - '0';
		draw_bitmap(menuw, top_left, digit_bitmaps[digit], 3, 5);
	}

exit:
	wrefresh(menuw);
}

void draw_menu(void)
{
	size_t h = MENU_WINDOW_SIZE, v = GRID_WINDOW_SIZE;

	wattrset(menuw, get_pair_for(MENU_BORDER_COLOR));
	mvwhline(menuw, 0, 0, ACS_BLOCK, h);
	mvwvline(menuw, 0, 0, ACS_BLOCK, v);
	mvwhline(menuw, v-1, 0, ACS_BLOCK, h);
	mvwvline(menuw, 0, h-1, ACS_BLOCK, v);

	mvwaddstr(menuw, v-4, 2, "STEPS:");

	wattron(menuw, A_REVERSE);
	//draw_bitmap(menuw, (Vector2i) { 1, 1 }, logo_bitmap, 40, 9);

	draw_color_list();

	wrefresh(menuw);
}
