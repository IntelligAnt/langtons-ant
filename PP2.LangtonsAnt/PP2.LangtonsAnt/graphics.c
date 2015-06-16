#include "graphics.h"

chtype fg_pair, bg_pair;

void init_graphics(short fg_color, short bg_color)
{
	initscr();
	resize_term(GRID_WINDOW_SIZE, GRID_WINDOW_SIZE + MENU_WINDOW_SIZE);
	curs_set(0);
	
	cbreak();
	// keypad(stdscr, TRUE);
	noecho();

	start_color();
	init_def_pairs(fg_color, bg_color);

	init_grid_window();
	draw_grid_full(NULL);

	// TODO Init menus
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
			fg_pair = i+1;
		} else if (i == bg_color) {
			bg_pair = i+1;
		}
	}
}

chtype get_pair_for(short color)
{
	short pair = color+1, i = 0, fg, bg;
	pair_content(pair, &fg, &bg);
	if (fg != color) {
		do {
			pair_content(++i, &fg, &bg);
		} while (fg != color);
		pair = (fg == color) ? i : 0;
	}
	return COLOR_PAIR(pair);
}
