#include <stdlib.h>
#include <string.h>

#include "graphics.h"
#include "io.h"

#define INPUT_WINDOW_WIDTH  (MENU_WINDOW_WIDTH-4)
#define INPUT_WINDOW_HEIGHT 3

static WINDOW *iow;
static const Vector2i io_pos = { MENU_CONTROLS_POS-22,
                                 GRID_WINDOW_SIZE+MENU_WINDOW_WIDTH-INPUT_WINDOW_WIDTH-2 };

input_t set_simulation(Simulation *sim)
{
	assert(stgs.linked_sim);
	simulation_delete(stgs.linked_sim);
	stgs.linked_sim = sim;
	colors_delete(stgs.colors);
	stgs.colors = sim->colors;
	stgs.init_size = sim->grid->init_size;
	reset_scroll();
	return INPUT_MENU_CHANGED | INPUT_GRID_CHANGED;
}

input_t reset_simulation(void)
{
	Simulation *sim = stgs.linked_sim;
	if (sim) {
		simulation_delete(sim);
	}
	stgs.linked_sim = simulation_new(stgs.colors, stgs.init_size);
	reset_scroll();
	return INPUT_MENU_CHANGED | INPUT_GRID_CHANGED;
}

input_t clear_simulation(void)
{
	remove_all_colors(stgs.colors);
	return reset_simulation();
}

static input_t isize_button_clicked(int d)
{
	Simulation *sim = stgs.linked_sim;
	if (d > 0) {
		stgs.init_size = min(stgs.init_size+d, GRID_MAX_INIT_SIZE);
	} else if (d < 0) {
		stgs.init_size = max(stgs.init_size+d, GRID_MIN_INIT_SIZE);
	} else {
		return INPUT_NO_CHANGE;
	}
	if (!is_simulation_running(sim) && !has_simulation_started(sim)) { // Sanity check
		return reset_simulation();
	}
	return INPUT_MENU_CHANGED;
}

static input_t dir_button_clicked(Direction dir)
{
	stgs.linked_sim->ant->dir = dir;
	return INPUT_MENU_CHANGED | INPUT_GRID_CHANGED;
}

static input_t speed_button_clicked(int d)
{
	if (d > 0) {
		stgs.speed = min(stgs.speed+d, LOOP_MAX_SPEED);
	} else if (d < 0) {
		stgs.speed = max(stgs.speed+d, LOOP_MIN_SPEED);
	} else {
		return INPUT_NO_CHANGE;
	}
	return INPUT_MENU_CHANGED;
}

static input_t play_button_clicked(void)
{
	input_t ret = INPUT_NO_CHANGE;
	Simulation *sim = stgs.linked_sim;
	if (is_simulation_running(sim)) {
		ret |= reset_simulation();
		sim = stgs.linked_sim;
	}
	if (sim && has_enough_colors(sim->colors)) {
		simulation_run(sim);
		ret |= INPUT_MENU_CHANGED;
	}
	return ret;
}

static input_t pause_button_clicked(void)
{
	Simulation *sim = stgs.linked_sim;
	if (is_simulation_running(sim)) {
		simulation_halt(sim);
	}
	return INPUT_MENU_CHANGED;
}

static input_t stop_button_clicked(void)
{
	return has_simulation_started(stgs.linked_sim) ? reset_simulation() : clear_simulation();
}

static void read_filename(char *filename)
{
	iow = newwin(3, INPUT_WINDOW_WIDTH, io_pos.y, io_pos.x); // TODO move to window drawing file
	wbkgd(iow, GET_PAIR_FOR(COLOR_GRAY) | A_REVERSE);
	wattron(iow, fg_pair);
	waddstr(iow, " Path: ");
	wattroff(iow, fg_pair);
	echo();
	mvwgetnstr(iow, 1, 1, filename, FILENAME_BUF_LEN);
	noecho();
	delwin(iow);
}

static input_t io_button_clicked(bool load)
{
	Simulation *sim;
	char filename[FILENAME_BUF_LEN];
	read_filename(filename);
	if (load) {
		load_status = (sim = load_state(filename)) ? STATUS_SUCCESS : STATUS_FAILURE;
		if (sim) {
			return set_simulation(sim);
		}
	} else {
		save_status = (save_state(filename, stgs.linked_sim) != EOF) ? STATUS_SUCCESS : STATUS_FAILURE;
	}
	return INPUT_MENU_CHANGED;
}

input_t menu_key_command(int key)
{
	Simulation *sim = stgs.linked_sim;

	switch (key) {
		/* Init size */
	case ']':
		return isize_button_clicked(1);
	case '[':
		return isize_button_clicked(-1);

		/* Speed */
	case '=': case PADPLUS:
		return speed_button_clicked(1);
	case '-': case PADMINUS:
		return speed_button_clicked(-1);

		/* Direction */
	case 'W': case 'w':
		return dir_button_clicked(DIR_UP);
	case 'D': case 'd':
		return dir_button_clicked(DIR_RIGHT);
	case 'S': case 's':
		return dir_button_clicked(DIR_DOWN);
	case 'A': case 'a':
		return dir_button_clicked(DIR_LEFT);

		/* Simulation */
	case ' ':
		return is_simulation_running(sim) ? pause_button_clicked() : play_button_clicked();
	case 'R': case 'r':
		return stop_button_clicked();
	case KEY_BACKSPACE: case '\b':
		return clear_simulation();

		/* IO */
	case KEY_F(1):
		return io_button_clicked(TRUE);
	case KEY_F(2):
		return io_button_clicked(FALSE);

		/* Quit */
	case KEY_ESC:
		stop_game_loop();
		return INPUT_NO_CHANGE;

	case KEY_MOUSE:
		return menu_mouse_command();

	default:
		return INPUT_NO_CHANGE;
	}
}

input_t menu_mouse_command(void)
{
	input_t ret = INPUT_NO_CHANGE;
	MEVENT event;
	Vector2i event_pos, pos, tile;
	size_t i;

	nc_getmouse(&event);
	event_pos.y = event.y, event_pos.x = event.x;
	pos = abs2rel(event_pos, menu_pos);

	if (dialogw) {
		if (area_contains(dialog_pos, DIALOG_WINDOW_WIDTH, DIALOG_WINDOW_HEIGHT, event_pos)) {
			return dialog_mouse_command(event);
		} else {
			close_dialog();
			ret = INPUT_MENU_CHANGED;
		}
	}

	/* Color tiles clicked */
	for (i = 0; i <= stgs.colors->n; ++i) {
		tile = get_menu_tile_pos(i);
		if (area_contains(tile, MENU_TILE_SIZE, MENU_TILE_SIZE, pos)) {
			if (event.bstate & BUTTON1_CLICKED) {
				open_dialog(pos, (i == stgs.colors->n) ? CIDX_NEWCOLOR : (color_t)i);
			} else if (event.bstate & BUTTON3_CLICKED) {
				open_dialog(pos, CIDX_DEFAULT);
			}
			return ret | INPUT_MENU_CHANGED;
		}
	}
	if (area_contains(get_menu_cdef_pos(), strlen(dialog_cdef_msg), 1, pos)) {
		open_dialog(pos, CIDX_DEFAULT);
		return ret | INPUT_MENU_CHANGED;
	}

	/* Init size buttons clicked */
	if (area_contains(menu_isize_u_pos, MENU_UDARROW_WIDTH, MENU_UDARROW_HEIGHT, pos)) {
		return ret | isize_button_clicked(1);
	}
	if (area_contains(menu_isize_d_pos, MENU_UDARROW_WIDTH, MENU_UDARROW_HEIGHT, pos)) {
		return ret | isize_button_clicked(-1);
	}

	/* Direction buttons clicked */
	if (area_contains(menu_dir_u_pos, MENU_UDARROW_WIDTH, MENU_UDARROW_HEIGHT, pos)) {
		return ret | dir_button_clicked(DIR_UP);
	}
	if (area_contains(menu_dir_r_pos, MENU_RLARROW_WIDTH, MENU_RLARROW_HEIGHT, pos)) {
		return ret | dir_button_clicked(DIR_RIGHT);
	}
	if (area_contains(menu_dir_d_pos, MENU_UDARROW_WIDTH, MENU_UDARROW_HEIGHT, pos)) {
		return ret | dir_button_clicked(DIR_DOWN);
	}
	if (area_contains(menu_dir_l_pos, MENU_RLARROW_WIDTH, MENU_RLARROW_HEIGHT, pos)) {
		return ret | dir_button_clicked(DIR_LEFT);
	}

	/* Speed buttons clicked */
	if (area_contains(menu_speed_u_pos, MENU_UDARROW_WIDTH, MENU_UDARROW_HEIGHT, pos)) {
		return ret | speed_button_clicked(1);
	}
	if (area_contains(menu_speed_d_pos, MENU_UDARROW_WIDTH, MENU_UDARROW_HEIGHT, pos)) {
		return ret | speed_button_clicked(-1);
	}

	/* Control buttons clicked */
	if (area_contains(menu_play_pos, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT, pos)) {
		return ret | play_button_clicked();
	}
	if (area_contains(menu_pause_pos, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT, pos)) {
		return ret | pause_button_clicked();
	}
	if (area_contains(menu_stop_pos, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT, pos)) {
		return ret | stop_button_clicked();
	}

	/* IO buttons clicked */
	if (area_contains(menu_load_pos, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT, pos)) {
		return ret | io_button_clicked(TRUE);
	}
	if (area_contains(menu_save_pos, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT, pos)) {
		return ret | io_button_clicked(FALSE);
	}

	return ret;
}
