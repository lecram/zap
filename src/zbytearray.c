/*
 * Byte Array Type
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "zerr.h"
#include "ztypes.h"
#include "zbyte.h"
#include "zbytearray.h"

ByteArray *
newyarr(unsigned int length)
{
    ByteArray *bytearray;
    char *array;

    bytearray = (ByteArray *) malloc(sizeof(ByteArray));
    if (bytearray == NULL)
        raise("Out of memory in newyarr().");
    if (length)
        array = (char *) malloc(length * sizeof(char));
    else
        array = (char *) malloc(1);
    if (array == NULL)
        raise("Out of memory in newyarr().");
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
    if (bytearray == NULL)
        raise("Out of memory in yarrfromstr().");
    array = (char *) malloc(length * sizeof(char));
    if (array == NULL)
        raise("Out of memory in yarrfromstr().");
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

    if (index < 0) index += bytearray->length;
    if (index < 0  ||  index >= bytearray->length)
        raise("Index out of range.");
    byte->value = bytearray->bytes[index];
    return byte;
}

void
setbyteitem(ByteArray *bytearray, int index, Byte *byte)
{
    if (index < 0) index += bytearray->length;
    if (index < 0  ||  index >= bytearray->length)
        raise("Index out of range.");
    bytearray->bytes[index] = byte->value;
}

void
concatstr(ByteArray *bytearray, char *str)
{
    int length;

    length = strlen(str);
    if (!length)
        return;
    bytearray->bytes = realloc(bytearray->bytes,
                               bytearray->length + length);
    if (bytearray->bytes == NULL)
        raise("Out of memory in concatstr().");
    memcpy(bytearray->bytes + bytearray->length, str, length);
    bytearray->length += length;
}

void
concat(ByteArray *bytearray, ByteArray *other)
{
    if (!other->length)
        return;
    bytearray->bytes = realloc(bytearray->bytes,
                               bytearray->length + other->length);
    if (bytearray->bytes == NULL)
        raise("Out of memory in concat().");
    memcpy(bytearray->bytes + bytearray->length, other->bytes, other->length);
    bytearray->length += other->length;
}

char
eqyarr(ByteArray *bytearray, Zob *other)
{
    if (*other != T_YARR) return 0;
    else {
        ByteArray *obytearray;

        obytearray = (ByteArray *) other;
        if (bytearray->length != obytearray->length)
            return 0;
        else {
            int index;

            for (index = 0; index < bytearray->length; index++)
                if (bytearray->bytes[index] != obytearray->bytes[index])
                    return 0;
            return 1;
        }
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
        if (((character < 32) && (character != '\n'))  ||  (character < 0))
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
        if (((character < 32) && (character != '\n'))  ||  (character < 0))
            blen += sprintf(tmpbff + blen, "?");
        else
            blen += sprintf(tmpbff + blen,
                            "%c",
                            character);
    }
    return blen;
}
