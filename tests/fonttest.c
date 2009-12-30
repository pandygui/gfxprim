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

#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>

#include "GP.h"

SDL_Surface *display = NULL;

static long colors[GP_BASIC_COLOR_COUNT];

static const char *test_strings[] = {
	" !\"#$%&\047()*+,-./0123456789:;<=>?@",
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`",
	"abcdefghijklmnopqrstuvwxyz{|}~",
	"Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor..."
};

static int flag_proportional = 0;

void redraw_screen(void)
{
	SDL_LockSurface(display);
	
	GP_Clear(display, colors[GP_BLACK]);
	
	GP_TextStyle style = GP_DEFAULT_TEXT_STYLE;

	if (flag_proportional)
		style.font = &GP_default_proportional_font;
	else
		style.font = &GP_default_console_font;

	style.foreground = colors[GP_WHITE];

	const size_t TEST_STRING_COUNT = sizeof(test_strings)/sizeof(const char *);
	size_t i;
	for (i = 0; i < TEST_STRING_COUNT; i++) {
		const char * test_string = test_strings[i];

		style.foreground = colors[GP_WHITE];
		style.pixel_width = 1;
		style.pixel_hspace = 0;
		style.pixel_vspace = 0;

		GP_FillRect(display, colors[GP_MID_RED], 16, 100*i + 16,
				16 + GP_TextWidth(&style, test_string),
				100*i + 16 + style.font->height);

		GP_Text(display, &style, 16, 100*i + 16, test_string);
	
		style.foreground = colors[GP_GRAY];
		style.pixel_width = 2;
		style.pixel_vspace = 1;

		GP_Text(display, &style, 34, 100*i + 34, test_string);

		style.foreground = colors[GP_DARK_GRAY];
		style.pixel_width = 4;
		style.pixel_hspace = 1;
		style.pixel_vspace = 1;

		GP_Text(display, &style, 64, 100*i + 64, test_string);
	}

	SDL_UnlockSurface(display);
}

void event_loop(void)
{
	SDL_Event event;

	while (SDL_WaitEvent(&event) > 0) {
		switch (event.type) {

		case SDL_VIDEOEXPOSE:
			redraw_screen();
			SDL_Flip(display);
			break;

		case SDL_KEYDOWN:
			if (event.key.keysym.sym == SDLK_SPACE) {
				flag_proportional = !flag_proportional;
				redraw_screen();
				SDL_Flip(display);
			}
			else if (event.key.keysym.sym == SDLK_ESCAPE) {
				return;
			}
			break;

		case SDL_QUIT:
			return;
		}
	}
}

int main(void)
{
	/* Initialize SDL */
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
		fprintf(stderr, "Could not initialize SDL: %s\n", SDL_GetError());
		return 1;
	}

	/* Create a window with a software back surface */
	display = SDL_SetVideoMode(640, 480, 0, SDL_SWSURFACE);
	if (display == NULL) {
		fprintf(stderr, "Could not open display: %s\n", SDL_GetError());
		goto fail;
	}

	/* Load a set of basic colors */
	GP_LoadBasicColors(display, colors);

	/* Set up a clipping rectangle to test proper clipping of pixels */
	SDL_Rect clip_rect = {10, 10, 620, 460};
	SDL_SetClipRect(display, &clip_rect);

	redraw_screen();
	SDL_Flip(display);

	event_loop();

	SDL_Quit();
	return 0;

fail:
	SDL_Quit();
	return 1;
}

