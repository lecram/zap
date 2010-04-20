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
 * Func Type (header)
 */

typedef int FImp;

typedef struct {
    FImp high; /* 0 */
    /* Pointer to C function. */
    Zob *(*func)(List *args);
} LowFunc;

typedef struct {
    FImp high; /* 1 */
    /* Pointer to zap function. */
    char *func;
} HighFunc;

typedef struct {
    Zob type; /* T_FUNC */
    unsigned char refc;
    /* Pointer to LowFunc or HighFunc. */
    FImp *fimp;
    /* Number of arguments that the function takes. */
    unsigned char arity;
} Func;

LowFunc *newlowfunc();
void dellowfunc(LowFunc **lowfunc);
HighFunc *newhighfunc();
void delhighfunc(HighFunc **highfunc);
Func *newfunc(FImp *fimp, unsigned char arity);
void delfunc(Func **func);
Func *cpyfunc(Func *func);
char eqfunc(Func *func, Zob *other);
Zob *callimp(FImp *high, List *args);
unsigned int repfunc(char *buffer, Func *func);
