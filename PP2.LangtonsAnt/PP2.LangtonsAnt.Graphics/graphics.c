#include "graphics.h"

void init_def_pairs(short def_bg_color)
{
	int i;
	for (i = 0; i < COLOR_COUNT; ++i) {
		init_pair(i+1, i, def_bg_color);
	}
}

short get_pair(short fg_color)
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
