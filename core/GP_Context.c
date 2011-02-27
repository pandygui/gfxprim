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
 * Copyright (C) 2009-2010 Jiri "BlueBear" Dluhos                            *
 *                         <jiri.bluebear.dluhos@gmail.com>                  *
 *                                                                           *
 * Copyright (C) 2009-2010 Cyril Hrubis <metan@ucw.cz>                       *
 *                                                                           *
 *****************************************************************************/

#include "GP.h"

#include <string.h>

inline GP_PixelType GP_GetContextPixelType(const GP_Context *context)
{
	return context->pixel_type;
}

GP_Context *GP_ContextCopy(GP_Context *context, int flag)
{
	GP_Context *new;
	uint8_t *pixels;

	if (context == NULL)
		return NULL;

	new     = malloc(sizeof(GP_Context));
	pixels  = malloc(context->bytes_per_row * context->h);

	if (pixels == NULL || context == NULL) {
		free(pixels);
		free(context);
		return NULL;
	}

	new->pixels = pixels;

	if (flag)
		memcpy(pixels, context->pixels,
		       context->bytes_per_row * context->h);

	new->bpp           = context->bpp;
	new->bytes_per_row = context->bytes_per_row;

	new->w = context->w;
	new->h = context->h;

	new->pixel_type = context->pixel_type;

	/* rotation and mirroring */
	new->axes_swap = context->axes_swap;
	new->y_swap    = context->y_swap;
	new->x_swap    = context->x_swap;

	/* clipping */
	new->clip_w_min = context->clip_w_min;
	new->clip_w_max = context->clip_w_max;
	new->clip_h_min = context->clip_h_min;
	new->clip_h_max = context->clip_h_max;

	return new;
	
}

GP_Context *GP_ContextAlloc(uint32_t w, uint32_t h, GP_PixelType type)
{
	GP_Context *context = malloc(sizeof(GP_Context));
	uint32_t bpp = GP_PixelSize(type);
	uint32_t bpr = (bpp * w) / 8 + !!((bpp * w) % 8);
	void *pixels;

	pixels = malloc(bpr * h);

	if (pixels == NULL || context == NULL) {
		free(pixels);
		free(context);
		return NULL;
	}

	context->pixels         = pixels;
	context->bpp            = bpp;
	context->bytes_per_row  = bpr;

	context->w = w;
	context->h = h;

	context->pixel_type = type;
	
	/* rotation and mirroring */
	context->axes_swap = 0;
	context->y_swap    = 0;
	context->x_swap    = 0;

	/* clipping */
	context->clip_w_min = 0;
	context->clip_w_max = w - 1;
	context->clip_h_min = 0;
	context->clip_h_max = h - 1;

	return context;
}

void GP_ContextFree(GP_Context *context)
{
	free(context->pixels);
	free(context);
}

GP_RetCode GP_ContextDump(GP_Context *context, const char *path)
{
	FILE *f = fopen(path, "w");
	uint32_t x, y;

	if (f == NULL)
		return GP_EBADFILE;

	for (y = 0; y < context->h; y++) {
		for (x = 0; x < context->bytes_per_row; x++)
			fprintf(f, "0x%02x ", ((uint8_t *)context->pixels)[y * context->bytes_per_row + x]);
		fprintf(f, "\n");
	}

	fclose(f);
	return GP_ESUCCESS;
}
