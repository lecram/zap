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

/* ZBigNum Type */

/* A ZBigNum object stores a number on an array of words (unsigned int).
 * The words are stored in little-endian: least significant zint first.
 * Bits inside words are in big-endian: most significant bit first.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ztypes.h"
#include "zerr.h"

#include "zbyte.h"
#include "zbignum.h"

/* Divide 's' by two in place.
 * Return previous last character in 's'.
 * First character in 's' should not be '0'.
 */
char
zhalfstr(char *s)
{
    char curchar = '0', nextchar;
    char outchar, lastchar = '0';
    int index = -1, nonzero = 0, back = 0;

    while (curchar != '\0') {
        nextchar = *(s + index + 1);
        outchar = (nextchar - '0') >> 1;
        outchar += 5 * (curchar & 1) + '0';
        *(s + index + 1 - back) = outchar;
        if (outchar == '0' && nonzero == 0)
            back++;
        else nonzero = 1;
        if (curchar != '\0')
            lastchar = curchar;
        curchar = nextchar;
        index++;
    }
    if (index == back) {
        *s = '0';
        *(s + 1) = 0;
    }
    else
        *(s + index - back) = 0;
    return lastchar;
}

/* Copy the number of bits of the number in 's' to 'bitlen'.
 * If there is not enough memory, return ZE_OUT_OF_MEMORY.
 * Otherwise, return ZE_OK.
 */
ZError
zstrbitlen(char *s, unsigned int *bitlen)
{
    size_t strlength;
    char *tmpstr;

    strlength = strlen(s);
    tmpstr = (char *) malloc(strlength + 1);
    if (tmpstr == NULL)
        return ZE_OUT_OF_MEMORY;
    memcpy(tmpstr, s, strlength + 1);
    *bitlen = 0;
    while (*tmpstr != '0') {
        (void) zhalfstr(tmpstr);
        (*bitlen)++;
    }
    free(tmpstr);
    tmpstr = NULL;
    return ZE_OK;
}

/* Create a new ZBigNum in 'zdict'.
 * If there is not enough memory, return ZE_OUT_OF_MEMORY.
 * Otherwise, return ZE_OK.
 */
ZError
znewbnum(ZBigNum **zbignum, unsigned int length)
{
    unsigned int *array;
    int wordlen;

    if (length < 32)
        length = 32;
    *zbignum = (ZBigNum *) malloc(sizeof(ZBigNum));
    if (*zbignum == NULL)
        return ZE_OUT_OF_MEMORY;
    wordlen = (int) (length / WL);
    if (length % WL)
        wordlen++;
    array = (unsigned int *) calloc((size_t) wordlen, sizeof(unsigned int));
    if (array == NULL)
        return ZE_OUT_OF_MEMORY;
    (*zbignum)->type = T_BNUM;
    (*zbignum)->length = length;
    (*zbignum)->words = array;
    (*zbignum)->refc = 0;
    return ZE_OK;
}

/* Create a new ZBigNum in 'zbignum'.
 * Convert 's' to a number and store it in 'zbignum'.
 * If there is not enough memory, return ZE_OUT_OF_MEMORY.
 * Otherwise, return ZE_OK.
 */
ZError
zbnumfromstr(ZBigNum **zbignum, char *s)
{
    unsigned int length;
    int index = 0;
    char lastchar;
    ZError err;

    err = zstrbitlen(s, &length);
    if (err != ZE_OK)
        return err;
    err = znewbnum(zbignum, length);
    if (err != ZE_OK)
        return err;
    while (length > 0) {
        lastchar = zhalfstr(s);
        if (lastchar % 2)
            (*zbignum)->words[index / WL] |= 1 << (index % WL);
        index++;
        length--;
    }
    return ZE_OK;
}

/* Remove 'zbignum' from memory. */
void
zdelbnum(ZBigNum **zbignum)
{
    free((*zbignum)->words);
    (*zbignum)->words = NULL;
    free(*zbignum);
    *zbignum = NULL;
}

/* Create a new copy of 'source' in 'dest'.
 * If there is not enough memory, return ZE_OUT_OF_MEMORY.
 * Otherwise, return ZE_OK.
 */
ZError
zcpybnum(ZBigNum *source, ZBigNum **dest)
{
    int index, wordlen;
    ZError err;

    wordlen = (int) (source->length / WL);
    if (source->length % WL)
        wordlen++;
    err = znewbnum(dest, source->length);
    if (err != ZE_OK)
        return err;
    for (index = wordlen - 1; index >=0; index--)
        *((*dest)->words + index) = *(source->words + index);
    return ZE_OK;
}

/* Test the truth value of 'zbignum'.
 * If 'zbignum' is 0, return zero.
 * Otherwise, return nonzero.
 */
int
ztstbnum(ZBigNum *zbignum)
{
    /* NYI. */
    if (zbignum->length)
        return 1;
    else
        return 0;
}

/* Compare 'zbignum' and 'other'.
 * If they are equal, return zero.
 * Otherwise, return nonzero.
 */
int
zcmpbnum(ZBigNum *zbignum, ZBigNum *other)
{
    if (zbignum->length != other->length)
        return 1;
    else {
        int index, wordlen;
        unsigned int mask;

        wordlen = (int) (zbignum->length / WL);
        if (zbignum->length % WL)
            wordlen++;
        for (index = 0; index < wordlen - 1; index++)
            if (zbignum->words[index] != other->words[index])
                return 1;
        mask = (1 << (zbignum->length % WL)) - 1;
        if ((zbignum->words[index] & mask) != (other->words[index] & mask))
            return 1;
        return 0;
    }
}

/* Print the textual representation of 'zbignum' on 'buffer'.
 * Return the number of bytes writen.
 */
/* FIXME: should print decimal representation, not binary. */
int
zrepbnum(char *buffer, size_t size, ZBigNum *zbignum)
{
    char *tmpbff = buffer;
    int index;

    *tmpbff = '|';
    tmpbff++;
    for (index = (int) zbignum->length - 1; index >= 0; index--) {
        if (zbignum->words[index / WL] & (1 << (index % WL)))
            snprintf(tmpbff++, size, "%u", 1);
        else
            snprintf(tmpbff++, size, "%u", 0);
    }
    *tmpbff++ = '|';
    *tmpbff = '\0';
    return (int) zbignum->length + 2;
}

/* Return the length of 'zbignum' in bits. */
unsigned int
znlength(ZBigNum *zbignum)
{
    return zbignum->length;
}

/* Copy the 'index'-th bit of 'zbignum' to 'value'.
 * 'value' must be preallocated with znewbyte().
 * If 'index' is negative, use znlength('zbytearray') + 'index'.
 * If 'index' is out of range, return ZE_INDEX_OUT_OF_RANGE.
 * Otherwise, return ZE_OK.
 */
ZError
znget(ZBigNum *zbignum, int index, ZByte **value)
{
    if (index < 0)
        index += zbignum->length;
    if (index < 0 || index >= (int) zbignum->length)
        return ZE_INDEX_OUT_OF_RANGE;
    if (zbignum->words[index / WL] & (1 << (index % WL))) {
        (*value)->value = 1;
        return ZE_OK;
    }
    (*value)->value = 0;
    return ZE_OK;
}

/* Copy 'zbyte' to the 'index'-th bit of 'zbignum'.
 * If 'index' is negative, use znlength('zbignum') + 'index'.
 * If 'index' is out of range, return ZE_INDEX_OUT_OF_RANGE.
 * Otherwise, return ZE_OK.
 */
ZError
znset(ZBigNum *zbignum, int index, ZByte *zbyte)
{
    if (index < 0)
        index += zbignum->length;
    if (index < 0 || index >= (int) zbignum->length)
        return ZE_INDEX_OUT_OF_RANGE;
    zbignum->words[index / WL] |= 1 << (index % WL);
    return ZE_OK;
}

/* Reset the 'index'-th bit of 'zbignum'.
 * If 'index' is negative, use znlength('zbignum') + 'index'.
 * If 'index' is out of range, return ZE_INDEX_OUT_OF_RANGE.
 * Otherwise, return ZE_OK.
 */
ZError
znrst(ZBigNum *zbignum, int index)
{
    if (index < 0)
        index += zbignum->length;
    if (index < 0 || index >= (int) zbignum->length)
        return ZE_INDEX_OUT_OF_RANGE;
    zbignum->words[index / WL] &= ~(1 << (index % WL));
    return ZE_OK;
}

/* Shift 'zbignum' 'shift' bits to the left. */
void
znlshift(ZBigNum *zbignum, unsigned int shift)
{
    int wordshift, bitshift, wordlen, index;

    wordshift = (int) (shift / WL);
    bitshift = (int) (shift % WL);
    wordlen = (int) (zbignum->length / WL);
    if (zbignum->length % WL)
        wordlen++;
    if (wordshift) {
        for (index = wordlen - 1; index >= wordshift; index--)
            zbignum->words[index] = zbignum->words[index - wordshift];
        for (; index >= 0; index--)
            zbignum->words[index] = 0;
    }
    if (bitshift) {
        unsigned int zint;

        for (index = wordlen - 1; index > wordshift; index--) {
            zbignum->words[index] <<= bitshift;
            zint = zbignum->words[index - 1] >> (WL - bitshift);
            zbignum->words[index] |= zint;
        }
        zbignum->words[index] <<= bitshift;
    }
}

/* Shift 'zbignum' 'shift' bits to the right. */
void
znrshift(ZBigNum *zbignum, unsigned int shift)
{
    int wordshift, bitshift, wordlen, index;

    wordshift = (int) (shift / WL);
    bitshift = (int) (shift % WL);
    wordlen = (int) (zbignum->length / WL);
    if (zbignum->length % WL)
        wordlen++;
    if (wordshift) {
        for (index = 0; index < wordlen - wordshift; index++)
            zbignum->words[index] = zbignum->words[index + wordshift];
        for (; index < wordlen; index++)
            zbignum->words[index] = 0;
    }
    if (bitshift) {
        unsigned int zint;

        for (index = 0; index < wordlen - wordshift - 1; index++) {
            zbignum->words[index] >>= bitshift;
            zint = zbignum->words[index + 1] << (WL - bitshift);
            zbignum->words[index] |= zint;
        }
        zbignum->words[index] >>= bitshift;
    }
}
