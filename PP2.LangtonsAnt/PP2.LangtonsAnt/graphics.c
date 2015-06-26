#include <math.h>
#include "graphics.h"

#define DRAW_EVERY 13
#define INPUT_DELAY 0 // TODO low delay causes lag when A is held

static bool do_draw = TRUE;
chtype fg_pair, bg_pair;

void init_graphics(short fg_color, short bg_color)
{
	initscr();
	resize_term(GRID_WINDOW_SIZE, GRID_WINDOW_SIZE+MENU_WINDOW_WIDTH);
	curs_set(0);
	
	cbreak();
	noecho();
	mousemask(BUTTON1_CLICKED | BUTTON3_CLICKED, NULL); // Left and right click
	keypad(stdscr, TRUE);
	nodelay(stdscr, TRUE);

	start_color();
	init_def_pairs(fg_color, bg_color);
	
	init_grid_window();
	draw_grid_full(NULL);

	init_menu_window();
	draw_menu();

	refresh();
}

void end_graphics(void)
{
	end_grid_window();
	endwin();
}

void init_def_pairs(short fg_color, short bg_color)
{
	short i;
	for (i = 0; i < COLOR_COUNT; ++i) {
		init_pair(i+1, i, bg_color);
		if (i == fg_color) {
			fg_pair = COLOR_PAIR(i+1);
		} else if (i == bg_color) {
			bg_pair = COLOR_PAIR(i+1);
		}
	}
}

static void handle_input(void)
{
	static int input_delay = 0;
	Simulation *sim = stgs.linked_sim;
	int ch;
	if (input_delay-- == 0) {
		ch = getch();
		if (is_simulation_valid(sim)) {
			if (grid_key_command(sim->grid, sim->ant, ch) != ERR) { // TODO wgetch refreshes
				draw_grid_full(sim->grid);
			}
		}
		menu_key_command(ch);
		input_delay = INPUT_DELAY;
	}
}

#define DELAY 10000000.0

void draw_loop(void)
{
	static size_t cnt = 0;
	Simulation *sim;
	Vector2i oldp;
	int delay;

	while (do_draw) {
		handle_input();

		sim = stgs.linked_sim;
		if (is_simulation_valid(sim)) {
			if (!is_simulation_running(sim) || !has_simulation_started(sim)) {
				draw_grid_full(sim->grid);
				cnt = 0;
			}
			if (is_simulation_running(sim)) {
				oldp = sim->ant->pos;
				ant_move(sim->ant, sim->grid, sim->colors);
				grid_silent_expand(sim->grid);

				if (is_ant_out_of_bounds(sim->ant, sim->grid)) {
					grid_expand(sim->grid, sim->ant);
					draw_grid_full(sim->grid);
				} else {
					draw_grid_iter(sim->grid, oldp);
					for (delay = 0; delay < DELAY/pow(sim->steps+1, 0.9); ++delay);
				}

				++(sim->steps);
			}
		}

		if (cnt-- == 0) { // TODO can do better
			draw_menu();
			cnt = DRAW_EVERY;
		}

		doupdate();
	}
}

void exit_draw_loop(bool exit)
{
	do_draw = !exit;
}

void draw_box(WINDOW *w, Vector2i top_left, size_t size)
{
	size_t i;
	if (size == 1) {
		mvwaddch(w, top_left.y, top_left.x, ACS_BLOCK);
		return;
	}
	for (i = 0; i < size; ++i) {
		mvwhline(w, top_left.y+i, top_left.x, ACS_BLOCK, size);
	}
}

void draw_rect(WINDOW *w, Vector2i top_left, size_t width, size_t height)
{
	size_t i;
	if (width == 1 && height == 1) {
		mvwaddch(w, top_left.y, top_left.x, ACS_BLOCK);
		return;
	}
	for (i = 0; i < height; ++i) {
		mvwhline(w, top_left.y + i, top_left.x, ACS_BLOCK, width);
	}
}

void draw_bitmap(WINDOW *w, Vector2i top_left,
				 const unsigned char *bitmap, size_t width, size_t height,
				 bool overwrite)
{
	size_t read, y, x;
	unsigned char bit;
	for (read = 0; read < width*height; ++read) {
		bit = bitmap[read/8] >> (7-read%8) & 0x1;
		y = read/width, x = read%width;
		if (overwrite) {
			mvwaddch(w, top_left.y+y, top_left.x+x, bit ? ACS_BLOCK : ' ');
		} else if (bit) {
			mvwaddch(w, top_left.y+y, top_left.x+x, ACS_BLOCK);
		}
	}
}

Vector2i rel2abs(Vector2i rel, Vector2i origin)
{
	return (Vector2i) {
		.y = origin.y + rel.y,
		.x = origin.x + rel.x
	};
}

Vector2i abs2rel(Vector2i abs, Vector2i origin)
{
	return (Vector2i) {
		.y = abs.y - origin.y,
		.x = abs.x - origin.x
	};
}

bool area_contains(Vector2i top_left, size_t width, size_t height, Vector2i needle)
{
	return (needle.y >= top_left.y && needle.y < top_left.y+height
		 && needle.x >= top_left.x && needle.x < top_left.x+width);
}

int sgn(int x)
{
	return (x > 0) - (x < 0);
}
