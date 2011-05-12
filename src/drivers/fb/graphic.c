/*
 * drivers/fb/graphic.c
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
#include <xboot.h>
#include <malloc.h>
#include <byteorder.h>
#include <rect.h>
#include <fb/bitmap.h>
#include <fb/font.h>
#include <fb/fb.h>
#include <fb/fbfill.h>
#include <fb/fbblit.h>
#include <fb/fbviewport.h>
#include <fb/graphic.h>

/*
 * framebuffer set viewport
 */
bool_t fb_set_viewport(struct fb * fb, struct rect * rect)
{
	if(!fb)
		return FALSE;

	return bitmap_set_viewport(&(fb->info->bitmap), rect);
}

/*
 * framebuffer get viewport
 */
bool_t fb_get_viewport(struct fb * fb, struct rect * rect)
{
	if(!fb)
		return FALSE;

	return bitmap_get_viewport(&(fb->info->bitmap), rect);
}

/*
 * framebuffer map color
 */
u32_t fb_map_color(struct fb * fb, u8_t r, u8_t g, u8_t b, u8_t a)
{
	if(!fb || !fb->map_color)
		return 0;

	return fb->map_color(fb, r, g, b, a);
}

/*
 * framebuffer unmap color
 */
void fb_unmap_color(struct fb * fb, u32_t c, u8_t * r, u8_t * g, u8_t * b, u8_t * a)
{
	if(!fb || !fb->unmap_color)
		return;

	return fb->unmap_color(fb, c, r, g, b, a);
}

/*
 * framebuffer fill rect
 */
bool_t fb_fill_rect(struct fb * fb, u32_t c, u32_t x, u32_t y, u32_t w, u32_t h)
{
	if(!fb || !fb->fill_rect)
		return FALSE;

	return fb->fill_rect(fb, c, x, y, w, h);
}

/*
 * framebuffer blit bitmap
 */
bool_t fb_blit_bitmap(struct fb * fb, struct bitmap * bitmap, enum blit_mode mode, u32_t x, u32_t y, u32_t w, u32_t h, u32_t ox, u32_t oy)
{
	if(!fb || !fb->blit_bitmap)
		return FALSE;

	return fb->blit_bitmap(fb, bitmap, mode, x, y, w, h, ox, oy);
}

/*
 * fill rect to bitmap
 */
bool_t bitmap_fill_rect(struct bitmap * bitmap, u32_t c, u32_t x, u32_t y, u32_t w, u32_t h)
{
	struct rect r, a, b;

	if(!bitmap)
		return FALSE;

	a.left = x;
	a.top = y;
	a.right = x + w;
	a.bottom = y + h;

	b.left = bitmap->viewport.left;
	b.top = bitmap->viewport.top;
	b.right = bitmap->viewport.right;
	b.bottom = bitmap->viewport.bottom;

	if(rect_intersect(&r, &a, & b) == FALSE)
		return FALSE;

	x = r.left;
	y = r.top;
	w = r.right - r.left;
	h = r.bottom - r.top;

	common_bitmap_fill_rect(bitmap, c, x, y, w, h);

	return TRUE;
}

/*
 * bitmap blitter
 */
bool_t bitmap_blit(struct bitmap * dst, struct bitmap * src, enum blit_mode mode, u32_t x, u32_t y, u32_t w, u32_t h, u32_t ox, u32_t oy)
{
	struct rect r, a, b;

	if(!dst || !src)
		return FALSE;

	a.left = x;
	a.top = y;
	a.right = x + w;
	a.bottom = y + h;

	b.left = dst->viewport.left;
	b.top = dst->viewport.top;
	b.right = dst->viewport.right;
	b.bottom = dst->viewport.bottom;

	if(rect_intersect(&r, &a, & b) == FALSE)
		return FALSE;

	x = r.left;
	y = r.top;
	w = r.right - r.left;
	h = r.bottom - r.top;

	common_bitmap_blit(dst, src, mode, x, y, w, h, ox, oy);

	return TRUE;
}
