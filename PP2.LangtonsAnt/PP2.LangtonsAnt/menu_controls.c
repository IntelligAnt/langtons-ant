#include "graphics.h"
#include "io.h"

static void isz_button_clicked(int i)
{
	Simulation *sim = stgs.linked_sim;
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
	default:
		return;
	}
	if (sim && !sim->is_running && sim->steps == 0) {
		simulation_delete(sim);
		stgs.linked_sim = simulation_new(stgs.colors, stgs.init_size);
	}
}

static void play_button_clicked(void)
{
	Simulation *sim = stgs.linked_sim;
	if (!sim || sim->is_running) {
		if (sim) {
			simulation_delete(sim);
		}
		sim = stgs.linked_sim = simulation_new(stgs.colors, stgs.init_size);
	}
	if (has_enough_colors(sim->colors)) {
		run_simulation(sim);
	}
}

static void pause_button_clicked(void)
{
	Simulation *sim = stgs.linked_sim;
	if (sim && sim->is_running) {
		stop_simulation(sim);
	}
}

static void clear_button_clicked(void)
{
	Simulation *sim = stgs.linked_sim;
	if (sim) {
		if (sim->is_running) {
			stop_simulation(sim);
		}
		simulation_delete(sim);
		remove_all_colors(stgs.colors);
		stgs.linked_sim = simulation_new(stgs.colors, stgs.init_size);
		gridscrl = (ScrollInfo) { 0 };
	}
}

int menu_key_command(int key)
{
	Simulation *sim = stgs.linked_sim;
	Vector2i rel;

	switch (key) {
	case ' ':
		sim->is_running ? pause_button_clicked() : play_button_clicked();
		break;

	case KEY_BACKSPACE: case '\b':
		clear_button_clicked();
		break;

	case KEY_ESC:
		exit_draw_loop(TRUE);
		break;

	case KEY_MOUSE:
		menu_mouse_command();
		break;

	default:
		return ERR;
	}
	return key;
}

void menu_mouse_command(void)
{
	MEVENT event;
	Vector2i event_pos, pos, tile;
	int dy, dx, i;

	nc_getmouse(&event);
	if (event.bstate & BUTTON1_CLICKED) {
		event_pos.y = event.y, event_pos.x = event.x;
		pos = abs2rel(event_pos, menu_pos);

		if (dialogw) {
			if (area_contains(dialog_pos, DIALOG_WINDOW_WIDTH, DIALOG_WINDOW_HEIGHT,
				              event_pos)) {
				dialog_mouse_command(event);
				return;
			}
			close_dialog();
		}

		/* Init size buttons clicked */
		if (area_contains(menu_isz_u_pos, 1, 2, pos)) {
			isz_button_clicked(1);
			return;
		}
		if (area_contains(menu_isz_d_pos, 1, 2, pos)) {
			isz_button_clicked(-1);
			return;
		}

		/* Control buttons clicked */
		if (area_contains(menu_play_pos, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT, pos)) {
			play_button_clicked();
			return;
		}
		if (area_contains(menu_pause_pos, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT, pos)) {
			pause_button_clicked();
			return;
		}
		if (area_contains(menu_clear_pos, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT, pos)) {
			clear_button_clicked();
			return;
		}

		/* IO buttons clicked */


		/* Color tiles clicked */
		for (i = 0; i < MENU_TILE_COUNT; ++i) {
			tile = get_menu_tile_pos(i);
			if (i <= stgs.colors->n &&
					area_contains(tile, MENU_TILE_SIZE, MENU_TILE_SIZE, pos)) {
				open_dialog(pos, (i == stgs.colors->n) ? CIDX_NEWCOLOR : i);
				break;
			}
		}
	}
}
