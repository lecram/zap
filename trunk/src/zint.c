/* Copyright 2010-2011 by Marcel Rodrigues <marcelgmr@gmail.com>
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

/* ZInt Type */

#include <stdlib.h>
#include <stdio.h>

#include "ztypes.h"
#include "zerr.h"

#include "zint.h"

/* Create a new ZInt in 'zint'.
 * If there is not enough memory, return ZE_OUT_OF_MEMORY.
 * Otherwise, return ZE_OK.
 */
ZError
znewint(ZInt **zint)
{
    *zint = (ZInt *) malloc(sizeof(ZInt));
    if (*zint == NULL)
        return ZE_OUT_OF_MEMORY;
    (*zint)->type = T_INT;
    (*zint)->refc = 0;
    return ZE_OK;
}

/* Remove 'zint' from memory. */
void
zdelint(ZInt **zint)
{
    free(*zint);
    *zint = NULL;
}

/* Create a new copy of 'source' in 'dest'.
 * If there is not enough memory, return ZE_OUT_OF_MEMORY.
 * Otherwise, return ZE_OK.
 */
ZError
zcpyint(ZInt *source, ZInt **dest)
{
    ZError err;

    err = znewint(dest);
    if (err != ZE_OK)
        return err;
    (*dest)->value = source->value;
    return ZE_OK;
}

/* Test the truth value of 'zint'.
 * Return 'zint->value'.
 */
int
ztstint(ZInt *zint)
{
    return (int) zint->value;
}

/* Compare 'zint' and 'other'.
 * If they are equal, return zero.
 * Otherwise, return nonzero.
 */
int
zcmpint(ZInt *zint, ZInt *other)
{
    if (other->value == zint->value)
        return 0;
    else
        return 1;
}

/* Print the textual representation of 'zint' on 'buffer'.
 * Return the number of bytes writen.
 */
int
zrepint(char *buffer, size_t size, ZInt *zint)
{
    return snprintf(buffer, size, "%d", zint->value);
}
