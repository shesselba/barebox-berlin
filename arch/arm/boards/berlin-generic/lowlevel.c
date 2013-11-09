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
#include <sizes.h>
#include <asm/barebox-arm.h>
#include <asm/barebox-arm-head.h>
#include <mach/lowlevel.h>

extern char __dtb_berlin2_sony_nsz_gs7[];
extern char __dtb_berlin2cd_google_chromecast_start[];

#if defined(CONFIG_MACH_SONY_NSZ_GS7)
static const void *__berlin_dtb_start = __dtb_berlin2_sony_nsz_gs7_start;

#elif defined(CONFIG_MACH_GOOGLE_CHROMECAST)
static const void *__berlin_dtb_start = __dtb_berlin2cd_google_chromecast_start;

#else
#error "No supported DT board file selected"
static const void *__berlin_dtb_start = NULL;
#endif

ENTRY_FUNCTION(start_berlin_generic)(void)
{
	u32 fdt = 0;

	__barebox_arm_head();
	arm_cpu_lowlevel_init();

	if (__berlin_dtb_start != NULL)
		fdt = (u32)__berlin_dtb_start - get_runtime_offset();
	berlin_barebox_entry(fdt);
}
