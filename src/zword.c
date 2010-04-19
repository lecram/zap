/*
 * Word Type
 */

#include <stdlib.h>
#include <stdio.h>

#include "zerr.h"
#include "ztypes.h"
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

char
eqword(Word *word, Zob *other)
{
    if (*other != T_WORD) return 0;
    if (((Word *) other)->value != word->value) return 0;
    return 1;
}

unsigned int
repword(char *buffer, Word *word)
{
    return sprintf(buffer, "%u", word->value);
}
