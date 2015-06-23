#include "graphics.h"

int menu_key_command(int key)
{
	Vector2i rel;

	switch (key) {
	case ' ':
		//is_running() ? stop_simulation() : run_simulation();
		break;

	case KEY_ESC:
		stop_simulation();
		break;

	case KEY_MOUSE:
		menu_mouse_command();
		break;

	default:
		return ERR;
	}
	return key;
}

void isz_button_clicked(int i)
{
	if (!is_running()) {
		switch (i) {
		case 1:
			if (stgs.init_size < GRID_MAX_INIT_SIZE) {
				++stgs.init_size;
			}
			break;
		case -1:
			if (stgs.init_size > GRID_MIN_INIT_SIZE) {
				--stgs.init_size;
			}
			break;
		}
		
	}
}

static bool is_tile_clicked(Vector2i pos, Vector2i click)
{
	return (click.y >= pos.y && click.y < pos.y+MENU_TILE_SIZE
		 && click.x >= pos.x && click.x < pos.x+MENU_TILE_SIZE);
}

void menu_mouse_command(void)
{
	MEVENT event;
	Vector2i pos;
	int dy, dx, i;

	nc_getmouse(&event);
	if (event.bstate & BUTTON1_CLICKED) {
		pos = abs2rel((Vector2i) { event.y, event.x }, menu_pos);
		if (dialogw) {
			close_dialog();
		}

		/* Init size buttons clicked */
		dy = pos.y-menu_isz_u_pos.y, dx = pos.x-menu_isz_u_pos.x;
		if (dx == 0 && (dy == 0 || dy == 1)) {
			isz_button_clicked(1);
			return;
		}
		dy = pos.y-menu_isz_d_pos.y, dx = pos.x-menu_isz_d_pos.x;
		if (dx == 0 && (dy == 0 || dy == 1)) {
			isz_button_clicked(-1);
			return;
		}

		/* Control buttons clicked */

		/* Color tiles clicked */
		for (i = 0; i < MENU_TILE_COUNT; ++i) {
			if (is_tile_clicked(get_menu_tile_pos(i), pos)) {
				open_dialog(pos, i);
				break;
			}
		}
	}
}
