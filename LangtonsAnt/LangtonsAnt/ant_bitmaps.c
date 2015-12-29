#include "graphics.h"

static const unsigned char ant24_up[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x81, 0x00, 0x00, 0x42, 0x00,
	0x00, 0x24, 0x00, 0x00, 0x18, 0x00, 0x10, 0x3C, 0x80, 0x10, 0x3C, 0x80,
	0x00, 0x99, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x7E, 0x00,
	0x30, 0xBD, 0xC0, 0x00, 0x5A, 0x00, 0x00, 0x99, 0x00, 0x10, 0x3C, 0x80,
	0x10, 0x7E, 0x80, 0x20, 0x7E, 0x40, 0x20, 0x7E, 0x40, 0x00, 0x7E, 0x00,
	0x00, 0x3C, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
static const unsigned char ant24_rt[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x81, 0x00, 0x00, 0x42, 0x00,
	0x00, 0x24, 0x00, 0x00, 0x18, 0x00, 0x10, 0x3C, 0x80, 0x10, 0x3C, 0x80,
	0x00, 0x99, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x7E, 0x00,
	0x30, 0xBD, 0xC0, 0x00, 0x5A, 0x00, 0x00, 0x99, 0x00, 0x10, 0x3C, 0x80,
	0x10, 0x7E, 0x80, 0x20, 0x7E, 0x40, 0x20, 0x7E, 0x40, 0x00, 0x7E, 0x00,
	0x00, 0x3C, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
static const unsigned char ant24_dn[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x81, 0x00, 0x00, 0x42, 0x00,
	0x00, 0x24, 0x00, 0x00, 0x18, 0x00, 0x10, 0x3C, 0x80, 0x10, 0x3C, 0x80,
	0x00, 0x99, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x7E, 0x00,
	0x30, 0xBD, 0xC0, 0x00, 0x5A, 0x00, 0x00, 0x99, 0x00, 0x10, 0x3C, 0x80,
	0x10, 0x7E, 0x80, 0x20, 0x7E, 0x40, 0x20, 0x7E, 0x40, 0x00, 0x7E, 0x00,
	0x00, 0x3C, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
static const unsigned char ant24_lf[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x81, 0x00, 0x00, 0x42, 0x00,
	0x00, 0x24, 0x00, 0x00, 0x18, 0x00, 0x10, 0x3C, 0x80, 0x10, 0x3C, 0x80,
	0x00, 0x99, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x7E, 0x00,
	0x30, 0xBD, 0xC0, 0x00, 0x5A, 0x00, 0x00, 0x99, 0x00, 0x10, 0x3C, 0x80,
	0x10, 0x7E, 0x80, 0x20, 0x7E, 0x40, 0x20, 0x7E, 0x40, 0x00, 0x7E, 0x00,
	0x00, 0x3C, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
static const unsigned char *ant24_bitmap[4] = { ant24_up, ant24_rt, ant24_dn, ant24_lf };

static const unsigned char ant8_up[] = {
	0x24, 0x18, 0x5A, 0x3C, 0x18, 0x3C, 0x5A, 0x00
};
static const unsigned char ant8_rt[] = {
	0x24, 0x18, 0x5A, 0x3C, 0x18, 0x3C, 0x5A, 0x00
};
static const unsigned char ant8_dn[] = {
	0x24, 0x18, 0x5A, 0x3C, 0x18, 0x3C, 0x5A, 0x00
};
static const unsigned char ant8_lf[] = {
	0x24, 0x18, 0x5A, 0x3C, 0x18, 0x3C, 0x5A, 0x00
};
static const unsigned char *ant8_bitmap[4] = { ant8_up, ant8_rt, ant8_dn, ant8_lf };

const unsigned char* get_ant_bitmap(size_t size, Direction dir)
{
	switch (size) {
	case 8:
		return ant8_bitmap[dir];
	case 24:
		return ant24_bitmap[dir];
	default:
		return NULL;
	}
}
