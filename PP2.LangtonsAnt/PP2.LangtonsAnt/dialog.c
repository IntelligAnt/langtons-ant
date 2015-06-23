#include "graphics.h"

WINDOW *dialogw;
Vector2i dialog_pos = { -1, -1 };

static int cidx;
static short picked_color = -1, picked_turn = 0;

static void draw_tiles(Vector2i top_left)
{
	short i, fg = GET_COLOR_FOR(fg_pair);
	chtype border_pair = GET_PAIR_FOR(stgs.colors->def);
	Vector2i outer = top_left, inner;

	for (i = 0; i < COLOR_COUNT; i++) {
		if (cidx != CIDX_DEFAULT && (i == fg || i == stgs.colors->def)) {
			continue;
		}
		if (color_exists(stgs.colors, i) || i == stgs.colors->def) {
			inner.y = outer.y + 1, inner.x = outer.x + 1;
			wattrset(dialogw, border_pair);
			draw_box(dialogw, outer, DIALOG_TILE_SIZE);
			wattrset(dialogw, GET_PAIR_FOR(i));
			draw_box(dialogw, inner, DIALOG_TILE_SIZE - 2);
		} else {
			wattrset(dialogw, GET_PAIR_FOR(i));
			draw_box(dialogw, outer, DIALOG_TILE_SIZE);
		}
		if (outer.x + DIALOG_TILE_SIZE + 1 >= DIALOG_WINDOW_WIDTH) {
			outer.y += DIALOG_TILE_SIZE, outer.x = top_left.x;
		} else {
			outer.x += DIALOG_TILE_SIZE;
		}
	}
}

static void draw_buttons(Vector2i top_left)
{
	Vector2i top_left2 = { top_left.y, top_left.x+DIALOG_BUTTON_WIDTH+1 };
	int i, mid = DIALOG_BUTTON_HEIGHT/2, n = DIALOG_BUTTON_WIDTH-2;

	wattrset(dialogw, GET_PAIR_FOR(DIALOG_BUTTON_COLOR));
	draw_rect(dialogw, top_left,  DIALOG_BUTTON_WIDTH, DIALOG_BUTTON_HEIGHT);
	draw_rect(dialogw, top_left2, DIALOG_BUTTON_WIDTH, DIALOG_BUTTON_HEIGHT);

	wattron(dialogw, A_REVERSE);
	mvwaddstr(dialogw, top_left.y+mid,   top_left.x+1, "  <  ");
	mvwaddstr(dialogw, top_left2.y+mid, top_left2.x+1, "  >  ");
}

void open_dialog(Vector2i pos, int color_index)
{
	cidx = color_index;

	if (pos.x + DIALOG_WINDOW_WIDTH >= MENU_WINDOW_WIDTH) {
		pos.x += DIALOG_WINDOW_WIDTH - MENU_WINDOW_WIDTH - 2;
	}
	dialog_pos = rel2abs(pos, menu_pos);

	dialogw = newwin(DIALOG_WINDOW_HEIGHT, DIALOG_WINDOW_WIDTH, dialog_pos.y, dialog_pos.x);
	keypad(dialogw, TRUE);
	nodelay(dialogw, TRUE);
}

void close_dialog(void)
{
	delwin(dialogw);
	dialogw = NULL;
	picked_color = -1;
	picked_turn = 0;
}

void draw_dialog(void)
{
	wattrset(dialogw, GET_PAIR_FOR(stgs.colors->def));
	draw_rect(dialogw, (Vector2i) { 0, 0 }, DIALOG_WINDOW_WIDTH, DIALOG_WINDOW_HEIGHT);

	draw_tiles((Vector2i) { 1, 1 });
	draw_buttons((Vector2i) { 2+DIALOG_ROWS*DIALOG_TILE_SIZE, 1 });

	wnoutrefresh(dialogw);
}

Vector2i get_dialog_tile_pos(int index)
{
	int i=0, fg = GET_COLOR_FOR(fg_pair);
	Vector2i pos = dialog_pos;
	++pos.y, ++pos.x;

	if (index == fg) {
		return (Vector2i){ -1, -1 };
	}
	while (i < index) {
		if (i != fg) {
			if (pos.x + DIALOG_TILE_SIZE + 1 < dialog_pos.x + DIALOG_WINDOW_WIDTH) {
				pos.x += DIALOG_TILE_SIZE;
			}
			else {
				pos.x = dialog_pos.x + 1;
				pos.y = dialog_pos.y + DIALOG_TILE_SIZE + 1;
			}
		}
		++i;
	}
	return pos;
}

Vector2i get_dialog_button_pos(int button)
{
	switch (button){
	case -1: break;
	case 1: break;
	}
}

void dialog_mouse_command(int key)
{
	if (key!=KEY_MOUSE){
		return;
	}

	MEVENT event;
	int i;
	Vector2i tile_pos;

	nc_getmouse(&event);
	if (!(event.bstate & BUTTON1_CLICKED)) {
		return;
	}

	for (i = 0; i < COLOR_COUNT; i++) {
		tile_pos = get_dialog_tile_pos(i);
		if (event.y >= tile_pos.y && event.y < tile_pos.y + DIALOG_TILE_SIZE &&
			event.x >= tile_pos.x && event.x < tile_pos.x + DIALOG_TILE_SIZE){
			picked_color = i;
			goto exit;
		}
	}



exit: if (picked_color != -1 && picked_turn != 0) {
	add_color(stgs.colors, picked_color, picked_turn);
	close_dialog();
}
}