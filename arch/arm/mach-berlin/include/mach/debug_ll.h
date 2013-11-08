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

#ifndef __MACH_DEBUG_LL_H__
#define __MACH_DEBUG_LL_H__

#include <io.h>

#define SM_APB_BASE		0xf7fc0000
#define SM_APB_UART_BASE	(SM_APB_BASE + 0x9000)
#define SM_APB_UARTn_BASE(n)	(SM_APB_UART_BASE + ((n) * 0x1000))

#define UART_THR	0x00
#define UART_LSR	0x14
# define LSR_THRE	BIT(5)

/* Currently, all accessible UARTs are connected to SM_UART0 */
#define EARLY_UART	SM_APB_UARTn_BASE(0)

static inline void PUTC_LL(char c)
{
	/* Wait until there is space in the FIFO */
	while (!(readl(EARLY_UART + UART_LSR) & LSR_THRE))
		;

	/* Send the character */
	writel(c, EARLY_UART + UART_THR);

	/* Wait to make sure it hits the line */
	while (!(readl(EARLY_UART + UART_LSR) & LSR_THRE))
		;
}
#endif
