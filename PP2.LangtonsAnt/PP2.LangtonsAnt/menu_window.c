#include <string.h>
#include "graphics.h"

WINDOW *menuw;
Settings settings;

unsigned char logo_bitmap[] = {
	0xE0, 0x00, 0x04, 0x00, 0x10, 0x40, 0x00, 0x04, 0x00,
	0x13, 0x41, 0xDC, 0x77, 0x33, 0x84, 0x42, 0x52, 0x94,
	0x4A, 0x47, 0x4A, 0x52, 0x94, 0x4A, 0x41, 0xF9, 0xD2,
	0x73, 0x32, 0x46, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00,
	0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

unsigned char digit_bitmaps[][2] = {
	{ 0xF6, 0xDE }, { 0x24, 0x92 }, { 0xE7, 0xCE }, { 0xE7, 0x9E }, { 0xB7, 0x92 },
	{ 0xF3, 0x9E }, { 0xF3, 0xDE }, { 0xE4, 0x92 }, { 0xF7, 0xDE }, { 0xF7, 0x9E }
};

unsigned char inf_bitmap[] = {
	0x00, 0x00, 0x03, 0x8e, 0x00, 0x00, 0x04, 0x51, 0x00, 0x00,
	0x04, 0x21, 0x00, 0x00, 0x04, 0x51, 0x00, 0x00, 0x03, 0x8e
};

void init_menu_window(void)
{
	menuw = newwin(GRID_WINDOW_SIZE, MENU_WINDOW_SIZE, 0, GRID_WINDOW_SIZE);
	wbkgd(menuw, bg_pair);
	mousemask(BUTTON1_CLICKED | BUTTON3_CLICKED, NULL); // Left and right click
	keypad(gridw, TRUE);
	nodelay(gridw, TRUE);
}

void end_menu_window(void)
{
	delwin(gridw);
	gridw = NULL;
}

const Vector2i steps_origin = { GRID_WINDOW_SIZE-8, MENU_WINDOW_SIZE-1 };
const Vector2i steps_msg_origin = { GRID_WINDOW_SIZE-4, 2 };

void draw_menu(void)
{
	size_t h = MENU_WINDOW_SIZE, v = GRID_WINDOW_SIZE;

	wattrset(menuw, get_pair_for(MENU_BORDER_COLOR));
	mvwhline(menuw, 0,   0,   ACS_BLOCK, h);
	mvwvline(menuw, 0,   0,   ACS_BLOCK, v);
	mvwhline(menuw, v-1, 0,   ACS_BLOCK, h);
	mvwvline(menuw, 0,   h-1, ACS_BLOCK, v);

	mvwaddstr(menuw, v-4, 2, "STEPS:");

	wattron(menuw, A_REVERSE);
	draw_bitmap(menuw, (Vector2i) { 1, 1 }, logo_bitmap, 40, 9);

	wrefresh(menuw);
}

static void draw_menu_steps(void)
{
	char digits[9], *p;
	size_t digit;
	Vector2i top_left = steps_origin;

	if (settings.steps == 10000000) {
		top_left.x -= 33;
		draw_bitmap(menuw, top_left, inf_bitmap, 32, 5);
		goto exit;
	} else if (settings.steps > 10000000) {
		return;
	}

	sprintf(digits, "%8d", settings.steps);
	wattrset(menuw, fg_pair);
	for (p = digits+7; p > 0 && *p != ' '; --p) {
		top_left.x -= 4;
		digit = *p - '0';
		draw_bitmap(menuw, top_left, digit_bitmaps[digit], 3, 5);
	}

exit:
	wrefresh(menuw);
}
