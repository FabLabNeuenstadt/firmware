/*
 * Copyright (C) 2016 by Daniel Friesel
 *
 * License: You may use, redistribute and/or modify this file under the terms
 * of either:
 * * The GNU LGPL v3 (see COPYING and COPYING.LESSER), or
 * * The 3-clause BSD License (see COPYING.BSD)
 *
 */

#ifndef STATIC_PATTERNS_H_
#define STATIC_PATTERNS_H_

#include <avr/pgmspace.h>

const uint8_t PROGMEM shutdownPattern[] = {
	0x20, 0x40,
	0x01, 0x0f,
	0xff, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0xff,
	0x7e, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x7e,
	0x3c, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x3c,
	0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
	0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00,
	0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const uint8_t PROGMEM flashingPattern[] = {
	0x20, 0x10,
	0x06, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x07, 0x33, 0x55, 0x98, 0x00, 0x00
};

const uint8_t PROGMEM emptyPattern[] = {
	0x10, 0x1a,
	0x20, 0x00,
	' ',   1, ' ', 'O', 'h', 'a', 'i', ' ', '-' ,' ',
	'S', 't', 'o', 'r', 'a', 'g', 'e', ' ', 'i', 's',
	' ', 'e', 'm', 'p', 't', 'y'
};

#endif /* STATIC_PATTERNS_H_ */
