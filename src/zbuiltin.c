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

/* Built-in Functions */

/* In This File:
 * - Implementation of built-in functions.
 * - Wrapping of built-in functions.
 */

#include <stdio.h>

#include "ztypes.h"
#include "zerr.h"
#include "zgc.h"

#include "znone.h"
#include "zbool.h"
#include "zbyte.h"
#include "zword.h"
#include "zbytearray.h"
#include "zbignum.h"
#include "zlist.h"
#include "zdict.h"
#include "zfunc.h"

#include "zobject.h"

#include "zbuiltin.h"

/* tname(o) */
Zob *
ztname(List *args)
{
    return typename(args->first->object);
}

/* refc(o) */
Zob *
zrefc(List *args)
{
    Byte *rval = newbyte();

    rval->value = ((RefC *) args->first->object)->refc;
    return (Zob *) rval;
}

/* print(s) */
Zob *
zprint(List *args)
{
    char buffer[1024];
    Word *rval = newword();

    rval->value = repplain(buffer, (ByteArray *) args->first->object);
    printf("%s", buffer);
    return (Zob *) rval;
}

/* repr(x) */
Zob *
zrepr(List *args)
{
    char buffer[1024];
    ByteArray *rval;

    repobj(buffer, args->first->object);
    rval = yarrfromstr(buffer);
    return (Zob *) rval;
}

/* len(x) */
Zob *
zlen(List *args)
{
    Word *rval = newword();
    Zob *obj = args->first->object;

    switch (*obj) {
        case T_YARR:
            rval->value = ((ByteArray *) obj)->length;
            break;
        case T_LIST:
            rval->value = ((List *) obj)->length;
            break;
        case T_DICT:
            rval->value = ((Dict *) obj)->list->length / 2;
            break;
        default:
            rval->value = 0;
    }
    return (Zob *) rval;
}

/* arr(i) */
Zob *
zarr(List *args)
{
    Zob *i, *a;

    i = args->first->object;
    a = (Zob *) newyarr(1);
    ((ByteArray *) a)->bytes[0] = ((Byte *) i)->value;
    return a;
}

/* concat(s1, s2) */
Zob *
zconcat(List *args)
{
    Zob *s1, *s2;

    s1 = args->first->object;
    s2 = args->first->next->object;
    concat((ByteArray *) s1, (ByteArray *) s2);
    return (Zob *) s1;
}

/* get(d, k, dv) */
Zob *
zget(List *args)
{
    Zob *dict, *key, *defval;
    Node *arg = args->first;

    dict = arg->object;
    key = arg->next->object;
    defval = arg->next->next->object;
    return getkey((Dict *) dict, key, defval);
}

/* +(a, b) */
Zob *
zsum(List *args)
{
    Zob *a, *b, *c;

    a = args->first->object;
    b = args->first->next->object;
    if (*a > *b) {
        c = a;
        a = b;
        b = c;
    }
    switch (*a) {
        case T_BYTE:
            switch (*b) {
                case T_BYTE:
                    c = (Zob *) newbyte();
                    ((Byte *) c)->value = ((Byte *) a)->value + ((Byte *) b)->value;
                    return c;
                case T_WORD:
                    c = (Zob *) newword();
                    ((Word *) c)->value = ((Byte *) a)->value + ((Word *) b)->value;
                    return c;
            }
        case T_WORD:
            switch (*b) {
                case T_WORD:
                    c = (Zob *) newword();
                    ((Word *) c)->value = ((Word *) a)->value + ((Word *) b)->value;
                    return c;
            }
    }
    return (Zob *) newnone();
}

/* ==(a, b) */
Zob *
zeq(List *args)
{
    Zob *a, *b;
    Bool *rval = newbool();

    a = args->first->object;
    b = args->first->next->object;
    rval->value = !cmpobj(a, b);
    return (Zob *) rval;
}

/* <(a, b) */
Zob *
zlt(List *args)
{
    Zob *a, *b;
    Bool *rval = newbool();

    a = args->first->object;
    b = args->first->next->object;
    rval->value = 1;
    switch (*a) {
        case T_BYTE:
            switch (*b) {
                case T_BYTE:
                    rval->value = ((Byte *) a)->value < ((Byte *) b)->value;
                    break;
                case T_WORD:
                    rval->value = ((Byte *) a)->value < ((Word *) b)->value;
                    break;
            }
            break;
        case T_WORD:
            switch (*b) {
                case T_BYTE:
                    rval->value = ((Word *) a)->value < ((Byte *) b)->value;
                    break;
                case T_WORD:
                    rval->value = ((Word *) a)->value < ((Word *) b)->value;
                    break;
            }
    }
    return (Zob *) rval;
}

void
regfunc(Dict *namespace,
        Zob *(*cfunc)(List *args),
        char *name,
        unsigned char arity)
{
    Func *func;
    LowFunc *lowfunc;

    lowfunc = newlowfunc();
    lowfunc->func = cfunc;
    func = newfunc((FImp *) lowfunc, arity);
    setkey(namespace,
           (Zob *) yarrfromstr(name),
           (Zob *) func);
}

Dict *
bbuild()
{
    Dict *builtins = newdict();

    regfunc(builtins, ztname, "tname", 1);
    regfunc(builtins, zrefc, "refc", 1);
    regfunc(builtins, zprint, "print", 1);
    regfunc(builtins, zrepr, "repr", 1);
    regfunc(builtins, zlen, "len", 1);
    regfunc(builtins, zarr, "arr", 1);
    regfunc(builtins, zconcat, "concat", 2);
    regfunc(builtins, zget, "get", 3);
    regfunc(builtins, zsum, "+", 2);
    regfunc(builtins, zeq, "==", 2);
    regfunc(builtins, zlt, "<", 2);

    return builtins;
}
