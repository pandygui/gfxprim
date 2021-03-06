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

#include <core/GP_Pixmap.h>
#include <filters/GP_Rotate.h>

#include "tst_test.h"

#include "common.h"

struct testcase {
	gp_size w, h;

	gp_pixel_type pixel_type;

	uint8_t offset;

	/* result */
	const char *res;

	/* source */
	char src[];
};

static int test_mirror_h(struct testcase *t)
{
	gp_pixmap src, *c;
	int err;

	/* Initialize source pixmap */
	gp_pixmap_init(&src, t->w, t->h, t->pixel_type, t->src);

	/* Set offset to emulate non-byte aligned subpixmaps */
	src.offset = t->offset;

	/* Test with allocated destination */
	c = gp_pixmap_alloc(t->w, t->h, t->pixel_type);

	if (c == NULL) {
		tst_err("Failed to allocate pixmap");
		return TST_UNTESTED;
	}

	gp_filter_mirror_h(&src, c, NULL);

	err = compare_buffers(t->res, c);

	gp_pixmap_free(c);

	/* And with in-place variant */
//	gp_filter_mirror_h(&src, &src, NULL);

//	err |= compare_buffers(t->res, &src);

	if (err)
		return TST_FAILED;

	return TST_SUCCESS;
}

struct testcase testcase_1x1 = {
	.w = 1,
	.h = 1,

	.pixel_type = GP_PIXEL_G8,

	.res = (const char[]) {
		0xf0,
	},

	.src = {
		0xf0,
	}
};

struct testcase testcase_2x2 = {
	.w = 2,
	.h = 2,

	.pixel_type = GP_PIXEL_G8,

	.res = (const char[]) {
		2, 3,
		0, 1,
	},

	.src = {
		0, 1,
		2, 3,
	}
};

struct testcase testcase_10x2 = {
	.w = 10,
	.h = 2,

	.pixel_type = GP_PIXEL_G8,

	.res = (const char[]) {
		0,   1,  2,  3,  4,  5,  6,  7,  8,  9,
		10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
	},

	.src = {
		10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
		0,   1,  2,  3,  4,  5,  6,  7,  8,  9,
	}
};

struct testcase testcase_2x3 = {
	.w = 2,
	.h = 3,

	.pixel_type = GP_PIXEL_G8,

	.res = (const char[]) {
		4, 5,
		2, 3,
		0, 1,
	},

	.src = {
		0, 1,
		2, 3,
		4, 5,
	}
};


struct testcase testcase_3x3 = {
	.w = 3,
	.h = 3,

	.pixel_type = GP_PIXEL_G8,

	.res = (const char[]) {
		6, 7, 8,
		3, 4, 5,
		0, 1, 2,
	},

	.src = {
		0, 1, 2,
		3, 4, 5,
		6, 7, 8,
	}
};

struct testcase testcase_4x4 = {
	.w = 4,
	.h = 4,

	.pixel_type = GP_PIXEL_G8,

	.res = (const char[]) {
		12, 13, 14, 15,
		 8,  9, 10, 11,
		 4,  5,  6,  7,
		 0,  1,  2,  3,
	},

	.src = {
		 0,  1,  2,  3,
		 4,  5,  6,  7,
		 8,  9, 10, 11,
		12, 13, 14, 15,
	}
};

/* Now tests with pixel types that are not byte aligned */
//TODO: Fix comparsion
struct testcase testcase_G1_16x2 = {
	.w = 3,
	.h = 2,

	.pixel_type = GP_PIXEL_G1,

	.offset = 4,

	.res = (const char[]) {
		0x0f, 0xaa, 0xf0,
		0xff, 0xff, 0xff,
	},

	.src = {
		 0x0f,  0xff,  0xf0,
		 0xff,  0xaa,  0xff,
	}
};

static int abort_callback_fn(gp_progress_cb GP_UNUSED(*self))
{
	return 1;
}

static gp_progress_cb abort_callback = {
	.callback = abort_callback_fn,
};

static int test_abort(void)
{
	int ret;
	gp_pixmap *c;

	c = gp_pixmap_alloc(10, 10, GP_PIXEL_G8);

	if (c == NULL) {
		tst_err("Failed to allocate pixmap");
		return TST_UNTESTED;
	}

	ret = gp_filter_mirror_h(c, c, &abort_callback);

	if (ret == 0) {
		tst_msg("Aborted filter haven't returned non-zero");
		return TST_FAILED;
	}

	if (errno != ECANCELED) {
		tst_msg("Errno wasn't set to ECANCELED");
		return TST_FAILED;
	}

	return TST_SUCCESS;
}

static int all_pixels(void)
{
	gp_pixel pixel_type;

	for (pixel_type = 1; pixel_type < GP_PIXEL_MAX; pixel_type++) {
		gp_pixmap *c;

		tst_msg("Trying pixel %s", gp_pixel_type_name(pixel_type));

		c = gp_pixmap_alloc(10, 10, pixel_type);

		if (c == NULL) {
			tst_err("Failed to allocate pixmap");
			return TST_UNTESTED;
		}

		gp_filter_mirror_h(c, c, NULL);

		gp_pixmap_free(c);
	}

	return TST_SUCCESS;
}

const struct tst_suite tst_suite = {
	.suite_name = "Mirror h Filter Testsuite",
	.tests = {
		{.name = "Mirror h 1x1",
		 .tst_fn = test_mirror_h,
		 .data = &testcase_1x1},

		{.name = "Mirror h 2x2",
		 .tst_fn = test_mirror_h,
		 .data = &testcase_2x2},

		{.name = "Mirror h 10x2",
		 .tst_fn = test_mirror_h,
		 .data = &testcase_10x2},

		{.name = "Mirror h 2x3",
		 .tst_fn = test_mirror_h,
		 .data = &testcase_2x3},

		{.name = "Mirror h 3x3",
		 .tst_fn = test_mirror_h,
		 .data = &testcase_3x3},

		{.name = "Mirror h 4x4",
		 .tst_fn = test_mirror_h,
		 .data = &testcase_3x3},

		{.name = "Mirror h G1 16x2",
		 .tst_fn = test_mirror_h,
		 .data = &testcase_G1_16x2},

		{.name = "Mirror h Callback Abort",
		 .tst_fn = test_abort},

		{.name = "Mirror h x Pixels",
		 .tst_fn = all_pixels},

		{.name = NULL}
	}
};
