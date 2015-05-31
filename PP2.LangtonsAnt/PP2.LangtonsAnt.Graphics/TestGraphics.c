#include "graphics.h"

int main(void)
{
	int i, ch;
	short r, g, b;

	initscr();
	cbreak();
	keypad(stdscr, TRUE);
	noecho();

	start_color();

	for (i = 0; i < 16; ++i) {
		init_pair(i+1, i, 15);
	}
	
	init_pair(100, COLOR_BLUE, COLOR_SILVER);
	attrset(COLOR_PAIR(100));
	printw("Print shit\n");

	for (i = 0; i < 16; ++i) {
		attrset(COLOR_PAIR(i+1));
		color_content(i, &r, &g, &b);
		printw("@ %d %d %d\n", r, g, b);
	}

	ch = getch();
	if (ch == KEY_F(1)) {
		printw("F1 Key pressed");
	} else {
		printw("The pressed key is ");
		attron(A_BOLD);
		printw("%c", ch);
		attroff(A_BOLD);
	}

	refresh();
	getch();
	endwin();

	return 0;
}