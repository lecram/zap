/*
 * None Type
 */

#include <stdlib.h>
#include <stdio.h>

#include "zerr.h"
#include "ztypes.h"
#include "znone.h"

None *
newnone()
{
    None *none;

    none = (None *) malloc(sizeof(None));
    if (none == NULL)
        raise("Out of memory in newnone().");
    none->type = T_NONE;
    none->refc = 0;
    return none;
}

void
delnone(None **none)
{
    free(*none);
    *none = NULL;
}

None *
cpynone(None *none)
{
    None *copy;

    copy = newnone();
    return copy;
}

char
eqnone(None *none, Zob *other)
{
    if (*other != T_NONE) return 0;
    return 1;
}

unsigned int
repnone(char *buffer, None *none)
{
    return sprintf(buffer, "NONE");
}
