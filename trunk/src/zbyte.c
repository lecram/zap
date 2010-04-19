/*
 * Byte Type
 */

#include <stdlib.h>
#include <stdio.h>

#include "zerr.h"
#include "ztypes.h"
#include "zbyte.h"

Byte *
newbyte()
{
    Byte *byte;

    byte = (Byte *) malloc(sizeof(Byte));
    if (byte == NULL)
        raise("Out of memory in newbyte().");
    byte->type = T_BYTE;
    byte->refc = 0;
    return byte;
}

void
delbyte(Byte **byte)
{
    free(*byte);
    *byte = NULL;
}

Byte *
cpybyte(Byte *byte)
{
    Byte *copy;

    copy = newbyte();
    copy->value = byte->value;
    return copy;
}

char
eqbyte(Byte *byte, Zob *other)
{
    if (*other != T_BYTE) return 0;
    if (((Byte *) other)->value != byte->value) return 0;
    return 1;
}

/*
unsigned int
repbyte(char *buffer, Byte *byte)
{
    return sprintf(buffer, "<%u>", byte->value);
}
*/

unsigned int
repbyte(char *buffer, Byte *byte)
{
    if (((byte->value < 32) && (byte->value != '\n'))  ||  (byte->value > 127))
        return sprintf(buffer, "?");
    else
        return sprintf(buffer, "%c", byte->value);
}
