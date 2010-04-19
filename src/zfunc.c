/*
 * Func Type
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "zap.h"

LowFunc *
newlowfunc()
{
    LowFunc *lowfunc;

    lowfunc = (LowFunc *) malloc(sizeof(LowFunc));
    if (lowfunc == NULL)
        raise("Out of memory in newlowfunc().");
    lowfunc->high = 0;
    return lowfunc;
}

void
dellowfunc(LowFunc **lowfunc)
{
    free(*lowfunc);
    *lowfunc = NULL;
}

HighFunc *
newhighfunc()
{
    HighFunc *highfunc;

    highfunc = (HighFunc *) malloc(sizeof(HighFunc));
    if (highfunc == NULL)
        raise("Out of memory in newhighfunc().");
    highfunc->high = 1;
    return highfunc;
}

void
delhighfunc(Func **highfunc)
{
    free(*highfunc);
    *highfunc = NULL;
}

Func *
newfunc(FImp *fimp)
{
    Func *func;

    func = (Func *) malloc(sizeof(Func));
    if (func == NULL)
        raise("Out of memory in newfunc().");
    func->type = T_FUNC;
    func->fimp = fimp;
    func->refc = 0;
    return func;
}

void
delfunc(Func **func)
{
    free((*func)->fimp);
    (*func)->fimp = NULL;
    free(*func);
    *func = NULL;
}

Func *
cpyfunc(Func *func)
{
    Func *copy;

    copy = newfunc(func->fimp);
    return copy;
}

char
eqfunc(Func *func, Zob *other)
{
    if (*other != T_FUNC) return 0;
    if (((Func *) other)->fimp != func->fimp) return 0;
    return 1;
}

Zob *
callimp(FImp *high, List *args)
{
    if (*high) {
        /* Call zap function. */
        /* NYI */
        return (Zob *) newnone();
    }
    else {
        /* Call C function. */
        return ((LowFunc *) high)->func(args);
    }
}

unsigned int
repfunc(char *buffer, Func *func)
{
    if (*func->fimp)
        return sprintf(buffer, "<zap function>");
    else
        return sprintf(buffer, "<C function>");
}
