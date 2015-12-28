#include "graphics.h"

WINDOW *dialogw;
Vector2i dialog_pos = { -1, -1 }; // TODO can do better

int cidx;
short picked_color = COLOR_NONE, picked_turn = TURN_NONE;

const Vector2i colors_pos  = { 1, 1 };
const Vector2i left_pos    = { DIALOG_TILE_ROWS*DIALOG_TILE_SIZE+2, 1 };
const Vector2i right_pos   = { DIALOG_TILE_ROWS*DIALOG_TILE_SIZE+2, DIALOG_BUTTON_WIDTH+2 };
const Vector2i delete_pos  = { DIALOG_TILE_ROWS*DIALOG_TILE_SIZE+DIALOG_BUTTON_HEIGHT+3,
                              (DIALOG_WINDOW_HEIGHT-DIALOG_BUTTON_WIDTH-2)/2+1 };

void open_dialog(Vector2i pos, int color_index)
{
	size_t height = (color_index < 0) ? delete_pos.y : DIALOG_WINDOW_HEIGHT;
	cidx = color_index;

	if (pos.x + DIALOG_WINDOW_WIDTH >= MENU_WINDOW_WIDTH) {
		pos.x += DIALOG_WINDOW_WIDTH - MENU_WINDOW_WIDTH - 2;
	}
	dialog_pos = rel2abs(pos, menu_pos);

	dialogw = newwin(height, DIALOG_WINDOW_WIDTH, dialog_pos.y, dialog_pos.x);
	keypad(dialogw, TRUE);
	nodelay(dialogw, TRUE);
}

void close_dialog(void)
{
	delwin(dialogw);
	dialogw = NULL;
	picked_color = COLOR_NONE;
	picked_turn = TURN_NONE;
}

static void draw_tiles(void)
{
	short i, fg = GET_COLOR_FOR(fg_pair);
	Vector2i outer = colors_pos, inner;

	for (i = 0; i < COLOR_COUNT; i++) {
		if (i == fg || cidx != CIDX_DEFAULT && i == stgs.colors->def) {
			continue;
		}
		if (color_exists(stgs.colors, i)) {
			inner.y = outer.y + 1, inner.x = outer.x + 1;
			wattrset(dialogw, GET_PAIR_FOR(stgs.colors->def));
			draw_box(dialogw, outer, DIALOG_TILE_SIZE);
			wattrset(dialogw, GET_PAIR_FOR(i));
			draw_box(dialogw, inner, DIALOG_TILE_SIZE - 2);
		} else {
			wattrset(dialogw, GET_PAIR_FOR(i));
			draw_box(dialogw, outer, DIALOG_TILE_SIZE);
		}
		if (outer.x + DIALOG_TILE_SIZE + 1 >= DIALOG_WINDOW_WIDTH) {
			outer.y += DIALOG_TILE_SIZE, outer.x = colors_pos.x;
		} else {
			outer.x += DIALOG_TILE_SIZE;
		}
	}
}

static void draw_buttons() // TODO draw line borders around selected buttons
{
	int mid = DIALOG_BUTTON_HEIGHT/2;

	wattrset(dialogw, GET_PAIR_FOR(DIALOG_BUTTON_COLOR));
	draw_rect(dialogw, left_pos,  DIALOG_BUTTON_WIDTH, DIALOG_BUTTON_HEIGHT);
	draw_rect(dialogw, right_pos, DIALOG_BUTTON_WIDTH, DIALOG_BUTTON_HEIGHT);
	wattron(dialogw, A_REVERSE);
	mvwaddstr(dialogw, left_pos.y+mid,  left_pos.x+3,  "<");
	mvwaddstr(dialogw, right_pos.y+mid, right_pos.x+3, ">");

	if (cidx >= 0) {
		wattrset(dialogw, GET_PAIR_FOR(DIALOG_DELETE_COLOR));
		draw_rect(dialogw, delete_pos, DIALOG_BUTTON_WIDTH-2, DIALOG_BUTTON_HEIGHT);
		wattron(dialogw, A_REVERSE);
		mvwaddstr(dialogw, delete_pos.y+mid, delete_pos.x+2, "X");
	}
}

void draw_dialog(void)
{
	wattrset(dialogw, GET_PAIR_FOR(stgs.colors->def));
	draw_rect(dialogw, (Vector2i) { 0, 0 }, DIALOG_WINDOW_WIDTH, DIALOG_WINDOW_HEIGHT);

	draw_tiles();
	draw_buttons();

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
	switch (index) {
	case -1:
		return (Vector2i) { 2+DIALOG_TILE_ROWS*DIALOG_TILE_SIZE, 1 };
	case 0:
		return (Vector2i) { 3+DIALOG_TILE_ROWS*DIALOG_TILE_SIZE+DIALOG_BUTTON_HEIGHT, 1 };
	case 1:
		return (Vector2i) { 2+DIALOG_TILE_ROWS*DIALOG_TILE_SIZE, DIALOG_BUTTON_WIDTH+2 };
	default:
		return (Vector2i) { -1, -1 };
	}
}

input_t dialog_mouse_command(MEVENT event)
{
	int i;
	bool del = FALSE;
	Vector2i pos = abs2rel((Vector2i) { event.y, event.x }, dialog_pos), top_left;

	if (!(event.bstate & BUTTON1_CLICKED)) {
		return INPUT_NO_CHANGE;
	}

	top_left = get_dialog_button_pos(0);
	if (cidx >= 0 && area_contains(top_left, DIALOG_WINDOW_WIDTH - 2, DIALOG_BUTTON_HEIGHT, pos)) {
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
		if (picked_color != COLOR_NONE && picked_turn != TURN_NONE) {
			add_color(stgs.colors, picked_color, picked_turn);
			close_dialog();
			return INPUT_MENU_CHANGED;
		}
		break;
	case CIDX_DEFAULT:
		if (picked_color != COLOR_NONE) {
			colors_delete(stgs.colors);
			stgs.colors = colors_new(picked_color);
			close_dialog();
			return INPUT_MENU_CHANGED;
		}
		break;
	default:
		if (cidx >= 0 && cidx < COLOR_COUNT) {
			if (picked_turn != TURN_NONE) {
				if (picked_color != COLOR_NONE) {
					set_color(stgs.colors, cidx, picked_color, picked_turn);
				} else {
					set_turn(stgs.colors, cidx, picked_turn);
				}
				close_dialog();
				return INPUT_MENU_CHANGED;
			} else if (del) {
				remove_color(stgs.colors, get_color_at(stgs.colors, cidx));
				if (!has_enough_colors(stgs.colors)) {
					halt_simulation(stgs.linked_sim);
				}
				close_dialog();
				return INPUT_MENU_CHANGED;
			}
		}
	}

	return INPUT_NO_CHANGE;
}
