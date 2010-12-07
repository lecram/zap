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

/* Errors */

/* In This File:
 * - Error handling functions.
 */

#include <stdlib.h>
#include <stdio.h>

#include "zerr.h"

void
zraise(char *msg)
{
    printf("Error: %s\n", msg);
}

void
zraisecpl(char *msg, char *srcname, unsigned int linum)
{
    printf("%s[%u]: Error: %s\n", srcname, linum, msg);
}

void
zraiseOutOfMemory(const char *caller)
{
    printf("Error: Out of memory in %s().\n", caller);
}

void
zraiseUnknownTypeNumber(const char *caller, unsigned char tnum)
{
    printf("Error: Unknown type number in %s(): %u.\n", caller, tnum);
}

void
zraiseIndexOutOfRange(const char *caller, int index, int length)
{
    printf("Error: Index out of range in %s(): %d > %d.\n",
           caller, index, length - 1);
}

void
zraiseNameNotDefined(const char *name)
{
    printf("Error: Name \"%s\" is not defined.\n", name);
}

void
zraiseFunctionNameNotDefined(const char *fname)
{
    printf("Error: Function name \"%s\" is not defined.\n", fname);
}

void
zraiseArityError(unsigned int passed,
                 unsigned char expected,
                 const char *fname)
{
    printf("Error: %d argument(s) passed to %d-ary function %s().\n",
           passed,
           expected,
           fname);
}

void
zraiseOpenFileError(const char *name)
{
    printf("Error: Cannot open file \"%s\".\n", name);
}