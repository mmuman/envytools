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

int envy_bios_parse_bit_C (struct envy_bios *bios, struct envy_bios_bit_entry *bit) {
	struct envy_bios_C *c = &bios->C;
	c->bit = bit;

	return 0;
}

void
envy_bios_print_bit_C(struct envy_bios *bios, FILE *out, unsigned mask)
{
	struct envy_bios_C *c = &bios->C;
	int *pointer_lengths = NULL;
	const char **table_names = NULL;
	uint32_t addr;
	int ret = 0, i = 0;

	if (!c->bit || !(mask & ENVY_BIOS_PRINT_C))
		return;

	fprintf(out, "BIT table 'C' at 0x%x, version %i, length %i\n",
		c->bit->offset, c->bit->version, c->bit->t_len);

	if (c->bit->version == 1 && c->bit->t_len == 14) {
		/* some BIOSes use this but it's not documented */
		pointer_lengths = (int[]){4, 4, 2, 2, 2}; /* or is it 4, 4, 2, 4? */
		table_names = (const char *[]){"???", "???", "PLL limits table (=PLL Info Table?)", "???", "???"};
	} else if (c->bit->version == 1 && c->bit->t_len == 18) {
		pointer_lengths = (int[]){4, 4, 2, 4, 2, 2};
		table_names = (const char *[]){"PLL Register Table", "Clock Script", "PLL Info Table", "Clock Frequency Table", "FIFO Table", "Noise-Aware PLL Table"};
	} else if (c->bit->version == 2 && c->bit->t_len == 24) {
		pointer_lengths = (int[]){4, 4, 4, 4, 4, 4};
		table_names = (const char *[]){"VBE Mode PCLK Table", "Clocks Table", "Clock Programming Table", "NAFLL Table", "ADC Table", "Frequency Controller Table"};
	} else {
		fprintf(out, "Unknown C table version %i, length %i\n\n",
			c->bit->version, c->bit->t_len);
		return;
	}

	for (i = 0; i < c->bit->t_len; i += *pointer_lengths++, table_names++) {
		uint16_t addr16;

		if (*pointer_lengths == 2) {
			ret = bios_u16(bios, c->bit->t_offset + i, &addr16);
			addr = addr16;
		} else if (*pointer_lengths == 4)
			ret = bios_u32(bios, c->bit->t_offset + i, &addr);
		else
			break;

		if (!ret && addr < bios->length) {
			fprintf(out, "0x%02x (%i): 0x%x => %s\n", i, *pointer_lengths, addr, *table_names);
			/* just dump the start for now */
			if (addr > 0)
				envy_bios_dump_hex(bios, out, addr, 32, mask);
		}
	}

	fprintf(out, "\n");
}
