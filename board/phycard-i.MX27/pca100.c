	/*
 * Copyright (C) 2007 Sascha Hauer, Pengutronix 
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 */

#include <common.h>
#include <net.h>
#include <cfi_flash.h>
#include <init.h>
#include <environment.h>
#include <asm/arch/imx-regs.h>
#include <fec.h>
#include <asm/arch/gpio.h>
#include <asm/armlinux.h>
#include <asm/mach-types.h>
#include <partition.h>
#include <fs.h>
#include <fcntl.h>
#include <nand.h>
#include <spi/spi.h>
#include <asm/io.h>
#include <asm/arch/imx-nand.h>
#include <asm/arch/imx-pll.h>
#include <gpio.h>

static struct device_d sdram_dev = {
	.name     = "ram",
	.id       = "ram0",

	.map_base = 0xa0000000,
	.size     = 128 * 1024 * 1024,
};

static struct fec_platform_data fec_info = {
	.xcv_type = MII100,
	.phy_addr = 1,
};

static struct device_d fec_dev = {
	.name     = "fec_imx",
	.id       = "eth0",
	.map_base = 0x1002b000,
	.platform_data	= &fec_info,
};

struct imx_nand_platform_data nand_info = {
	.width = 1,
	.hw_ecc = 1,
};

static struct device_d nand_dev = {
	.name     = "imx_nand",
	.map_base = 0xd8000000,
	.platform_data	= &nand_info,
};

static int pca100_devices_init(void)
{
	int i;
	struct device_d *nand;

	unsigned int mode[] = {
		PD0_AIN_FEC_TXD0,
		PD1_AIN_FEC_TXD1,
		PD2_AIN_FEC_TXD2,
		PD3_AIN_FEC_TXD3,
		PD4_AOUT_FEC_RX_ER,
		PD5_AOUT_FEC_RXD1,
		PD6_AOUT_FEC_RXD2,
		PD7_AOUT_FEC_RXD3,
		PD8_AF_FEC_MDIO,
		PD9_AIN_FEC_MDC | GPIO_PUEN,
		PD10_AOUT_FEC_CRS,
		PD11_AOUT_FEC_TX_CLK,
		PD12_AOUT_FEC_RXD0,
		PD13_AOUT_FEC_RX_DV,
		PD14_AOUT_FEC_CLR,
		PD15_AOUT_FEC_COL,
		PD16_AIN_FEC_TX_ER,
		PF23_AIN_FEC_TX_EN,
		PE12_PF_UART1_TXD,
		PE13_PF_UART1_RXD,
		PE14_PF_UART1_CTS,
		PE15_PF_UART1_RTS,
		PD25_PF_CSPI1_RDY,
		PD26_PF_CSPI1_SS2,
		PD27_PF_CSPI1_SS1,
		PD28_PF_CSPI1_SS0,
		PD29_PF_CSPI1_SCLK,
		PD30_PF_CSPI1_MISO,
		PD31_PF_CSPI1_MOSI,
	};

	/* initizalize gpios */
	for (i = 0; i < ARRAY_SIZE(mode); i++)
		imx_gpio_mode(mode[i]);

	/* disable the usb phys */
	imx_gpio_mode((GPIO_PORTB | 23) | GPIO_GPIO | GPIO_IN);
	imx_gpio_direction_output(GPIO_PORTB + 23, 1);
	imx_gpio_mode((GPIO_PORTB | 24) | GPIO_GPIO | GPIO_IN);
	imx_gpio_direction_output(GPIO_PORTB + 24, 1);

	register_device(&nand_dev);
	register_device(&sdram_dev);
	register_device(&fec_dev);

	PCCR1 |= PCCR1_PERCLK2_EN;

	nand = get_device_by_path("/dev/nand0");
	devfs_add_partition("nand0", 0x00000, 0x40000, PARTITION_FIXED, "self_raw");
	dev_add_bb_dev("self_raw", "self0");

	devfs_add_partition("nand0", 0x40000, 0x20000, PARTITION_FIXED, "env_raw");
	dev_add_bb_dev("env_raw", "env0");

	armlinux_add_dram(&sdram_dev);
	armlinux_set_bootparams((void *)0xa0000100);
	armlinux_set_architecture(2149);

	return 0;
}

device_initcall(pca100_devices_init);

static struct device_d pca100_serial_device = {
	.name     = "imx_serial",
	.id       = "cs0",
	.map_base = IMX_UART1_BASE,
	.size     = 4096,
};

static int pca100_console_init(void)
{
	register_device(&pca100_serial_device);
	return 0;
}

console_initcall(pca100_console_init);

#ifdef CONFIG_NAND_IMX_BOOT
void __bare_init nand_boot(void)
{
	imx_nand_load_image((void *)TEXT_BASE, 256 * 1024, 512, 16384);
}
#endif

static int pll_init(void)
{
	int i = 0;

        /* for i.MX27 TO2, Set divider of H264_CLK to zero, NFC to 3. */
	PCDR0 &= ~0x0000FC00;

        /* Configure PCDR */
        /* Configure PCDR1 */
	PCDR1 = 0x09030913;

        /* Configure PCCR0 and PCCR1 */
	PCCR0 = 0x3108480F;
	PCCR1 |= 0x0780;

	CSCR |= CSCR_UPDATE_DIS;

	MPCTL0 = IMX_PLL_PD(0) |
		 IMX_PLL_MFD(51) |
		 IMX_PLL_MFI(7) |
		 IMX_PLL_MFN(35); /* 399 MHz */

	SPCTL0 = IMX_PLL_PD(1) |
		 IMX_PLL_MFD(12) |
		 IMX_PLL_MFI(9) |
		 IMX_PLL_MFN(3); /* SPLL = 2 * 26 * 4.61538 MHz = 240 MHz */

	CSCR = CSCR_MPLL_RESTART | CSCR_SPLL_RESTART | CSCR_ARM_SRC_MPLL |
	       CSCR_MCU_SEL | CSCR_SP_SEL | CSCR_FPM_EN | CSCR_MPEN |
	       CSCR_SPEN | CSCR_ARM_DIV(0) | CSCR_AHB_DIV(3) | CSCR_USB_DIV(3) |
	       CSCR_SD_CNT(3) | CSCR_SSI2_SEL | CSCR_SSI1_SEL | CSCR_H264_SEL |
	       CSCR_MSHC_SEL;

	while (i++ < 1000) {
		while (CCSR & CCSR_32K_SR);
		while (!(CCSR & CCSR_32K_SR));
	}

	return 0;
}

core_initcall(pll_init);
