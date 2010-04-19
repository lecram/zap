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
 * Word Array Type
 */

#include <stdlib.h>
#include <stdio.h>

#include "zerr.h"
#include "ztypes.h"
#include "zword.h"
#include "zwordarray.h"

WordArray *
newwarr(unsigned int length)
{
    WordArray *wordarray;
    unsigned int *array;

    wordarray = (WordArray *) malloc(sizeof(WordArray));
    if (wordarray == NULL)
        raise("Out of memory in newwarr().");
    array = (unsigned int *) malloc(length * sizeof(unsigned int));
    if (array == NULL)
        raise("Out of memory in newwarr().");
    wordarray->type = T_WARR;
    wordarray->length = length;
    wordarray->words = array;
    wordarray->refc = 0;
    return wordarray;
}

void
delwarr(WordArray **wordarray)
{
    free((*wordarray)->words);
    (*wordarray)->words = NULL;
    free(*wordarray);
    *wordarray = NULL;
}

WordArray *
cpywarr(WordArray *wordarray)
{
    int index;
    WordArray *copy;

    copy = newwarr(wordarray->length);
    for (index = wordarray->length - 1; index >= 0; index--)
        *(copy->words + index) = *(wordarray->words + index);
    return copy;
}

Word *
getworditem(WordArray *wordarray, int index)
{
    Word *word = newword();

    if (index < 0) index += wordarray->length;
    if (index < 0  ||  index >= wordarray->length)
        raise("Index out of range.");
    word->value = wordarray->words[index];
    return word;
}

void
setworditem(WordArray *wordarray, int index, Word *word)
{
    if (index < 0) index += wordarray->length;
    if (index < 0  ||  index >= wordarray->length)
        raise("Index out of range.");
    wordarray->words[index] = word->value;
}

char
eqwarr(WordArray *wordarray, Zob *other)
{
    if (*other != T_WARR) return 0;
    else {
        WordArray *owordarray;

        owordarray = (WordArray *) other;
        if (wordarray->length != owordarray->length)
            return 0;
        else {
            int index;
            unsigned int wordlen;

            wordlen = wordarray->length / WL;
            if (wordarray->length % WL) wordlen++;
            for (index = 0; index < wordlen; index++)
                if (wordarray->words[index] != owordarray->words[index])
                    return 0;
            return 1;
        }
    }
}

unsigned int
repwarr(char *buffer, WordArray *wordarray)
{
    char *tmpbff = buffer;
    int index;
    unsigned int blen = 0;

    *tmpbff++ = '{';
    for (index = 0; index < wordarray->length - 1; index++)
        blen += sprintf(tmpbff + blen,
                        "%u, ",
                        wordarray->words[index]);
    blen += sprintf(tmpbff + blen,
                    "%u}",
                    wordarray->words[wordarray->length - 1]);
    return blen + 2;
}
