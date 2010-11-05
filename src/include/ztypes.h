/* Copyright 2010 by Marcel Rodrigues <marcelgmr@gmail.com>
 *
 * This file is part of zap.
 *
 * zap is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * zap is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with zap.  If not, see <http://www.gnu.org/licenses/>.
 */

/* Types (header) */

/* int length */
#define WL (8 * sizeof(unsigned int))

/* Type Signs */
/* Should use enum? */
#define EMPTY   0
#define T_NONE  1
#define T_BOOL  2
#define T_BYTE  3
#define T_WORD  4
#define T_YARR  5  /* Byte Array */
#define T_BNUM  6  /* BigNum     */
#define T_LIST  7
#define T_DICT  8
#define T_FUNC  9

typedef unsigned char Zob;
