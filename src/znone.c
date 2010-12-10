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

/* ZNone Type */

#include <stdlib.h>
#include <stdio.h>

#include "ztypes.h"
#include "zerr.h"

#include "znone.h"

/* Create a new ZNone in 'znone'.
 * If there is not enough memory, return ZE_OUT_OF_MEMORY.
 * Otherwise, return ZE_OK.
 */
ZError
znewnone(ZNone **znone)
{
    *znone = (ZNone *) malloc(sizeof(ZNone));
    if (*znone == NULL)
        return ZE_OUT_OF_MEMORY;
    (*znone)->type = T_NONE;
    (*znone)->refc = 0;
    return ZE_OK;
}

/* Remove 'znone' from memory. */
void
zdelnone(ZNone **znone)
{
    free(*znone);
    *znone = NULL;
}

/* Create a new copy of 'source' in 'dest'.
 * If there is not enough memory, return ZE_OUT_OF_MEMORY.
 * Otherwise, return ZE_OK.
 */
ZError
zcpynone(ZNone *source, ZNone **dest)
{
    return znewnone(dest);
}

/* Test the truth value of 'znone'.
 * Always return zero.
 */
int
ztstnone(ZNone *znone)
{
    return 0;
}

/* Compare 'znone' and 'other'.
 * Always return zero.
 */
int
zcmpnone(ZNone *znone, ZNone *other)
{
    return 0;
}

/* Print the textual representation of 'znone' on 'buffer'.
 * Return the number of bytes writen.
 */
int
zrepnone(char *buffer, size_t size, ZNone *znone)
{
    return snprintf(buffer, size, "NONE");
}
