#include "graphics.h"

chtype fg_pair, bg_pair;

void init_graphics(short fg_color, short bg_color)
{
	initscr();
	resize_term(GRID_WINDOW_SIZE, GRID_WINDOW_SIZE + MENU_WINDOW_SIZE);
	curs_set(0);
	
	cbreak();
	keypad(stdscr, TRUE);
	nodelay(stdscr, TRUE);
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

void draw_box(WINDOW *w, Vector2i top_left, int size)
{
	int i;
	if (size == 1) {
		mvwaddch(w, top_left.y, top_left.x, GRID_CELL);
		return;
	}
	for (i = 0; i < size; ++i) {
		mvwhline(w, top_left.y+i, top_left.x, GRID_CELL, size);
	}
}
