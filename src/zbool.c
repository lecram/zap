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

/* Bool Type */

#include <stdlib.h>
#include <stdio.h>

#include "ztypes.h"
#include "zerr.h"

#include "zbool.h"

Bool *
newbool()
{
    Bool *bool;

    bool = (Bool *) malloc(sizeof(Bool));
    if (bool == NULL) {
        raiseOutOfMemory("newbool");
        exit(EXIT_FAILURE);
    }
    bool->type = T_BOOL;
    bool->refc = 0;
    return bool;
}

void
delbool(Bool **bool)
{
    free(*bool);
    *bool = NULL;
}

Bool *
cpybool(Bool *bool)
{
    Bool *copy;

    copy = newbool();
    copy->value = bool->value;
    return copy;
}

int
tstbool(Bool *bool)
{
    return bool->value;
}

int
cmpbool(Bool *bool, Bool *other)
{
    if (other->value) {
        if (bool->value)
            return 0;
        else
            return 1;
    }
    else {
        if (!bool->value)
            return 0;
        else
            return 1;
    }
}

unsigned int
repbool(char *buffer, Bool *bool)
{
    if (bool->value)
        return sprintf(buffer, "TRUE");
    else
        return sprintf(buffer, "FALSE");
}
