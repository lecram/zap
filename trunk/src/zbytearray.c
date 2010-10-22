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

/* Byte Array Type */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ztypes.h"
#include "zerr.h"

#include "zbyte.h"
#include "zbytearray.h"

ByteArray *
newyarr(unsigned int length)
{
    ByteArray *bytearray;
    char *array;

    bytearray = (ByteArray *) malloc(sizeof(ByteArray));
    if (bytearray == NULL) {
        raiseOutOfMemory("newyarr");
        exit(EXIT_FAILURE);
    }
    if (length > 0)
        array = (char *) malloc(length * sizeof(char));
    else
        array = (char *) malloc(1);
    if (array == NULL) {
        raiseOutOfMemory("newyarr");
        exit(EXIT_FAILURE);
    }
    bytearray->type = T_YARR;
    bytearray->length = length;
    bytearray->bytes = array;
    bytearray->refc = 0;
    return bytearray;
}

ByteArray *
yarrfromstr(char *s)
{
    ByteArray *bytearray;
    char *array;
    unsigned int length;

    length = strlen(s);
    bytearray = (ByteArray *) malloc(sizeof(ByteArray));
    if (bytearray == NULL) {
        raiseOutOfMemory("yarrfromstr");
        exit(EXIT_FAILURE);
    }
    array = (char *) malloc((length + 1) * sizeof(char));
    if (array == NULL) {
        raiseOutOfMemory("yarrfromstr");
        exit(EXIT_FAILURE);
    }
    strcpy(array, s);
    bytearray->type = T_YARR;
    bytearray->length = length;
    bytearray->bytes = array;
    bytearray->refc = 0;
    return bytearray;
}

void
delyarr(ByteArray **bytearray)
{
    free((*bytearray)->bytes);
    (*bytearray)->bytes = NULL;
    free(*bytearray);
    *bytearray = NULL;
}

ByteArray *
cpyyarr(ByteArray *bytearray)
{
    int index;
    ByteArray *copy;

    copy = newyarr(bytearray->length);
    for (index = bytearray->length - 1; index >=0; index--)
        *(copy->bytes + index) = *(bytearray->bytes + index);
    return copy;
}

Byte *
getbyteitem(ByteArray *bytearray, int index)
{
    Byte *byte = newbyte();

    if (index < 0)
        index += bytearray->length;
    if (index < 0 || index >= bytearray->length) {
        raiseIndexOutOfRange("getbyteitem", index, bytearray->length);
        exit(EXIT_FAILURE);
    }
    byte->value = bytearray->bytes[index];
    return byte;
}

void
setbyteitem(ByteArray *bytearray, int index, Byte *byte)
{
    if (index < 0)
        index += bytearray->length;
    if (index < 0 || index >= bytearray->length) {
        raiseIndexOutOfRange("setbyteitem", index, bytearray->length);
        exit(EXIT_FAILURE);
    }
    bytearray->bytes[index] = byte->value;
}

void
concatstr(ByteArray *bytearray, char *str)
{
    int length;

    length = strlen(str);
    if (length == 0)
        return;
    bytearray->bytes = realloc(bytearray->bytes,
                               bytearray->length + length);
    if (bytearray->bytes == NULL) {
        raiseOutOfMemory("concatstr");
        exit(EXIT_FAILURE);
    }
    memcpy(bytearray->bytes + bytearray->length, str, length);
    bytearray->length += length;
}

void
concat(ByteArray *bytearray, ByteArray *other)
{
    if (other->length == 0)
        return;
    bytearray->bytes = realloc(bytearray->bytes,
                               bytearray->length + other->length);
    if (bytearray->bytes == NULL) {
        raiseOutOfMemory("concat");
        exit(EXIT_FAILURE);
    }
    memcpy(bytearray->bytes + bytearray->length, other->bytes, other->length);
    bytearray->length += other->length;
}

int
tstyarr(ByteArray *bytearray)
{
    if (bytearray->length > 0)
        return 1;
    else
        return 0;
}

int
cmpyarr(ByteArray *bytearray, ByteArray *other)
{
    if (bytearray->length != other->length)
        return 1;
    else {
        int index;

        for (index = 0; index < bytearray->length; index++)
            if (bytearray->bytes[index] != other->bytes[index])
                return 1;
        return 0;
    }
}

unsigned int
repyarr(char *buffer, ByteArray *bytearray)
{
    char *tmpbff = buffer, character;
    int index, blen = 0;

    *tmpbff++ = '"';
    for (index = 0; index < bytearray->length; index++) {
        character = bytearray->bytes[index];
        if ((character < 32 && character != '\n') || (character < 0))
            blen += sprintf(tmpbff + blen, "?");
        else
            blen += sprintf(tmpbff + blen,
                            "%c",
                            character);
    }
    blen += sprintf(tmpbff + blen, "\"");
    return blen;
}

unsigned int
repplain(char *buffer, ByteArray *bytearray)
{
    char *tmpbff = buffer, character;
    int index, blen = 0;

    for (index = 0; index < bytearray->length; index++) {
        character = bytearray->bytes[index];
        if ((character < 32 && character != '\n') || (character < 0))
            blen += sprintf(tmpbff + blen, "?");
        else
            blen += sprintf(tmpbff + blen,
                            "%c",
                            character);
    }
    return blen;
}