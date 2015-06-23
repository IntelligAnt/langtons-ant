#include "graphics.h"

WINDOW *dialogw;
Vector2i dialog_origin = { -1, -1 };

void open_dialog(Vector2i pos)
{
	short i, fg = get_color_for(fg_pair), tile_border = AVAILABLE_FG_COLOR(stgs.colors->def, COLOR_WHITE), x, y;
	Vector2i inner;

	pos = rel2abs(pos, menu_origin);
	if (pos.x + DIALOG_WINDOW_WIDTH >= MENU_WINDOW_SIZE){
		pos.x += DIALOG_WINDOW_WIDTH - MENU_WINDOW_SIZE - 2;
	}
	dialog_origin = pos;

	dialogw = newwin(DIALOG_WINDOW_HEIGHT, DIALOG_WINDOW_WIDTH, pos.y, pos.x);
	wbkgd(dialogw, get_pair_for(stgs.colors->def));
	nodelay(dialogw, TRUE);
	++pos.y, ++pos.x;

	for (i = 0; i < COLOR_COUNT; i++){
		if (i != fg) {
			if (color_exists(stgs.colors, i) || i == stgs.colors->def) {
				inner.y = pos.y + 1, inner.x = pos.x + 1;
				wattrset(dialogw, tile_border);
				draw_box(dialogw, pos, DIALOG_TILE_SIZE);
				wattrset(dialogw, i);
				draw_box(dialogw, inner, DIALOG_TILE_SIZE - 2);
				mvwaddch(dialogw, inner.y, inner.x, 'X' | A_REVERSE);
			}
			else {
				wattrset(dialogw, i);
				draw_box(dialogw, pos, 3);
			}
			if (pos.x + DIALOG_TILE_SIZE + 1 < dialog_origin.x + DIALOG_WINDOW_WIDTH) {
				pos.x += DIALOG_TILE_SIZE;
			}
			else {
				pos.x = dialog_origin.x + 1;
				pos.y = dialog_origin.y + DIALOG_TILE_SIZE + 1;
			}
		}
	}
	pos.y = dialog_origin.y + DIALOG_WINDOW_HEIGHT - 3;
	pos.x = dialog_origin.x + 1;
	y = DIALOG_WINDOW_HEIGHT - dialog_origin.y + pos.y - 1;
	x = (DIALOG_WINDOW_WIDTH - 3) / 2;
	for (i = 0; i < 2; i++) {
		inner.y = pos.y + y/2 - i, inner.x = pos.x + x/2;
		wattrset(dialogw, tile_border);
		draw_rect(dialogw, pos, x, y);
		if (tile_border == COLOR_WHITE) {
			wattron(dialogw, A_REVERSE);
		}
		else {
			wattrset(dialogw, get_pair_for(fg));
		}
		mvwaddch(dialogw, inner.y, inner.x, i ? '>' : '<');
		pos.x = dialog_origin.x + DIALOG_WINDOW_WIDTH - x - 1;
	}
}

void close_dialog(void)
{
	delwin(dialogw);
	dialogw = NULL;
}

Vector2i get_dialog_tile_pos(int tile)
{
	int i=0, fg = get_color_for(fg_pair);
	Vector2i pos = dialog_origin;
	++pos.y, ++pos.x;

	if (tile == fg) {
		return (Vector2i){ -1, -1 };
	}
	while (i<tile) {
		if (i != fg) {
			if (pos.x + DIALOG_TILE_SIZE + 1 < dialog_origin.x + DIALOG_WINDOW_WIDTH) {
				pos.x += DIALOG_TILE_SIZE;
			}
			else {
				pos.x = dialog_origin.x + 1;
				pos.y = dialog_origin.y + DIALOG_TILE_SIZE + 1;
			}
		}
		++i;
	}
	return pos;
}