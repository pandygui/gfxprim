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

  Macros for fixed point arithmetic.

  We use 8 bits for fractional part for coordinates and sizes for Anti Aliased
  primitives.

 */

#ifndef CORE_GP_FIXED_POINT_H
#define CORE_GP_FIXED_POINT_H

#include <stdint.h>

typedef uint8_t GP_FP_Frac;

/*
 * Number of bits used for fractional part.
 */
#define GP_FP_FRAC_BITS 8

/*
 * One
 */
#define GP_FP_1 (1<<GP_FP_FRAC_BITS)

/*
 * One Half
 */
#define GP_FP_1_2 ((1<<(GP_FP_FRAC_BITS - 1))

/*
 * Fraction part bitmask.
 */
#define GP_FP_FRAC_MASK ((1<<GP_FP_FRAC_BITS)-1)

/*
 * Addition.
 */
#define GP_FP_ADD(a, b) ((a)+(b))

/*
 * Substraction.
 */
#define GP_FP_SUB(a, b) ((a)-(b))

/*
 * Floor.
 */
#define GP_FP_FLOOR(a) ((a)>>GP_FP_FRAC_BITS)

/*
 * Ceilling.
 */
#define GP_FP_CEIL(a) (((a)>>GP_FP_FRAC_BITS) + !!(GP_FP_FRAC(a)))

/*
 * Rounding.
 */
#define GP_FP_ROUND(a) (((a) + GP_FP_1_2))>>GP_FP_FRAC_BITS

/*
 * Integer part.
 */
#define GP_FP_INT(a) GP_FP_FLOOR(a)

/*
 * Fractional part.
 */
#define GP_FP_FRAC(a) ((a) & GP_FP_FRAC_MASK)

/*
 * Returns an float.
 */
#define GP_FP_TO_FLOAT(a) (((float)(a))/((float)GP_FP_1))

/*
 * Returns fixed point from integer.
 */
#define GP_FP_FROM_INT(a) ((a)<<GP_FP_FRAC_BITS)

#endif /* CORE_GP_FIXED_POINT_H */
