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
