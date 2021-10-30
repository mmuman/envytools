/*
 * Copyright (C) 2021 François Revol <revol@free.fr>
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include "bios.h"
#include <string.h>

int envy_bios_parse_bit_S (struct envy_bios *bios, struct envy_bios_bit_entry *bit) {
	struct envy_bios_S *s = &bios->S;
	s->bit = bit;

	return 0;
}

void
envy_bios_print_bit_S(struct envy_bios *bios, FILE *out, unsigned mask)
{
	struct envy_bios_S *s = &bios->S;
	uint16_t addr;
	uint8_t maxlen;
	int ret = 0, i = 0;

	if (!s->bit || !(mask & ENVY_BIOS_PRINT_S))
		return;

	fprintf(out, "BIT table 'S' at 0x%x, version %i\n",
		s->bit->offset, s->bit->version);
	if (s->bit->version > 2) {
		fprintf(out, "Unknown S table version %i\n\n", s->bit->version);
		return;
	}

	for (i = 0; i * 3 < s->bit->t_len; ++i) {
		ret = bios_u8(bios, s->bit->t_offset + (i * 3 + 2), &maxlen);
		if (ret)
			break;
		ret = bios_u16(bios, s->bit->t_offset + (i * 3), &addr);
		if (!ret && addr && addr < bios->length) {
			char *str = strndup((const char *)&bios->data[addr], maxlen);
			fprintf(out, "STRINGS[0x%02x] [<%02x]: 0x%x => '%s'\n", i, maxlen, addr, str);
			free(str);
		}
	}

	fprintf(out, "\n");
}
