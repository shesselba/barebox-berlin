/*
 * Copyright (C) 2013
 *  Sebastian Hesselbarth <sebastian.hesselbarth@gmail.com>
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
#include <io.h>
#include <sizes.h>
#include <asm/barebox-arm.h>
#include <asm/barebox-arm-head.h>

/*
 * Determining the actual memory size is still on the TODO list,
 * but for all SoCs RAM starts at 0x00000000. Therefore, we start
 * with a minimal memory setup of 64M and probe correct memory size
 * later from DT.
 */
#define BERLIN_BOOTUP_MEMORY_BASE	0x00000000
#define BERLIN_BOOTUP_MEMORY_SIZE	SZ_64M

void __naked __noreturn berlin_barebox_entry(uint32_t fdtp)
{
	barebox_arm_entry(BERLIN_BOOTUP_MEMORY_BASE,
			  BERLIN_BOOTUP_MEMORY_SIZE, fdtp);
}

void __naked barebox_arm_reset_vector(void)
{
	arm_cpu_lowlevel_init();
	berlin_barebox_entry(0);
}
