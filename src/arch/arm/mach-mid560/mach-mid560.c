/*
 * arch/arm/mach-mid560/mach-mid560.c
 *
 * Copyright (c) 2007-2010  jianjun jiang <jerryjianjun@gmail.com>
 * official site: http://xboot.org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <configs.h>
#include <default.h>
#include <types.h>
#include <macros.h>
#include <io.h>
#include <mode/mode.h>
#include <xboot/log.h>
#include <xboot/printk.h>
#include <xboot/machine.h>
#include <xboot/initcall.h>
#include <s3c6410-cp15.h>
#include <s3c6410/reg-gpio.h>
#include <s3c6410/reg-wdg.h>


extern u8_t	__text_start[];
extern u8_t __text_end[];
extern u8_t __ramdisk_start[];
extern u8_t __ramdisk_end[];
extern u8_t __data_shadow_start[];
extern u8_t __data_shadow_end[];
extern u8_t __data_start[];
extern u8_t __data_end[];
extern u8_t __bss_start[];
extern u8_t __bss_end[];
extern u8_t __heap_start[];
extern u8_t __heap_end[];
extern u8_t __stack_start[];
extern u8_t __stack_end[];

/*
 * system initial, like power lock
 */
static void mach_init(void)
{
	/* gpk13 high level for power lock */
	writel(S3C6410_GPKCON1, (readl(S3C6410_GPKCON1) & ~(0xf<<20)) | (0x1<<20));
	writel(S3C6410_GPKPUD, (readl(S3C6410_GPKPUD) & ~(0x3<<26)) | (0x2<<26));
	writel(S3C6410_GPKDAT, (readl(S3C6410_GPKDAT) & ~(0x1<<13)) | (0x1<<13));
}

/*
 * system halt, shutdown machine.
 */
static bool_t mach_halt(void)
{
	/* GPF15 low level for backlight off */
	writel(S3C6410_GPFDAT, (readl(S3C6410_GPFDAT) & ~(0x1<<15)) | (0x0<<15));

	/* GPK13 low level for power down */
	writel(S3C6410_GPKCON1, (readl(S3C6410_GPKCON1) & ~(0xf<<20)) | (0x1<<20));
	writel(S3C6410_GPKPUD, (readl(S3C6410_GPKPUD) & ~(0x3<<26)) | (0x1<<26));
	writel(S3C6410_GPKDAT, (readl(S3C6410_GPKDAT) & ~(0x1<<13)) | (0x0<<13));

	/* gpo4 for power lock 2 */
	writel(S3C6410_GPOCON, (readl(S3C6410_GPOCON) & ~(0x3<<8)) | (0x1<<8));
	writel(S3C6410_GPOPUD, (readl(S3C6410_GPOPUD) & ~(0x3<<8)) | (0x1<<8));
	writel(S3C6410_GPODAT, (readl(S3C6410_GPODAT) & ~(0x1<<4)) | (0x0<<4));

	return TRUE;
}

/*
 * reset the cpu by setting up the watchdog timer and let him time out
 */
static bool_t mach_reset(void)
{
	/* disable watchdog */
	writel(S3C6410_WTCON, 0x0000);

	/* initialize watchdog timer count register */
	writel(S3C6410_WTCNT, 0x0001);

	/* enable watchdog timer; assert reset at timer timeout */
	writel(S3C6410_WTCON, 0x0021);

	return TRUE;
}

/*
 * get system mode
 */
static enum mode mach_getmode(void)
{
	u32_t gpm;

	/* set gpm0 intput and pull up */
	writel(S3C6410_GPMCON, (readl(S3C6410_GPMCON) & ~(0xf<<0)) | (0x0<<0));
	writel(S3C6410_GPMPUD, (readl(S3C6410_GPMPUD) & ~(0x3<<0)) | (0x2<<0));

	/* set gpm3 intput and pull up */
	writel(S3C6410_GPMCON, (readl(S3C6410_GPMCON) & ~(0xf<<12)) | (0x0<<12));
	writel(S3C6410_GPMPUD, (readl(S3C6410_GPMPUD) & ~(0x3<<6)) | (0x2<<6));

	/* gpm0 and gpm3 key down */
	gpm = readl(S3C6410_GPMDAT) & 0x09;
	if(gpm == 0x09)
		return MODE_MENU;

	return MODE_MENU;
}

/*
 * clean up system before running os
 */
static bool_t mach_cleanup(void)
{
	/* disable irq */
	irq_disable();

	/* disable fiq */
	fiq_disable();

	/* disable icache */
	icache_disable();

	/* disable dcache */
	dcache_disable();

	/* disable mmu */
	mmu_disable();

	/* disable vic */
	vic_disable();

	return TRUE;
}

/*
 * for anti-piracy
 */
static bool_t mach_genuine(void)
{
	return TRUE;
}

/*
 * a portable data interface for machine.
 */
static struct machine mid560 = {
	.info = {
		.board_name 		= "mid560",
		.board_desc 		= "the smit's mid560 device",
		.board_id			= "0",

		.cpu_name			= "s3c6410x",
		.cpu_desc			= "based on arm11 by samsung",
		.cpu_id				= "0x410fb760",
	},

	.res = {
		.mem_banks = {
			[0] = {
				.start		= 0x50000000,
				.end		= 0x50000000 + SZ_128M - 1,
			},

			[1] = {
				.start		= 0,
				.end		= 0,
			},
		},

		.xtal				= 12*1000*1000,
	},

	.link = {
		.text_start			= (const x_sys)__text_start,
		.text_end			= (const x_sys)__text_end,

		.ramdisk_start		= (const x_sys)__ramdisk_start,
		.ramdisk_end		= (const x_sys)__ramdisk_end,

		.data_shadow_start	= (const x_sys)__data_shadow_start,
		.data_shadow_end	= (const x_sys)__data_shadow_end,

		.data_start			= (const x_sys)__data_start,
		.data_end			= (const x_sys)__data_end,

		.bss_start			= (const x_sys)__bss_start,
		.bss_end			= (const x_sys)__bss_end,

		.heap_start			= (const x_sys)__heap_start,
		.heap_end			= (const x_sys)__heap_end,

		.stack_start		= (const x_sys)__stack_start,
		.stack_end			= (const x_sys)__stack_end,
	},

	.pm = {
		.init 				= mach_init,
		.suspend			= NULL,
		.resume				= NULL,
		.halt				= mach_halt,
		.reset				= mach_reset,
	},

	.misc = {
		.getmode			= mach_getmode,
		.cleanup			= mach_cleanup,
		.genuine			= mach_genuine,
	},

	.priv					= NULL,
};

static __init void mach_mid560_init(void)
{
	if(!machine_register(&mid560))
		LOG_E("failed to register machine 'mid560'");
}

module_init(mach_mid560_init, LEVEL_MACH);
