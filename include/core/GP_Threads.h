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
 * Copyright (C) 2009-2012 Cyril Hrubis <metan@ucw.cz>                       *
 *                                                                           *
 *****************************************************************************/

 /*

   A code to ease multithreaded filters and more.

  */

#ifndef CORE_GP_THREADS_H
#define CORE_GP_THREADS_H

#include <pthread.h>

#include <core/GP_ProgressCallback.h>
#include <core/GP_Types.h>

/*
 * Sets default number of threads the library uses
 * (changes the behavior of gp_nr_threads()).
 *
 * 0 == auto
 *      Most of the time, if the image is not too small, this makes
 *      the filter run number of processors (as seen by the operating system)
 *      threads.
 *
 * 1 == one thread
 *      Everything runs in exactly one thread. This is default value.
 *
 *   >= 2
 *      Runs exactly n threads unless the image is too small.
 *
 * This value may also be overriden by the GP_THREADS enviroment variable.
 *
 * Moreover the value may be changed for a single call by settings in progres
 * callback structure.
 */
void gp_nr_threads_set(unsigned int nr);

/*
 * Returns a number of threads to use.
 */
unsigned int gp_nr_threads(gp_size w, gp_size h, gp_progress_cb *callback);

/*
 * Multithreaded progress callback priv data guarded by a mutex.
 */
struct gp_progress_cb_mp_priv {
	float max;
	int abort;
	pthread_mutex_t mutex;
	gp_progress_cb *orig_callback;
};

/*
 * Creates and initalizes a on-the-stack progress callback
 *
 * The intended usage is:
 *
 * GP_PROGRESS_CALLBACK_MP(callback_mp, orig_callback);
 *
 * ...
 *
 * for n threads:
 *	run_filter(..., callback ? &callback_mp : NULL);
 */
#define GP_PROGRESS_CALLBACK_MP(name, callback)                        \
	struct gp_progress_cb_mp_priv name_priv = {                 \
		.max = 0,                                              \
		.abort = 0,                                            \
		.mutex = PTHREAD_MUTEX_INITIALIZER,                    \
		.orig_callback = callback,                             \
	};                                                             \
	GP_PROGRESS_CALLBACK(name, gp_progress_cb_mp, &name_priv);

/*
 * Multithreaded callback function itself.
 */
int gp_progress_cb_mp(gp_progress_cb *self);

#endif /* CORE_GP_THREADS_H */
