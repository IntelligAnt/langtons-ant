#include <stdlib.h>

#include "graphics.h"
#include "io.h"

#define INPUT_WINDOW_WIDTH  (MENU_WINDOW_WIDTH-4)
#define INPUT_WINDOW_HEIGHT 3

static WINDOW *iow;
static const Vector2i io_pos = { MENU_CONTROLS_POS-22,
                                 GRID_WINDOW_SIZE+MENU_WINDOW_WIDTH-INPUT_WINDOW_WIDTH-2 };

static void reset_sim(void)
{
	Simulation *sim = stgs.linked_sim;
	if (sim) {
		simulation_delete(sim);
	}
	stgs.linked_sim = simulation_new(stgs.colors, stgs.init_size);
	reset_scroll();
}

static void clear_sim(void)
{
	remove_all_colors(stgs.colors);
	reset_sim();
}

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
	if (!is_simulation_running(sim) && !has_simulation_started(sim)) {
		reset_sim();
	}
}

static void play_button_clicked(void)
{
	Simulation *sim = stgs.linked_sim;
	if (is_simulation_running(sim)) {
		reset_sim();
	}
	if (is_simulation_valid(sim) && has_enough_colors(sim->colors)) {
		run_simulation(sim);
	}
}

static void pause_button_clicked(void)
{
	Simulation *sim = stgs.linked_sim;
	if (is_simulation_running(sim)) {
		stop_simulation(sim);
	}
}

static void stop_button_clicked(void)
{
	has_simulation_started(stgs.linked_sim) ? reset_sim() : clear_sim();
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

static void io_button_clicked(bool load)
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
	} else {
		status = (save_rules(filename, stgs.colors) != EOF) ? COLOR_LIME : COLOR_RED;
		wattrset(menuw, GET_PAIR_FOR(status));
		mvwvline(menuw, menu_save_pos.y, MENU_WINDOW_WIDTH-3, ACS_BLOCK, MENU_BUTTON_HEIGHT);
	}
}

input_t menu_key_command(int key)
{
	Simulation *sim = stgs.linked_sim;

	switch (key) {
	case ' ':
		is_simulation_running(sim) ? pause_button_clicked() : play_button_clicked();
		break;

	case 'R': case 'r':
		reset_sim();
		break;

	case KEY_BACKSPACE: case '\b':
		clear_sim();
		break;

	case KEY_ESC:
		exit_draw_loop(TRUE);
		break;

	case KEY_MOUSE:
		return menu_mouse_command();

	default:
		return INPUT_NO_CHANGE;
	}

	return INPUT_MENU_CHANGED | INPUT_GRID_CHANGED;
}

input_t menu_mouse_command(void)
{
	MEVENT event;
	Vector2i event_pos, pos, tile;
	int i;

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
		if (area_contains(menu_isz_u_pos, 3, 2, pos)) {
			isz_button_clicked(1);
			return;
		}
		if (area_contains(menu_isz_d_pos, 3, 2, pos)) {
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
		if (area_contains(menu_stop_pos, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT, pos)) {
			stop_button_clicked();
			return;
		}

		/* IO buttons clicked */
		if (area_contains(menu_load_pos, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT, pos)) {
			io_button_clicked(TRUE);
			return;
		}
		if (area_contains(menu_save_pos, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT, pos)) {
			io_button_clicked(FALSE);
			return;
		}

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
