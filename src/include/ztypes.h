/*
 * Copyright 2010 by Marcel Rodrigues <marcelgmr@gmail.com>
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

/*
 * Types (header)
 */

/* Word length */
#define WL (8 * sizeof(unsigned int))

/* Type signs */
#define T_NONE  1
#define T_BOOL  2
#define T_BYTE  3
#define T_WORD  4
#define T_IARR  5  /* Bit Array  */
#define T_YARR  6  /* Byte Array */
#define T_WARR  7  /* Word Array */
#define T_BNUM  8  /* BigNum     */
#define T_LIST  9
#define T_DICT 10
#define T_FUNC 11

typedef unsigned char Zob;
