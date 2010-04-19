/*
 * Bool Type
 */

#include <stdlib.h>
#include <stdio.h>

#include "zerr.h"
#include "ztypes.h"
#include "zbool.h"

Bool *
newbool()
{
    Bool *bool;

    bool = (Bool *) malloc(sizeof(Bool));
    if (bool == NULL)
        raise("Out of memory in newbool().");
    bool->type = T_BOOL;
    bool->refc = 0;
    return bool;
}

void
delbool(Bool **bool)
{
    free(*bool);
    *bool = NULL;
}

Bool *
cpybool(Bool *bool)
{
    Bool *copy;

    copy = newbool();
    copy->value = bool->value;
    return copy;
}

char
eqbool(Bool *bool, Zob *other)
{
    if (*other != T_BOOL) return 0;
    if (((Bool *) other)->value) {
        if (!bool->value) return 0;
    }
    else if (bool->value) return 0;
    return 1;
}

unsigned int
repbool(char *buffer, Bool *bool)
{
    if (bool->value)
        return sprintf(buffer, "TRUE");
    else
        return sprintf(buffer, "FALSE");
}
