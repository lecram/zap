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
#include "zint.h"
#include "zbytearray.h"
#include "zbignum.h"
#include "zlist.h"
#include "zdict.h"
#include "zfunc.h"

#include "zobject.h"

#include "zbuiltin.h"

/* $(o) */
ZError
z_copy(ZList *args, Zob **ret)
{
    return zcpyobj(args->first->object, ret);
}

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
            ((ZInt *) *ret)->value = (int) zalength((ZByteArray *) obj);
            break;
        case T_LIST:
            ((ZInt *) *ret)->value = (int) zllength((ZList *) obj);
            break;
        case T_DICT:
            ((ZInt *) *ret)->value = (int) zdlength((ZDict *) obj);
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

/* push(list, item) */
ZError
z_push(ZList *args, Zob **ret)
{
    Zob *zlist, *item;

    zlist = args->first->object;
    if (*zlist != T_LIST)
        return ZE_INVALID_ARGUMENT;
    item = args->first->next->object;
    *ret = zlist;
    return zlpush((ZList *) zlist, item);
}

/* peek(list, item) */
ZError
z_peek(ZList *args, Zob **ret)
{
    Zob *zlist;

    zlist = args->first->object;
    if (*zlist != T_LIST)
        return ZE_INVALID_ARGUMENT;
    *ret = zlpeek((ZList *) zlist);
    if (**ret == EMPTY)
        return ZE_INDEX_OUT_OF_RANGE;
    else
        return ZE_OK;
}

/* pop(list, item) */
ZError
z_pop(ZList *args, Zob **ret)
{
    Zob *zlist;

    zlist = args->first->object;
    if (*zlist != T_LIST)
        return ZE_INVALID_ARGUMENT;
    *ret = zlpop((ZList *) zlist);
    if (**ret == EMPTY)
        return ZE_INDEX_OUT_OF_RANGE;
    else
        return ZE_OK;
}

/* append(list, item) */
ZError
z_append(ZList *args, Zob **ret)
{
    Zob *zlist, *item;

    zlist = args->first->object;
    if (*zlist != T_LIST)
        return ZE_INVALID_ARGUMENT;
    item = args->first->next->object;
    *ret = zlist;
    return zlappend((ZList *) zlist, item);
}

/* set(list, index, item) */
/* set(dict, key, value) */
ZError
z_set(ZList *args, Zob **ret)
{
    *ret = args->first->object;
    switch (**ret) {
        case T_LIST:
            {
                ZList *zlist;
                ZInt *index;
                Zob *item;

                zlist = (ZList *) args->first->object;
                index = (ZInt *) args->first->next->object;
                item = args->first->next->next->object;
                return zlset(zlist, index->value, item);
            }
        case T_DICT:
            {
                ZDict *zdict;
                Zob *key, *value;

                zdict = (ZDict *) args->first->object;
                key = args->first->next->object;
                value = args->first->next->next->object;
                return zdset(zdict, key, value);
            }
        default:
            return ZE_INVALID_ARGUMENT;
    }
}

/* get(d, k, dv) */
ZError
z_get(ZList *args, Zob **ret)
{
    Zob *zdict, *key, *defval;
    ZNode *arg = args->first;
    ZError err;

    zdict = arg->object;
    key = arg->next->object;
    defval = arg->next->next->object;
    err = zcpyobj(defval, ret);
    if (err != ZE_OK)
        return err;
    (void) zdget((ZDict *) zdict, key, ret);
    return ZE_OK;
}

/* +(a, b) */
ZError
z_sum(ZList *args, Zob **ret)
{
    Zob *a, *b;
    ZError err;

    a = args->first->object;
    b = args->first->next->object;
    if (*a != *b)
        return ZE_INVALID_ARGUMENT;
    switch (*a) {
        case T_BYTE:
            err = znewbyte((ZByte **) ret);
            if (err != ZE_OK)
                return err;
            ((ZByte *) *ret)->value = ((ZByte *) a)->value +
                                      ((ZByte *) b)->value;
            return ZE_OK;
        case T_INT:
            err = znewint((ZInt **) ret);
            if (err != ZE_OK)
                return err;
            ((ZInt *) *ret)->value = ((ZInt *) a)->value +
                                     ((ZInt *) b)->value;
            return ZE_OK;
    }
    return ZE_OK;
}

/* -(a, b) */
ZError
z_sub(ZList *args, Zob **ret)
{
    Zob *a, *b;
    ZError err;

    a = args->first->object;
    b = args->first->next->object;
    if (*a != *b)
        return ZE_INVALID_ARGUMENT;
    switch (*a) {
        case T_BYTE:
            err = znewbyte((ZByte **) ret);
            if (err != ZE_OK)
                return err;
            ((ZByte *) *ret)->value = ((ZByte *) a)->value -
                                      ((ZByte *) b)->value;
            return ZE_OK;
        case T_INT:
            err = znewint((ZInt **) ret);
            if (err != ZE_OK)
                return err;
            ((ZInt *) *ret)->value = ((ZInt *) a)->value -
                                     ((ZInt *) b)->value;
            return ZE_OK;
    }
    return ZE_OK;
}

/* *(a, b) */
ZError
z_mul(ZList *args, Zob **ret)
{
    Zob *a, *b;
    ZError err;

    a = args->first->object;
    b = args->first->next->object;
    if (*a != *b)
        return ZE_INVALID_ARGUMENT;
    switch (*a) {
        case T_BYTE:
            err = znewbyte((ZByte **) ret);
            if (err != ZE_OK)
                return err;
            ((ZByte *) *ret)->value = ((ZByte *) a)->value *
                                      ((ZByte *) b)->value;
            return ZE_OK;
        case T_INT:
            err = znewint((ZInt **) ret);
            if (err != ZE_OK)
                return err;
            ((ZInt *) *ret)->value = ((ZInt *) a)->value *
                                     ((ZInt *) b)->value;
            return ZE_OK;
    }
    return ZE_OK;
}

/* /(a, b) */
ZError
z_div(ZList *args, Zob **ret)
{
    Zob *a, *b;
    ZError err;

    a = args->first->object;
    b = args->first->next->object;
    if (*a != *b)
        return ZE_INVALID_ARGUMENT;
    switch (*a) {
        case T_BYTE:
            err = znewbyte((ZByte **) ret);
            if (err != ZE_OK)
                return err;
            if (((ZByte *) b)->value == 0)
                return ZE_DIVISION_BY_ZERO;
            ((ZByte *) *ret)->value = ((ZByte *) a)->value /
                                      ((ZByte *) b)->value;
            return ZE_OK;
        case T_INT:
            err = znewint((ZInt **) ret);
            if (err != ZE_OK)
                return err;
            if (((ZInt *) b)->value == 0)
                return ZE_DIVISION_BY_ZERO;
            ((ZInt *) *ret)->value = ((ZInt *) a)->value /
                                     ((ZInt *) b)->value;
            return ZE_OK;
    }
    return ZE_OK;
}

/* %(a, b) */
ZError
z_mod(ZList *args, Zob **ret)
{
    Zob *a, *b;
    ZError err;

    a = args->first->object;
    b = args->first->next->object;
    if (*a != *b)
        return ZE_INVALID_ARGUMENT;
    switch (*a) {
        case T_BYTE:
            err = znewbyte((ZByte **) ret);
            if (err != ZE_OK)
                return err;
            if (((ZByte *) b)->value == 0)
                return ZE_DIVISION_BY_ZERO;
            ((ZByte *) *ret)->value = ((ZByte *) a)->value %
                                      ((ZByte *) b)->value;
            return ZE_OK;
        case T_INT:
            err = znewint((ZInt **) ret);
            if (err != ZE_OK)
                return err;
            if (((ZInt *) b)->value == 0)
                return ZE_DIVISION_BY_ZERO;
            ((ZInt *) *ret)->value = ((ZInt *) a)->value %
                                     ((ZInt *) b)->value;
            return ZE_OK;
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

/* !=(a, b) */
ZError
z_neq(ZList *args, Zob **ret)
{
    Zob *a, *b;
    ZError err;

    err = znewbool((ZBool **) ret);
    if (err != ZE_OK)
        return err;
    a = args->first->object;
    b = args->first->next->object;
    ((ZBool *) *ret)->value = zcmpobj(a, b);
    return ZE_OK;
}

/* <(a, b) */
ZError
z_lt(ZList *args, Zob **ret)
{
    Zob *a, *b;
    ZError err;

    a = args->first->object;
    b = args->first->next->object;
    if (*a != *b)
        return ZE_INVALID_ARGUMENT;
    err = znewbool((ZBool **) ret);
    if (err != ZE_OK)
        return err;
    ((ZBool *) *ret)->value = 1;
    switch (*a) {
        case T_BYTE:
            ((ZBool *) *ret)->value = ((ZByte *) a)->value <
                                      ((ZByte *) b)->value;
            break;
        case T_INT:
            ((ZBool *) *ret)->value = ((ZInt *) a)->value <
                                      ((ZInt *) b)->value;
            break;
    }
    return ZE_OK;
}

/* >(a, b) */
ZError
z_gt(ZList *args, Zob **ret)
{
    Zob *a, *b;
    ZError err;

    a = args->first->object;
    b = args->first->next->object;
    if (*a != *b)
        return ZE_INVALID_ARGUMENT;
    err = znewbool((ZBool **) ret);
    if (err != ZE_OK)
        return err;
    ((ZBool *) *ret)->value = 1;
    switch (*a) {
        case T_BYTE:
            ((ZBool *) *ret)->value = ((ZByte *) a)->value >
                                      ((ZByte *) b)->value;
            break;
        case T_INT:
            ((ZBool *) *ret)->value = ((ZInt *) a)->value >
                                      ((ZInt *) b)->value;
            break;
    }
    return ZE_OK;
}

/* <=(a, b) */
ZError
z_leq(ZList *args, Zob **ret)
{
    Zob *a, *b;
    ZError err;

    a = args->first->object;
    b = args->first->next->object;
    if (*a != *b)
        return ZE_INVALID_ARGUMENT;
    err = znewbool((ZBool **) ret);
    if (err != ZE_OK)
        return err;
    ((ZBool *) *ret)->value = 1;
    switch (*a) {
        case T_BYTE:
            ((ZBool *) *ret)->value = ((ZByte *) a)->value <=
                                      ((ZByte *) b)->value;
            break;
        case T_INT:
            ((ZBool *) *ret)->value = ((ZInt *) a)->value <=
                                      ((ZInt *) b)->value;
            break;
    }
    return ZE_OK;
}

/* >=(a, b) */
ZError
z_geq(ZList *args, Zob **ret)
{
    Zob *a, *b;
    ZError err;

    a = args->first->object;
    b = args->first->next->object;
    if (*a != *b)
        return ZE_INVALID_ARGUMENT;
    err = znewbool((ZBool **) ret);
    if (err != ZE_OK)
        return err;
    ((ZBool *) *ret)->value = 1;
    switch (*a) {
        case T_BYTE:
            ((ZBool *) *ret)->value = ((ZByte *) a)->value >=
                                      ((ZByte *) b)->value;
            break;
        case T_INT:
            ((ZBool *) *ret)->value = ((ZInt *) a)->value >=
                                      ((ZInt *) b)->value;
            break;
    }
    return ZE_OK;
}

/* Register 'func' in 'dict'.
 * If there is not enough memory, return ZE_OUT_OF_MEMORY.
 * Otherwise, return ZE_OK.
 */
ZError
regfunc(ZDict *dict,
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
    err = zdset(dict, (Zob *) key, (Zob *) zfunc);
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

    err = regfunc(*builtins, z_copy, "$", 1);
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
    err = regfunc(*builtins, z_push, "push", 2);
    if (err != ZE_OK)
        return err;
    err = regfunc(*builtins, z_peek, "peek", 1);
    if (err != ZE_OK)
        return err;
    err = regfunc(*builtins, z_pop, "pop", 1);
    if (err != ZE_OK)
        return err;
    err = regfunc(*builtins, z_append, "append", 2);
    if (err != ZE_OK)
        return err;
    err = regfunc(*builtins, z_set, "set", 3);
    if (err != ZE_OK)
        return err;
    err = regfunc(*builtins, z_get, "get", 3);
    if (err != ZE_OK)
        return err;
    err = regfunc(*builtins, z_sum, "+", 2);
    if (err != ZE_OK)
        return err;
    err = regfunc(*builtins, z_sub, "-", 2);
    if (err != ZE_OK)
        return err;
    err = regfunc(*builtins, z_mul, "*", 2);
    if (err != ZE_OK)
        return err;
    err = regfunc(*builtins, z_div, "/", 2);
    if (err != ZE_OK)
        return err;
    err = regfunc(*builtins, z_mod, "%", 2);
    if (err != ZE_OK)
        return err;
    err = regfunc(*builtins, z_eq, "==", 2);
    if (err != ZE_OK)
        return err;
    err = regfunc(*builtins, z_neq, "!=", 2);
    if (err != ZE_OK)
        return err;
    err = regfunc(*builtins, z_lt, "<", 2);
    if (err != ZE_OK)
        return err;
    err = regfunc(*builtins, z_gt, ">", 2);
    if (err != ZE_OK)
        return err;
    err = regfunc(*builtins, z_leq, "<=", 2);
    if (err != ZE_OK)
        return err;
    err = regfunc(*builtins, z_geq, ">=", 2);
    if (err != ZE_OK)
        return err;
    return ZE_OK;
}
