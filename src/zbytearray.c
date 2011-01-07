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

/* ZByteArray Type */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ztypes.h"
#include "zerr.h"

#include "zbyte.h"
#include "zbytearray.h"

/* Create a new ZByteArray in 'zbytearray'.
 * If there is not enough memory, return ZE_OUT_OF_MEMORY.
 * Otherwise, return ZE_OK.
 */
ZError
znewyarr(ZByteArray **zbytearray, unsigned int length)
{
    unsigned char *array;

    *zbytearray = (ZByteArray *) malloc(sizeof(ZByteArray));
    if (*zbytearray == NULL)
        return ZE_OUT_OF_MEMORY;
    if (length > 0)
        array = (unsigned char *) malloc(length * sizeof(char));
    else
        array = (unsigned char *) malloc(1);
    if (array == NULL)
        return ZE_OUT_OF_MEMORY;
    (*zbytearray)->type = T_YARR;
    (*zbytearray)->length = length;
    (*zbytearray)->bytes = array;
    (*zbytearray)->refc = 0;
    return ZE_OK;
}

/* Create a new ZByteArray in 'zbytearray'.
 * Copy the contents of 's' in 'zbytearray'.
 * If there is not enough memory, return ZE_OUT_OF_MEMORY.
 * Otherwise, return ZE_OK.
 */
ZError
zyarrfromstr(ZByteArray **zbytearray, char *s)
{
    unsigned char *array;
    size_t length;

    length = strlen(s);
    *zbytearray = (ZByteArray *) malloc(sizeof(ZByteArray));
    if (*zbytearray == NULL)
        return ZE_OUT_OF_MEMORY;
    array = (unsigned char *) malloc((length + 1) * sizeof(char));
    if (array == NULL)
        return ZE_OUT_OF_MEMORY;
    strcpy((char *) array, s);
    (*zbytearray)->type = T_YARR;
    (*zbytearray)->length = (unsigned int) length;
    (*zbytearray)->bytes = array;
    (*zbytearray)->refc = 0;
    return ZE_OK;
}

/* Remove 'zbytearray' from memory. */
void
zdelyarr(ZByteArray **zbytearray)
{
    free((*zbytearray)->bytes);
    (*zbytearray)->bytes = NULL;
    free(*zbytearray);
    *zbytearray = NULL;
}

/* Create a new copy of 'source' in 'dest'.
 * If there is not enough memory, return ZE_OUT_OF_MEMORY.
 * Otherwise, return ZE_OK.
 */
ZError
zcpyyarr(ZByteArray *source, ZByteArray **dest)
{
    int index;
    ZError err;

    err = znewyarr(dest, source->length);
    if (err != ZE_OK)
        return err;
    for (index = (int) source->length - 1; index >=0; index--)
        *((*dest)->bytes + index) = *(source->bytes + index);
    return ZE_OK;
}

/* Test the truth value of 'zbytearray'.
 * If 'zbytearray' is empty, return zero.
 * Otherwise, return nonzero.
 */
int
ztstyarr(ZByteArray *zbytearray)
{
    if (zbytearray->length > 0)
        return 1;
    else
        return 0;
}

/* Compare 'zbytearray' and 'other'.
 * If they are equal, return zero.
 * Otherwise, return nonzero.
 */
int
zcmpyarr(ZByteArray *zbytearray, ZByteArray *other)
{
    if (zbytearray->length != other->length)
        return 1;
    else {
        int index;

        for (index = 0; index < (int) zbytearray->length; index++)
            if (zbytearray->bytes[index] != other->bytes[index])
                return 1;
        return 0;
    }
}

/* Print the textual representation of 'zbytearray' on 'buffer'.
 * Return the number of bytes writen.
 */
int
zrepyarr(char *buffer, size_t size, ZByteArray *zbytearray)
{
    char *tmpbff = buffer;
    char character;
    int index, blen = 0;

    *tmpbff++ = '"';
    for (index = 0; index < (int) zbytearray->length; index++) {
        character = (char) zbytearray->bytes[index];
        if ((character < 32 && character != '\n') || (character < 0))
            blen += snprintf(tmpbff + blen, size, "?");
        else
            blen += snprintf(tmpbff + blen,
                             size,
                             "%c",
                             character);
    }
    blen += snprintf(tmpbff + blen, size, "\"");
    return blen;
}

/* Print the textual representation of 'zbytearray' on 'buffer'.
 * Do not include quotes.
 * Return the number of bytes writen.
 */
int
zrepplain(char *buffer, size_t size, ZByteArray *zbytearray)
{
    char *tmpbff = buffer;
    char character;
    int index, blen = 0;

    for (index = 0; index < (int) zbytearray->length; index++) {
        character = (char) zbytearray->bytes[index];
        if ((character < 32 && character != '\n') || (character < 0))
            blen += snprintf(tmpbff + blen, size, "?");
        else
            blen += snprintf(tmpbff + blen,
                             size,
                             "%c",
                             character);
    }
    return blen;
}

/* Return the length of 'zbytearray' in bytes. */
unsigned int
zalength(ZByteArray *zbytearray)
{
    return zbytearray->length;
}

/* Copy the 'index'-th ZByte of 'zbytearray' to 'value'.
 * 'value' must be preallocated with znewbyte().
 * If 'index' is negative, use zalength('zbytearray') + 'index'.
 * If 'index' is out of range, return ZE_INDEX_OUT_OF_RANGE.
 * Otherwise, return ZE_OK.
 */
ZError
zaget(ZByteArray *zbytearray, int index, ZByte **value)
{
    if (index < 0)
        index += zbytearray->length;
    if (index < 0 || index >= (int) zbytearray->length)
        return ZE_INDEX_OUT_OF_RANGE;
    (*value)->value = zbytearray->bytes[index];
    return ZE_OK;
}

/* Copy 'zbyte' to the 'index'-th ZByte of 'zbytearray'.
 * If 'index' is negative, use zalength('zbytearray') + 'index'.
 * If 'index' is out of range, return ZE_INDEX_OUT_OF_RANGE.
 * Otherwise, return ZE_OK.
 */
ZError
zaset(ZByteArray *zbytearray, int index, ZByte *zbyte)
{
    if (index < 0)
        index += zbytearray->length;
    if (index < 0 || index >= (int) zbytearray->length)
        return ZE_INDEX_OUT_OF_RANGE;
    zbytearray->bytes[index] = zbyte->value;
    return ZE_OK;
}

/* Concatenate 's' to 'zbytearray'.
 * If there is not enough memory, return ZE_OUT_OF_MEMORY.
 * Otherwise, return ZE_OK.
 */
ZError
zconcatstr(ZByteArray *zbytearray, char *s)
{
    size_t length;

    length = strlen(s);
    if (length == 0)
        return ZE_OK;
    zbytearray->bytes = realloc(zbytearray->bytes,
                               zbytearray->length + length);
    if (zbytearray->bytes == NULL)
        return ZE_OUT_OF_MEMORY;
    memcpy(zbytearray->bytes + zbytearray->length, s, length);
    zbytearray->length += (unsigned int) length;
    return ZE_OK;
}

/* Concatenate 'other' to 'zbytearray'.
 * If there is not enough memory, return ZE_OUT_OF_MEMORY.
 * Otherwise, return ZE_OK.
 */
ZError
zconcat(ZByteArray *zbytearray, ZByteArray *other)
{
    if (other->length == 0)
        return ZE_OK;
    zbytearray->bytes = realloc(zbytearray->bytes,
                               zbytearray->length + other->length);
    if (zbytearray->bytes == NULL)
        return ZE_OUT_OF_MEMORY;
    memcpy(zbytearray->bytes + zbytearray->length,
           other->bytes,
           other->length);
    zbytearray->length += other->length;
    return ZE_OK;
}
