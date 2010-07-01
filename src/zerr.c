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
raise(char *msg)
{
    printf("Error: %s\n", msg);
}

void
raiseOutOfMemory(const char *caller)
{
    printf("Error: Out of memory in %s().\n", caller);
}

void
raiseUnknownTypeNumber(const char *caller, unsigned char tnum)
{
    printf("Error: Unknown type number in %s(): %u.\n", caller, tnum);
}

void
raiseIndexOutOfRange(const char *caller, int index, int length)
{
    printf("Error: Index out of range in %s(): %d > %d.\n",
           caller, index, length - 1);
}

void
raiseNameNotDefined(const char *name)
{
    printf("Error: Name \"%s\" is not defined.\n", name);
}

void
raiseFunctionNameNotDefined(const char *fname)
{
    printf("Error: Function name \"%s\" is not defined.\n", fname);
}

void
raiseArityError(unsigned int passed,
                unsigned char expected,
                const char *fname)
{
    printf("Error: %d argument(s) passed to %d-ary function %s().\n",
           passed,
           expected,
           fname);
}
