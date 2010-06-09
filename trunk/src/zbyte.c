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

/* Byte Type */

#include <stdlib.h>
#include <stdio.h>

#include "ztypes.h"
#include "zerr.h"

#include "zbyte.h"

Byte *
newbyte()
{
    Byte *byte;

    byte = (Byte *) malloc(sizeof(Byte));
    if (byte == NULL)
        raise("Out of memory in newbyte().");
    byte->type = T_BYTE;
    byte->refc = 0;
    return byte;
}

void
delbyte(Byte **byte)
{
    free(*byte);
    *byte = NULL;
}

Byte *
cpybyte(Byte *byte)
{
    Byte *copy;

    copy = newbyte();
    copy->value = byte->value;
    return copy;
}

int
tstbyte(Byte *byte)
{
    if (byte->value)
        return 1;
    else
        return 0;
}

int
cmpbyte(Byte *byte, Byte *other)
{
    if (other->value == byte->value)
        return 0;
    else
        return 1;
}

/*
unsigned int
repbyte(char *buffer, Byte *byte)
{
    return sprintf(buffer, "<%u>", byte->value);
}
*/

unsigned int
repbyte(char *buffer, Byte *byte)
{
    return sprintf(buffer, "0x%02X", byte->value);
}
