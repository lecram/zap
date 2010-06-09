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

/* Word Type */

#include <stdlib.h>
#include <stdio.h>

#include "ztypes.h"
#include "zerr.h"

#include "zword.h"

Word *
newword()
{
    Word *word;

    word = (Word *) malloc(sizeof(Word));
    if (word == NULL)
        raise("Out of memory in newword().");
    word->type = T_WORD;
    word->refc = 0;
    return word;
}

void
delword(Word **word)
{
    free(*word);
    *word = NULL;
}

Word *
cpyword(Word *word)
{
    Word *copy;

    copy = newword();
    copy->value = word->value;
    return copy;
}

int
tstword(Word *word)
{
    if (word->value)
        return 1;
    else
        return 0;
}

int
cmpword(Word *word, Word *other)
{
    if (other->value == word->value)
        return 0;
    else
        return 1;
}

unsigned int
repword(char *buffer, Word *word)
{
    return sprintf(buffer, "%u", word->value);
}
