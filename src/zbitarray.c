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

/* Bit Array Type */

#include <stdlib.h>
#include <stdio.h>

#include "zerr.h"
#include "ztypes.h"
#include "zbyte.h"
#include "zbitarray.h"

BitArray *
newiarr(unsigned int length)
{
    BitArray *bitarray;
    char *array;
    unsigned int bytelen;

    bitarray = (BitArray *) malloc(sizeof(BitArray));
    if (bitarray == NULL)
        raise("Out of memory in newiarr().");
    bytelen = length / 8;
    if (length % 8) bytelen++;
    array = (char *) malloc(bytelen * sizeof(char));
    if (array == NULL)
        raise("Out of memory in newiarr().");
    bitarray->type = T_IARR;
    bitarray->length = length;
    bitarray->bytes = array;
    bitarray->refc = 0;
    return bitarray;
}

void
deliarr(BitArray **bitarray)
{
    free((*bitarray)->bytes);
    (*bitarray)->bytes = NULL;
    free(*bitarray);
    *bitarray = NULL;
}

BitArray *
cpyiarr(BitArray *bitarray)
{
    int index;
    unsigned int bytelen;
    BitArray *copy;

    bytelen = bitarray->length / 8;
    if (bitarray->length % 8) bytelen++;
    copy = newiarr(bitarray->length);
    for (index = bytelen - 1; index >=0; index--)
        *(copy->bytes + index) = *(bitarray->bytes + index);
    return copy;
}

Byte *
getbititem(BitArray *bitarray, int index)
{
    Byte *byte = newbyte();

    if (index < 0) index += bitarray->length;
    if (index < 0  ||  index >= bitarray->length)
        raise("Index out of range.");
    if (bitarray->bytes[index / 8] & (1 << (index % 8))) {
        byte->value = 1;
        return byte;
    }
    byte->value = 0;
    return byte;
}

void
setbititem(BitArray *bitarray, int index)
{
    if (index < 0) index += bitarray->length;
    if (index < 0  ||  index >= bitarray->length)
        raise("Index out of range.");
    bitarray->bytes[index / 8] |= 1 << (index % 8);
}

void
rstbititem(BitArray *bitarray, int index)
{
    if (index < 0) index += bitarray->length;
    if (index < 0  ||  index >= bitarray->length)
        raise("Index out of range.");
    bitarray->bytes[index / 8] &= ~(1 << (index % 8));
}

char
eqiarr(BitArray *bitarray, Zob *other)
{
    if (*other != T_IARR) return 0;
    else {
        BitArray *obitarray;

        obitarray = (BitArray *) other;
        if (bitarray->length != obitarray->length)
            return 0;
        else {
            int index;
            unsigned int bytelen;
            char mask;

            bytelen = bitarray->length / 8;
            if (bitarray->length % 8) bytelen++;
            for (index = 0; index < bytelen - 1; index++)
                if (bitarray->bytes[index] != obitarray->bytes[index])
                    return 0;
            mask = (1 << (bitarray->length % 8)) - 1;
            if ((bitarray->bytes[index] & mask) != (obitarray->bytes[index] & mask))
                return 0;
            return 1;
        }
    }
}

unsigned int
repiarr(char *buffer, BitArray *bitarray)
{
    char *tmpbff = buffer;
    int index;

    *tmpbff = '|';
    tmpbff++;
    for (index = bitarray->length - 1; index >= 0; index--) {
        if (bitarray->bytes[index / 8] & (1 << (index % 8)))
            sprintf(tmpbff++, "%u", 1);
        else
            sprintf(tmpbff++, "%u", 0);
    }
    *tmpbff++ = '|';
    *tmpbff = '\0';
    return bitarray->length + 2;
}
