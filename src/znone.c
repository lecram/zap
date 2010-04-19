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
 * None Type
 */

#include <stdlib.h>
#include <stdio.h>

#include "zerr.h"
#include "ztypes.h"
#include "znone.h"

None *
newnone()
{
    None *none;

    none = (None *) malloc(sizeof(None));
    if (none == NULL)
        raise("Out of memory in newnone().");
    none->type = T_NONE;
    none->refc = 0;
    return none;
}

void
delnone(None **none)
{
    free(*none);
    *none = NULL;
}

None *
cpynone(None *none)
{
    None *copy;

    copy = newnone();
    return copy;
}

char
eqnone(None *none, Zob *other)
{
    if (*other != T_NONE) return 0;
    return 1;
}

unsigned int
repnone(char *buffer, None *none)
{
    return sprintf(buffer, "NONE");
}
