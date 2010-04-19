/*
 * Garbage Collector
 */

#include "zap.h"

void
increfc(Zob *object)
{
    ((RefC *) object)->refc++;
}

void
decrefc(Zob *object)
{
    if (((RefC *) object)->refc <= 1)
        delobj(&object);
    else
        ((RefC *) object)->refc--;
}
