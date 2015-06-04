#include "graphics.h"

short bg_pair;

void init_graphics(short bg_color)
{
	initscr();
	resize_term(GRID_WINDOW_SIZE, GRID_WINDOW_SIZE+MENU_WINDOW_SIZE);

	cbreak();
	// keypad(stdscr, TRUE);
	noecho();

	start_color();
	init_def_pairs(bg_color);

	init_grid_window();
	grid_draw_full(NULL);

	// TODO Init menus
}

void end_graphics(void)
{
	end_grid_window();
	endwin();
}

void init_def_pairs(short bg_color)
{
	short i;
	for (i = 0; i < COLOR_COUNT; ++i) {
		init_pair(i+1, i, bg_color);
		if (i == bg_color) {
			bg_pair = i+1;
		}
	}
}

short get_pair_for(short fg_color)
{
	short pair = fg_color+1, i = 0, fg, bg;
	pair_content(pair, &fg, &bg);
	if (fg != fg_color) {
		do {
			pair_content(++i, &fg, &bg);
		} while (fg != fg_color);
		pair = (fg == fg_color) ? i : 0;
	}
	return pair;
}
