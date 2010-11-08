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

/* ZBool Type */

#include <stdlib.h>
#include <stdio.h>

#include "ztypes.h"
#include "zerr.h"

#include "zbool.h"

/* Create a new ZBool in 'zbool'.
 * If there is not enough memory, return ZE_OUT_OF_MEMORY.
 * Otherwise, return ZE_OK.
 */
ZError
znewbool(ZBool **zbool)
{
    *zbool = (ZBool *) malloc(sizeof(ZBool));
    if (*zbool == NULL)
        return ZE_OUT_OF_MEMORY;
    (*zbool)->type = T_BOOL;
    (*zbool)->refc = 0;
    return ZE_OK;
}

/* Remove 'zdict' from memory. */
void
zdelbool(ZBool **zbool)
{
    free(*zbool);
    *zbool = NULL;
}

/* Create a new copy of 'source' in 'dest'.
 * If there is not enough memory, return ZE_OUT_OF_MEMORY.
 * Otherwise, return ZE_OK.
 */
ZError
zcpybool(ZBool *source, ZBool **dest)
{
    ZError err;

    err = znewbool(dest);
    if (err != ZE_OK)
        return err;
    (*dest)->value = source->value;
    return ZE_OK;
}

/* Test the truth value of 'zbool'.
 * Return 'zbool->value'.
 */
int
ztstbool(ZBool *zbool)
{
    return (int) zbool->value;
}

/* Compare 'zbool' and 'other'.
 * If they are equal, return zero.
 * Otherwise, return nonzero.
 */
int
zcmpbool(ZBool *zbool, ZBool *other)
{
    if (other->value) {
        if (zbool->value)
            return 0;
        else
            return 1;
    }
    else {
        if (!zbool->value)
            return 0;
        else
            return 1;
    }
}

/* Print the textual representation of 'zbool' on 'buffer'.
 * Return the number of bytes writen.
 */
int
zrepbool(char *buffer, size_t size, ZBool *zbool)
{
    if (zbool->value)
        return snprintf(buffer, size, "TRUE");
    else
        return snprintf(buffer, size, "FALSE");
}
