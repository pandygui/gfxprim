/*****************************************************************************
 * This file is part of gfxprim library.                                     *
 *                                                                           *
 * Gfxprim is free software; you can redistribute it and/or                  *
 * modify it under the terms of the GNU Lesser General Public                *
 * License as published by the Free Software Foundation; either              *
 * version 2.1 of the License, or (at your option) any later version.        *
 *                                                                           *
 * Gfxprim is distributed in the hope that it will be useful,                *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         *
 * Lesser General Public License for more details.                           *
 *                                                                           *
 * You should have received a copy of the GNU Lesser General Public          *
 * License along with gfxprim; if not, write to the Free Software            *
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,                        *
 * Boston, MA  02110-1301  USA                                               *
 *                                                                           *
 * Copyright (C) 2009-2013 Cyril Hrubis <metan@ucw.cz>                       *
 *                                                                           *
 *****************************************************************************/

#include <errno.h>
#include <string.h>

#include <core/GP_Core.h>
#include <core/GP_FnPerBpp.h>
#include <filters/GP_Rotate.h>

int gp_filter_mirror_v_raw(const gp_pixmap *src, gp_pixmap *dst,
                           gp_progress_cb *callback)
{
	uint32_t bpr = src->bytes_per_row;
	uint8_t  buf[bpr];
	unsigned int y;

	GP_DEBUG(1, "Mirroring image %ux%u vertically", src->w, src->h);

	#warning FIXME: non byte aligned pixels

	/* Note that this should work both for src != dst and src == dst */
	for (y = 0; y < src->h/2; y++) {
		uint8_t *sl1 = GP_PIXEL_ADDR(src, 0, y);
		uint8_t *sl2 = GP_PIXEL_ADDR(src, 0, src->h - y - 1);
		uint8_t *dl1 = GP_PIXEL_ADDR(dst, 0, y);
		uint8_t *dl2 = GP_PIXEL_ADDR(dst, 0, src->h - y - 1);

		memcpy(buf, sl1, bpr);
		memcpy(dl1, sl2, bpr);
		memcpy(dl2, buf, bpr);

		if (gp_progress_cb_report(callback, 2 * y, src->h, src->w)) {
			GP_DEBUG(1, "Operation aborted");
			errno = ECANCELED;
			return 1;
		}
	}

	/* Copy the middle odd line */
	if (src != dst && src->h % 2) {
		y = src->h / 2;

		uint8_t *sl = GP_PIXEL_ADDR(src, 0, y);
		uint8_t *dl = GP_PIXEL_ADDR(dst, 0, y);

		memcpy(dl, sl, bpr);
	}

	gp_progress_cb_done(callback);
	return 0;
}

int gp_filter_mirror_v(const gp_pixmap *src, gp_pixmap *dst,
                       gp_progress_cb *callback)
{
	GP_ASSERT(src->pixel_type == dst->pixel_type,
		  "The src and dst pixel types must match");

	GP_ASSERT(src->w <= dst->w && src->h <= dst->h,
	          "Destination is not large enough");

	if (gp_filter_mirror_v_raw(src, dst, callback))
		return 1;

	return 0;
}

gp_pixmap *gp_filter_mirror_v_alloc(const gp_pixmap *src,
                                    gp_progress_cb *callback)
{
	gp_pixmap *res;

	res = gp_pixmap_copy(src, 0);

	if (res == NULL)
		return NULL;

	if (gp_filter_mirror_v_raw(src, res, callback)) {
		gp_pixmap_free(res);
		return NULL;
	}

	return res;
}

static const char *symmetry_names[] = {
	"90",
	"180",
	"270",
	"H",
	"V",
	NULL,
};

const char **gp_filter_symmetry_names = symmetry_names;

int gp_filter_symmetry_by_name(const char *symmetry)
{
	int i;

	for (i = 0; symmetry_names[i] != NULL; i++)
		if (!strcasecmp(symmetry, symmetry_names[i]))
			return i;

	return -1;
}

gp_pixmap *gp_filter_symmetry_alloc(const gp_pixmap *src,
                                    gp_filter_symmetries symmetry,
			            gp_progress_cb *callback)
{
	switch (symmetry) {
	case GP_ROTATE_90:
		return gp_filter_rotate_90_alloc(src, callback);
	case GP_ROTATE_180:
		return gp_filter_rotate_180_alloc(src, callback);
	case GP_ROTATE_270:
		return gp_filter_rotate_270_alloc(src, callback);
	case GP_MIRROR_H:
		return gp_filter_mirror_h_alloc(src, callback);
	case GP_MIRROR_V:
		return gp_filter_mirror_v_alloc(src, callback);
	default:
		GP_DEBUG(1, "Invalid symmetry %i", (int) symmetry);
		return NULL;
	}
}

int gp_filter_symmetry(const gp_pixmap *src, gp_pixmap *dst,
                       gp_filter_symmetries symmetry,
                       gp_progress_cb *callback)
{
	switch (symmetry) {
	case GP_ROTATE_90:
		return gp_filter_rotate_90(src, dst, callback);
	case GP_ROTATE_180:
		return gp_filter_rotate_180(src, dst, callback);
	case GP_ROTATE_270:
		return gp_filter_rotate_270(src, dst, callback);
	case GP_MIRROR_H:
		return gp_filter_mirror_h(src, dst, callback);
	case GP_MIRROR_V:
		return gp_filter_mirror_v(src, dst, callback);
	default:
		GP_DEBUG(1, "Invalid symmetry %i", (int) symmetry);
		return 1;
	}
}
