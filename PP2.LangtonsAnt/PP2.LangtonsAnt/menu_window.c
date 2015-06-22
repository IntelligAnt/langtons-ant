#include "graphics.h"

WINDOW *menuw;

void init_menu_window(void)
{
	gridw = newwin(GRID_WINDOW_SIZE, MENU_WINDOW_SIZE, 0, GRID_WINDOW_SIZE);
	mousemask(BUTTON1_CLICKED | BUTTON3_CLICKED, NULL); // Left and right click
	keypad(gridw, TRUE);
	nodelay(gridw, TRUE);
}

void end_menu_window(void)
{
	delwin(gridw);
	gridw = NULL;
}

void draw_menu(void);
