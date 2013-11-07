/*
 * Copyright
 * (C) 2013 Sebastian Hesselbarth <sebastian.hesselbarth@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <common.h>
#include <init.h>
#include <io.h>
#include <asm/memory.h>

static int berlin_init_soc(void)
{
	/* Currently, all is parsed from DT */
	return 0;
}
core_initcall(berlin_init_soc);

void __noreturn reset_cpu(unsigned long addr)
{
	/* TODO: implement proper reset handler */
	while (1)
		;
}
EXPORT_SYMBOL(reset_cpu);
