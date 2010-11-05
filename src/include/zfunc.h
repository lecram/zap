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

/* ZFunc Type (header) */

typedef int FImp;

typedef struct {
    FImp high; /* 0 */
    /* Pointer to C function. */
    ZError (*func)(ZList *args, Zob **ret);
} ZLowFunc;

typedef struct {
    FImp high; /* 1 */
    /* Pointer to zap function. */
    char *func;
} ZHighFunc;

typedef struct {
    Zob type; /* T_FUNC */
    unsigned char refc;
    /* Pointer to ZLowFunc or ZHighFunc. */
    FImp *fimp;
    /* Number of arguments that the function takes. */
    unsigned char arity;
} ZFunc;

ZError znewlowfunc(ZLowFunc **zlowfunc);
void zdellowfunc(ZLowFunc **zlowfunc);
ZError znewhighfunc(ZHighFunc **zhighfunc);
void zdelhighfunc(ZHighFunc **zhighfunc);
ZError znewfunc(ZFunc **zfunc, FImp *fimp, unsigned char arity);
void zdelfunc(ZFunc **zfunc);
ZError zcpyfunc(ZFunc *source, ZFunc **dest);
int ztstfunc(ZFunc *zfunc);
int zcmpfunc(ZFunc *zfunc, ZFunc *other);
unsigned int zrepfunc(char *buffer, ZFunc *zfunc);
