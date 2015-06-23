#include "graphics.h"

chtype fg_pair, bg_pair;

void init_graphics(short fg_color, short bg_color)
{
	initscr();
	resize_term(GRID_WINDOW_SIZE, GRID_WINDOW_SIZE + MENU_WINDOW_SIZE);
	curs_set(0);
	
	cbreak();
	noecho();
	mousemask(BUTTON1_CLICKED | BUTTON3_CLICKED, NULL); // Left and right click
	//keypad(stdscr, TRUE);
	//nodelay(stdscr, TRUE);

	start_color();
	init_def_pairs(fg_color, bg_color);

	init_grid_window();
	draw_grid_full(NULL);

	init_menu_window();
	draw_menu();
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

short get_color_for(chtype pair)
{
	return PAIR_NUMBER(pair) - 1;
}

void draw_box(WINDOW *w, Vector2i top_left, int size)
{
	int i;
	if (size == 1) {
		mvwaddch(w, top_left.y, top_left.x, ACS_BLOCK);
		return;
	}
	for (i = 0; i < size; ++i) {
		mvwhline(w, top_left.y+i, top_left.x, ACS_BLOCK, size);
	}
}

void draw_bitmap(WINDOW *w, Vector2i top_left,
				 const unsigned char *bitmap, size_t width, size_t height)
{
	size_t read, y, x;
	unsigned char bit;
	for (read = 0; read < width*height; ++read) {
		bit = bitmap[read/8] >> (7-read%8) & 0x1;
		y = read/width, x = read%width;
		mvwaddch(w, top_left.y+y, top_left.x+x, bit ? ACS_BLOCK : ' ');
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

int sgn(int x)
{
	return (x > 0) - (x < 0);
}
