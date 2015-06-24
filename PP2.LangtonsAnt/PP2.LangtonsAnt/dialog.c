#include "graphics.h"

WINDOW *dialogw;
Vector2i dialog_pos = { -1, -1 };

static int cidx;
static short picked_color = COLOR_EMPTY, picked_turn = 0;

static void draw_tiles(Vector2i top_left)
{
	short i, fg = GET_COLOR_FOR(fg_pair);
	chtype border_pair = GET_PAIR_FOR(stgs.colors->def);
	Vector2i outer = top_left, inner;

	for (i = 0; i < COLOR_COUNT; i++) {
		if (i == fg || cidx != CIDX_DEFAULT && i == stgs.colors->def) {
			continue;
		}
		if (color_exists(stgs.colors, i)) {
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
	picked_color = COLOR_EMPTY;
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
	int i, fg = GET_COLOR_FOR(fg_pair);
	Vector2i pos = { 1, 1 };

	if (index == fg || cidx != CIDX_DEFAULT && index == stgs.colors->def) {
		return (Vector2i) { -1, -1 };
	}

	for (i = 0; i < index; ++i) {
		if (i == fg || cidx != CIDX_DEFAULT && i == stgs.colors->def) {
			continue;
		}
		if (pos.x + DIALOG_TILE_SIZE + 1 < DIALOG_WINDOW_WIDTH) {
			pos.x += DIALOG_TILE_SIZE;
		} else {
			pos.x = 1;
			pos.y += DIALOG_TILE_SIZE;
		}
	}

	return pos;
}

Vector2i get_dialog_button_pos(int index)
{
	return (Vector2i) {
		.y = 2 + DIALOG_ROWS*DIALOG_TILE_SIZE,
		.x = (index == -1) ? 1 : DIALOG_BUTTON_WIDTH + 2
	};
}

Vector2i get_del_button_pos(void)
{
	return (Vector2i) { 3 + DIALOG_ROWS*DIALOG_TILE_SIZE + DIALOG_BUTTON_HEIGHT, 1 };
}

void dialog_mouse_command(MEVENT event)
{
	int i;
	bool del = FALSE;
	Vector2i pos = abs2rel((Vector2i) { event.y, event.x }, dialog_pos), top_left;

	if (!(event.bstate & BUTTON1_CLICKED)) {
		return;
	}

	top_left = get_del_button_pos();
	if (area_contains(top_left, DIALOG_WINDOW_WIDTH - 2, DIALOG_BUTTON_HEIGHT, pos)) {
		del = TRUE;
		goto exit;
	}
	for (i = 0; i < COLOR_COUNT; i++) {
		top_left = get_dialog_tile_pos(i);
		if (!color_exists(stgs.colors, i) &&
			area_contains(top_left, DIALOG_TILE_SIZE, DIALOG_TILE_SIZE, pos)) {
			picked_color = i;
			goto exit;
		}
	}
	for (i = -1; i <= 1; i += 2) {
		top_left = get_dialog_button_pos(i);
		if (area_contains(top_left, DIALOG_BUTTON_WIDTH, DIALOG_BUTTON_HEIGHT, pos)) {
			picked_turn = i;
			goto exit;
		}
	}

exit:
	switch (cidx) {
	case CIDX_NEWCOLOR:
		if (picked_color != COLOR_EMPTY && picked_turn != 0) {
			add_color(stgs.colors, picked_color, picked_turn);
			close_dialog();
		}
		break;
	case CIDX_DEFAULT:
		if (picked_color != COLOR_EMPTY) {
			colors_delete(stgs.colors);
			stgs.colors = colors_new(picked_color);
			close_dialog();
		}
		break;
	default:
		if (cidx >= 0 && cidx < COLOR_COUNT) {
			if (picked_turn != 0) {
				if (picked_color != COLOR_EMPTY) {
					set_color(stgs.colors, cidx, picked_color, picked_turn);
				} else {
					set_turn(stgs.colors, cidx, picked_turn);
				}
				close_dialog();
			} else if (del) {
				remove_color(stgs.colors, get_color_at(stgs.colors, cidx));
				close_dialog();
			}
		}
	}
}
