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
 * BigNum Type
 */

/* A BigNum object stores a number on an array of words (unsigned int).
 * The words are stored in little-endian: least significant word first.
 * Bits inside words are in big-endian: most significant first.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "zerr.h"
#include "ztypes.h"
#include "zbyte.h"
#include "zbignum.h"

/* Divides str by two in place.
 * Return previus last character in str.
 * First character in str should not be '0'.
 */
char
halfstr(char *str)
{
    char curchar = '0', nextchar;
    char outchar, lastchar = '0';
    int index = -1, nonzero = 0, back = 0;

    while (curchar) {
        nextchar = *(str + index + 1);
        outchar = (nextchar - '0') >> 1;
        outchar += 5 * (curchar & 1) + '0';
        *(str + index + 1 - back) = outchar;
        if ((outchar == '0') && (nonzero == 0))
            back++;
        else nonzero = 1;
        if (curchar)
            lastchar = curchar;
        curchar = nextchar;
        index++;
    }
    if (index == back) {
        *str = '0';
        *(str + 1) = 0;
    }
    else
        *(str + index - back) = 0;
    return lastchar;
}

unsigned int
strbitlen(char *str)
{
    unsigned int bitlen = 0;
    unsigned int strlength;
    char *tmpstr;

    strlength = strlen(str);
    tmpstr = (char *) malloc(strlength + 1);
    if (tmpstr == NULL)
        raise("Out of memory in strbitlen().");
    memcpy(tmpstr, str, strlength + 1);
    while (*tmpstr != '0') {
        halfstr(tmpstr);
        bitlen++;
    }
    free(tmpstr);
    tmpstr = NULL;
    return bitlen;
}

BigNum *
newbnum(unsigned int length)
{
    BigNum *bignum;
    unsigned int *array;
    unsigned int wordlen;

    if (length < 32) length = 32;
    bignum = (BigNum *) malloc(sizeof(BigNum));
    if (bignum == NULL)
        raise("Out of memory in newbnum().");
    wordlen = length / WL;
    if (length % WL) wordlen++;
    array = (unsigned int *) calloc(wordlen, sizeof(unsigned int));
    if (array == NULL)
        raise("Out of memory in newbnum().");
    bignum->type = T_BNUM;
    bignum->length = length;
    bignum->words = array;
    bignum->refc = 0;
    return bignum;
}

BigNum *
bnumfromstr(char *str)
{
    BigNum *bignum;
    unsigned int length;
    int index = 0;
    char lastchar;

    length = strbitlen(str);
    bignum = newbnum(length);
    while (length) {
        lastchar = halfstr(str);
        if (lastchar % 2)
            bignum->words[index / WL] |= 1 << (index % WL);
        index++;
        length--;
    }
    return bignum;
}

void
delbnum(BigNum **bignum)
{
    free((*bignum)->words);
    (*bignum)->words = NULL;
    free(*bignum);
    *bignum = NULL;
}

BigNum *
cpybnum(BigNum *bignum)
{
    int index;
    unsigned int wordlen;
    BigNum *copy;

    wordlen = bignum->length / WL;
    if (bignum->length % WL) wordlen++;
    copy = newbnum(bignum->length);
    for (index = wordlen - 1; index >=0; index--)
        *(copy->words + index) = *(bignum->words + index);
    return copy;
}

Byte *
getbit(BigNum *bignum, int index)
{
    Byte *byte = newbyte();

    if (index < 0) index += bignum->length;
    if (index < 0  ||  index >= bignum->length)
        raise("Index out of range.");
    if (bignum->words[index / WL] & (1 << (index % WL))) {
        byte->value = 1;
        return byte;
    }
    byte->value = 0;
    return byte;
}

void
setbit(BigNum *bignum, int index)
{
    if (index < 0) index += bignum->length;
    if (index < 0  ||  index >= bignum->length)
        raise("Index out of range.");
    bignum->words[index / WL] |= 1 << (index % WL);
}

void
rstbit(BigNum *bignum, int index)
{
    if (index < 0) index += bignum->length;
    if (index < 0  ||  index >= bignum->length)
        raise("Index out of range.");
    bignum->words[index / WL] &= ~(1 << (index % WL));
}

void
lshiftbnum(BigNum *bignum, unsigned int shift)
{
    int wordshift, bitshift, wordlen, index;

    wordshift = shift / WL;
    bitshift = shift % WL;
    wordlen = bignum->length / WL;
    if (bignum->length % WL) wordlen++;
    if (wordshift) {
        for (index = wordlen - 1; index >= wordshift; index--)
            bignum->words[index] = bignum->words[index - wordshift];
        for (; index >= 0; index--)
            bignum->words[index] = 0;
    }
    if (bitshift) {
        unsigned int word;

        for (index = wordlen - 1; index > wordshift; index--) {
            bignum->words[index] <<= bitshift;
            word = bignum->words[index - 1] >> (WL - bitshift);
            bignum->words[index] |= word;
        }
        bignum->words[index] <<= bitshift;
    }
}

void
rshiftbnum(BigNum *bignum, unsigned int shift)
{
    int wordshift, bitshift, wordlen, index;

    wordshift = shift / WL;
    bitshift = shift % WL;
    wordlen = bignum->length / WL;
    if (bignum->length % WL) wordlen++;
    if (wordshift) {
        for (index = 0; index < wordlen - wordshift; index++)
            bignum->words[index] = bignum->words[index + wordshift];
        for (; index < wordlen; index++)
            bignum->words[index] = 0;
    }
    if (bitshift) {
        unsigned int word;

        for (index = 0; index < wordlen - wordshift - 1; index++) {
            bignum->words[index] >>= bitshift;
            word = bignum->words[index + 1] << (WL - bitshift);
            bignum->words[index] |= word;
        }
        bignum->words[index] >>= bitshift;
    }
}

void
double_dabble(char *buffer, BigNum *bignum)
{
    
}

char
eqbnum(BigNum *bignum, Zob *other)
{
    if (*other != T_BNUM) return 0;
    else {
        BigNum *obignum;

        obignum = (BigNum *) other;
        if (bignum->length != obignum->length)
            return 0;
        else {
            int index;
            unsigned int wordlen;
            unsigned int mask;

            wordlen = bignum->length / WL;
            if (bignum->length % WL) wordlen++;
            for (index = 0; index < wordlen - 1; index++)
                if (bignum->words[index] != obignum->words[index])
                    return 0;
            mask = (1 << (bignum->length % WL)) - 1;
            if ((bignum->words[index] & mask) != (obignum->words[index] & mask))
                return 0;
            return 1;
        }
    }
}

unsigned int
repbnum(char *buffer, BigNum *bignum)
{
    char *tmpbff = buffer;
    int index;

    *tmpbff = '|';
    tmpbff++;
    for (index = bignum->length - 1; index >= 0; index--) {
        if (bignum->words[index / WL] & (1 << (index % WL)))
            sprintf(tmpbff++, "%u", 1);
        else
            sprintf(tmpbff++, "%u", 0);
    }
    *tmpbff++ = '|';
    *tmpbff = '\0';
    return bignum->length + 2;
}
