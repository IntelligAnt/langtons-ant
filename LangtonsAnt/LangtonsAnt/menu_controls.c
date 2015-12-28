#include <stdlib.h>

#include "graphics.h"
#include "io.h"

#define INPUT_WINDOW_WIDTH  (MENU_WINDOW_WIDTH-4)
#define INPUT_WINDOW_HEIGHT 3

static WINDOW *iow;
static const Vector2i io_pos = { MENU_CONTROLS_POS-22,
                                 GRID_WINDOW_SIZE+MENU_WINDOW_WIDTH-INPUT_WINDOW_WIDTH-2 };

static input_t reset_sim(void)
{
	Simulation *sim = stgs.linked_sim;
	if (sim) {
		simulation_delete(sim);
	}
	stgs.linked_sim = simulation_new(stgs.colors, stgs.init_size);
	reset_scroll();
	return INPUT_MENU_CHANGED | INPUT_GRID_CHANGED;
}

static input_t clear_sim(void)
{
	remove_all_colors(stgs.colors);
	return reset_sim();
}

static input_t isz_button_clicked(int i)
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
		return INPUT_NO_CHANGE;
	}
	if (!is_simulation_running(sim) && !has_simulation_started(sim)) { // Sanity check
		return reset_sim();
	}
	return INPUT_MENU_CHANGED;
}

static input_t play_button_clicked(void)
{
	input_t res = INPUT_NO_CHANGE;
	Simulation *sim = stgs.linked_sim;
	if (is_simulation_running(sim)) {
		res |= reset_sim();
		sim = stgs.linked_sim;
	}
	if (sim && has_enough_colors(sim->colors)) {
		run_simulation(sim);
		res |= INPUT_MENU_CHANGED;
	}
	return res;
}

static input_t pause_button_clicked(void)
{
	Simulation *sim = stgs.linked_sim;
	if (is_simulation_running(sim)) {
		halt_simulation(sim);
	}
	return INPUT_NO_CHANGE;
}

static input_t stop_button_clicked(void)
{
	return has_simulation_started(stgs.linked_sim) ? reset_sim() : clear_sim();
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
	Colors *colors;
	short status;
	char filename[FILENAME_BUF_LEN];
	read_filename(filename);
	if (load) {
		status = (colors = load_rules(filename)) ? COLOR_LIME : COLOR_RED;
		wattrset(menuw, GET_PAIR_FOR(status));	// TODO Separate drawing from controls
		mvwvline(menuw, menu_load_pos.y, MENU_WINDOW_WIDTH-3, ACS_BLOCK, MENU_BUTTON_HEIGHT);
		if (colors) {
			if (stgs.colors) {
				assert(stgs.linked_sim->colors);
				clear_sim();
			}
			memcpy(stgs.colors, colors, sizeof(Colors));
			free(colors);
		}
		return INPUT_MENU_CHANGED | INPUT_GRID_CHANGED;
	} else {
		status = (save_rules(filename, stgs.colors) != EOF) ? COLOR_LIME : COLOR_RED;
		wattrset(menuw, GET_PAIR_FOR(status));
		mvwvline(menuw, menu_save_pos.y, MENU_WINDOW_WIDTH-3, ACS_BLOCK, MENU_BUTTON_HEIGHT);
		return INPUT_MENU_CHANGED;
	}
}

input_t menu_key_command(int key)
{
	Simulation *sim = stgs.linked_sim;

	switch (key) {
	case ' ':
		if (is_simulation_running(sim)) {
			return pause_button_clicked();
		} else {
			return play_button_clicked();
		}

	case 'R': case 'r':
		return reset_sim();

	case KEY_BACKSPACE: case '\b':
		return clear_sim();

	case KEY_ESC:
		exit_draw_loop(TRUE);
		return INPUT_NO_CHANGE;

	case KEY_MOUSE:
		return menu_mouse_command();

	default:
		return INPUT_NO_CHANGE;
	}
}

input_t menu_mouse_command(void)
{
	input_t res = INPUT_NO_CHANGE;
	MEVENT event;
	Vector2i event_pos, pos, tile;
	int i;

	nc_getmouse(&event);
	if (!(event.bstate & BUTTON1_CLICKED)) {
		return INPUT_NO_CHANGE;
	}

	event_pos.y = event.y, event_pos.x = event.x;
	pos = abs2rel(event_pos, menu_pos);

	if (dialogw) {
		if (area_contains(dialog_pos, DIALOG_WINDOW_WIDTH, DIALOG_WINDOW_HEIGHT, event_pos)) {
			return dialog_mouse_command(event);
		} else {
			close_dialog();
			res = INPUT_MENU_CHANGED;
		}
	}

	/* Color tiles clicked */
	for (i = 0; i <= stgs.colors->n; ++i) {
		tile = get_menu_tile_pos(i);
		if (area_contains(tile, MENU_TILE_SIZE, MENU_TILE_SIZE, pos)) {
			open_dialog(pos, (i == stgs.colors->n) ? CIDX_NEWCOLOR : i);
			return res | INPUT_MENU_CHANGED;
		}
	}

	/* Control buttons clicked */
	if (area_contains(menu_play_pos, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT, pos)) {
		return res | play_button_clicked();
	}
	if (area_contains(menu_pause_pos, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT, pos)) {
		return res | pause_button_clicked();
	}
	if (area_contains(menu_stop_pos, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT, pos)) {
		return res | stop_button_clicked();
	}

	/* IO buttons clicked */
	if (area_contains(menu_load_pos, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT, pos)) {
		return res | io_button_clicked(TRUE);
	}
	if (area_contains(menu_save_pos, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT, pos)) {
		return res | io_button_clicked(FALSE);
	}

	/* Init size buttons clicked */
	if (area_contains(menu_isz_u_pos, 3, 2, pos)) {
		return res | isz_button_clicked(1);
	}
	if (area_contains(menu_isz_d_pos, 3, 2, pos)) {
		return res | isz_button_clicked(-1);
	}

	return res;
}
