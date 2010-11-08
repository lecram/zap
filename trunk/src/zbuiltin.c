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
ZError
z_tname(ZList *args, Zob **ret)
{
    return ztypename(args->first->object, ret);
}

/* refc(o) */
ZError
z_refc(ZList *args, Zob **ret)
{
    ZError err;

    err = znewbyte((ZByte **) ret);
    if (err != ZE_OK)
        return err;
    ((ZByte *) *ret)->value = ((RefC *) args->first->object)->refc;
    return ZE_OK;
}

/* print(s) */
ZError
z_print(ZList *args, Zob **ret)
{
    char buffer[1024];
    ZError err;

    err = znewint((ZInt **) ret);
    if (err != ZE_OK)
        return err;
    ((ZInt *) *ret)->value = zrepplain(buffer,
                                       1024,
                             (ZByteArray *) args->first->object);
    printf("%s", buffer);
    return ZE_OK;
}

/* repr(x) */
ZError
z_repr(ZList *args, Zob **ret)
{
    char buffer[1024];

    zrepobj(buffer, 1024, args->first->object);
    return zyarrfromstr((ZByteArray **) ret, buffer);
}

/* len(x) */
ZError
z_len(ZList *args, Zob **ret)
{
    Zob *obj = args->first->object;
    ZError err;

    err = znewint((ZInt **) ret);
    if (err != ZE_OK)
        return err;
    switch (*obj) {
        case T_YARR:
            ((ZInt *) *ret)->value = zalength((ZByteArray *) obj);
            break;
        case T_LIST:
            ((ZInt *) *ret)->value = zllength((ZList *) obj);
            break;
        case T_DICT:
            ((ZInt *) *ret)->value = zdlength((ZDict *) obj);
            break;
        default:
            ((ZInt *) *ret)->value = 0;
    }
    return ZE_OK;
}

/* arr(i) */
ZError
z_arr(ZList *args, Zob **ret)
{
    Zob *i;
    ZError err;

    i = args->first->object;
    err = znewyarr((ZByteArray **) ret, 1);
    if (err != ZE_OK)
        return err;
    ((ZByteArray *) *ret)->bytes[0] = ((ZByte *) i)->value;
    return ZE_OK;
}

/* concat(s1, s2) */
ZError
z_concat(ZList *args, Zob **ret)
{
    Zob *s2;
    ZError err;

    *ret = args->first->object;
    s2 = args->first->next->object;
    err = zconcat((ZByteArray *) *ret, (ZByteArray *) s2);
    return err;
}

/* get(d, k, dv) */
ZError
z_get(ZList *args, Zob **ret)
{
    Zob *zdict, *key, *defval;
    ZNode *arg = args->first;
    ZError err;

    err = znewbool((ZBool **) ret);
    if (err != ZE_OK)
        return err;
    zdict = arg->object;
    key = arg->next->object;
    defval = arg->next->next->object;
    ((ZBool *) *ret)->value = zdget((ZDict *) zdict, key, &defval);
    return ZE_OK;
}

/* +(a, b) */
ZError
z_sum(ZList *args, Zob **ret)
{
    Zob *a, *b, *c;
    ZError err;

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
                    err = znewbyte((ZByte **) ret);
                    if (err != ZE_OK)
                        return err;
                    ((ZByte *) *ret)->value = ((ZByte *) a)->value +
                                              ((ZByte *) b)->value;
                    return ZE_OK;
                case T_WORD:
                    err = znewint((ZInt **) ret);
                    if (err != ZE_OK)
                        return err;
                    ((ZInt *) *ret)->value = ((ZByte *) a)->value +
                                             ((ZInt *) b)->value;
                    return ZE_OK;
            }
        case T_WORD:
            switch (*b) {
                case T_WORD:
                    err = znewint((ZInt **) ret);
                    if (err != ZE_OK)
                        return err;
                    ((ZInt *) *ret)->value = ((ZInt *) a)->value +
                                             ((ZInt *) b)->value;
                    return ZE_OK;
            }
    }
    return ZE_OK;
}

/* ==(a, b) */
ZError
z_eq(ZList *args, Zob **ret)
{
    Zob *a, *b;
    ZError err;

    err = znewbool((ZBool **) ret);
    if (err != ZE_OK)
        return err;
    a = args->first->object;
    b = args->first->next->object;
    ((ZBool *) *ret)->value = !zcmpobj(a, b);
    return ZE_OK;
}

/* <(a, b) */
ZError
z_lt(ZList *args, Zob **ret)
{
    Zob *a, *b;
    ZError err;

    err = znewbool((ZBool **) ret);
    if (err != ZE_OK)
        return err;
    a = args->first->object;
    b = args->first->next->object;
    ((ZBool *) *ret)->value = 1;
    switch (*a) {
        case T_BYTE:
            switch (*b) {
                case T_BYTE:
                    ((ZBool *) *ret)->value = ((ZByte *) a)->value <
                                              ((ZByte *) b)->value;
                    break;
                case T_WORD:
                    ((ZBool *) *ret)->value = ((ZByte *) a)->value <
                                              ((ZInt *) b)->value;
                    break;
            }
            break;
        case T_WORD:
            switch (*b) {
                case T_BYTE:
                    ((ZBool *) *ret)->value = ((ZInt *) a)->value <
                                              ((ZByte *) b)->value;
                    break;
                case T_WORD:
                    ((ZBool *) *ret)->value = ((ZInt *) a)->value <
                                              ((ZInt *) b)->value;
                    break;
            }
    }
    return ZE_OK;
}

/* Register 'cfunc' in 'namespace'.
 * If there is not enough memory, return ZE_OUT_OF_MEMORY.
 * Otherwise, return ZE_OK.
 */
ZError
regfunc(ZDict *namespace,
        ZError (*func)(ZList *args, Zob **ret),
        char *name,
        unsigned char arity)
{
    ZFunc *zfunc;
    ZLowFunc *zlowfunc;
    ZByteArray *key;
    ZError err;

    err = znewlowfunc(&zlowfunc);
    if (err != ZE_OK)
        return err;
    zlowfunc->func = func;
    err = znewfunc(&zfunc, (FImp *) zlowfunc, arity);
    if (err != ZE_OK) {
        zdellowfunc(&zlowfunc);
        return err;
    }
    err = zyarrfromstr(&key, name);
    if (err != ZE_OK) {
        zdellowfunc(&zlowfunc);
        zdelfunc(&zfunc);
        return err;
    }
    err = zdset(namespace, (Zob *) key, (Zob *) zfunc);
    if (err != ZE_OK) {
        zdellowfunc(&zlowfunc);
        zdelfunc(&zfunc);
        zdelyarr(&key);
        return err;
    }
    return ZE_OK;
}

/* Create a new ZDict with builtins names in 'builtins'.
 * If there is not enough memory, return ZE_OUT_OF_MEMORY.
 * Otherwise, return ZE_OK.
 */
ZError
zbuild(ZDict **builtins)
{
    ZError err;

    err = znewdict(builtins);
    if (err != ZE_OK)
        return err;

    err = regfunc(*builtins, z_tname, "tname", 1);
    if (err != ZE_OK)
        return err;
    err = regfunc(*builtins, z_refc, "refc", 1);
    if (err != ZE_OK)
        return err;
    err = regfunc(*builtins, z_print, "print", 1);
    if (err != ZE_OK)
        return err;
    err = regfunc(*builtins, z_repr, "repr", 1);
    if (err != ZE_OK)
        return err;
    err = regfunc(*builtins, z_len, "len", 1);
    if (err != ZE_OK)
        return err;
    err = regfunc(*builtins, z_arr, "arr", 1);
    if (err != ZE_OK)
        return err;
    err = regfunc(*builtins, z_concat, "concat", 2);
    if (err != ZE_OK)
        return err;
    err = regfunc(*builtins, z_get, "get", 3);
    if (err != ZE_OK)
        return err;
    err = regfunc(*builtins, z_sum, "+", 2);
    if (err != ZE_OK)
        return err;
    err = regfunc(*builtins, z_eq, "==", 2);
    if (err != ZE_OK)
        return err;
    err = regfunc(*builtins, z_lt, "<", 2);
    if (err != ZE_OK)
        return err;
    return ZE_OK;
}
