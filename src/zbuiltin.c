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

    if (*args->first->object != T_YARR)
        return ZE_INVALID_ARGUMENT;
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
            return ZE_INVALID_ARGUMENT;
    }
    return ZE_OK;
}

/* arr(i) */
ZError
z_arr(ZList *args, Zob **ret)
{
    Zob *i;
    ZError err;

    if (*args->first->object != T_BYTE)
        return ZE_INVALID_ARGUMENT;
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

    if (*args->first->object != T_YARR ||
        *args->first->next->object != T_YARR)
        return ZE_INVALID_ARGUMENT;
    *ret = args->first->object;
    s2 = args->first->next->object;
    err = zconcat((ZByteArray *) *ret, (ZByteArray *) s2);
    return err;
}

/* join([s1, s2, ..., sn], sep) */
ZError
z_join(ZList *args, Zob **ret)
{
    ZNode *sub;
    ZByteArray *sep;
    ZError err;

    if (*args->first->object != T_LIST ||
        *args->first->next->object != T_YARR)
        return ZE_INVALID_ARGUMENT;
    sub = ((ZList *) args->first->object)->first;
    sep = (ZByteArray *) args->first->next->object;
    err = zcpyobj(sub->object, ret);
    if (err != ZE_OK)
        return err;
    sub = sub->next;
    while (sub != NULL) {
        err = zconcat((ZByteArray *) *ret, sep);
        if (err != ZE_OK)
            return err;
        err = zconcat((ZByteArray *) *ret, (ZByteArray *) sub->object);
        if (err != ZE_OK)
            return err;
        sub = sub->next;
    }
    return ZE_OK;
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
ZError
z_set(ZList *args, Zob **ret)
{
    ZList *zlist;
    Zob *index, *item;

    *ret = args->first->object;
    if (**ret != T_LIST)
        return ZE_INVALID_ARGUMENT;
    zlist = (ZList *) *ret;
    index = args->first->next->object;
    if (*index != T_INT)
        return ZE_INVALID_ARGUMENT;
    item = args->first->next->next->object;
    return zlset(zlist, ((ZInt *) index)->value, item);
}

/* get(list, index) */
ZError
z_get(ZList *args, Zob **ret)
{
    Zob *zlist, *index;

    zlist = args->first->object;
    if (*zlist != T_LIST)
        return ZE_INVALID_ARGUMENT;
    index = args->first->next->object;
    if (*index != T_INT)
        return ZE_INVALID_ARGUMENT;
    return zlget((ZList *) zlist, ((ZInt *) index)->value, ret);
}

/* ins(list, index, item) */
ZError
z_ins(ZList *args, Zob **ret)
{
    ZList *zlist;
    Zob *index, *item;

    *ret = args->first->object;
    if (**ret != T_LIST)
        return ZE_INVALID_ARGUMENT;
    zlist = (ZList *) *ret;
    index = args->first->next->object;
    if (*index != T_INT)
        return ZE_INVALID_ARGUMENT;
    item = args->first->next->next->object;
    return zlinsert(zlist, ((ZInt *) index)->value, item);
}

/* ext(lista, listb) */
ZError
z_ext(ZList *args, Zob **ret)
{
    Zob *zlista, *zlistb;

    zlista = args->first->object;
    if (*zlista != T_LIST)
        return ZE_INVALID_ARGUMENT;
    zlistb = args->first->next->object;
    if (*zlistb != T_LIST)
        return ZE_INVALID_ARGUMENT;
    *ret = zlista;
    return zlextend((ZList *) zlista, (ZList *) zlistb);
}

/* rem(list, index) */
ZError
z_rem(ZList *args, Zob **ret)
{
    ZList *zlist;
    Zob *index;

    *ret = args->first->object;
    if (**ret != T_LIST)
        return ZE_INVALID_ARGUMENT;
    zlist = (ZList *) *ret;
    index = args->first->next->object;
    if (*index != T_INT)
        return ZE_INVALID_ARGUMENT;
    return zlremove(zlist, ((ZInt *) index)->value);
}

/* has(list, item) */
ZError
z_has(ZList *args, Zob **ret)
{
    Zob *zlist, *item;
    ZError err;

    err = znewbool((ZBool **) ret);
    if (err != ZE_OK)
        return err;
    if (*args->first->object != T_LIST)
        return ZE_INVALID_ARGUMENT;
    zlist = args->first->object;
    item = args->first->next->object;
    ((ZBool *) *ret)->value = zlhasitem((ZList *) zlist, item);
    return ZE_OK;
}

/* setkey(dict, key, value) */
ZError
z_setkey(ZList *args, Zob **ret)
{
    ZDict *zdict;
    Zob *key, *value;

    *ret = args->first->object;
    if (**ret != T_DICT)
        return ZE_INVALID_ARGUMENT;
    zdict = (ZDict *) *ret;
    key = args->first->next->object;
    value = args->first->next->next->object;
    return zdset(zdict, key, value);
}

/* getkey(dict, key, defval) */
ZError
z_getkey(ZList *args, Zob **ret)
{
    Zob *zdict, *key, *defval;
    ZError err;

    zdict = args->first->object;
    if (*zdict != T_DICT)
        return ZE_INVALID_ARGUMENT;
    key = args->first->next->object;
    defval = args->first->next->next->object;
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
    return ZE_INVALID_ARGUMENT;
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
    return ZE_INVALID_ARGUMENT;
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
    return ZE_INVALID_ARGUMENT;
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
    return ZE_INVALID_ARGUMENT;
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
    return ZE_INVALID_ARGUMENT;
}

/* <<(a, b) */
ZError
z_lshift(ZList *args, Zob **ret)
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
            ((ZByte *) *ret)->value = ((ZByte *) a)->value <<
                                      ((ZByte *) b)->value;
            return ZE_OK;
        case T_INT:
            err = znewint((ZInt **) ret);
            if (err != ZE_OK)
                return err;
            ((ZInt *) *ret)->value = ((ZInt *) a)->value <<
                                     ((ZInt *) b)->value;
            return ZE_OK;
    }
    return ZE_INVALID_ARGUMENT;
}

/* >>(a, b) */
ZError
z_rshift(ZList *args, Zob **ret)
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
            ((ZByte *) *ret)->value = ((ZByte *) a)->value >>
                                      ((ZByte *) b)->value;
            return ZE_OK;
        case T_INT:
            err = znewint((ZInt **) ret);
            if (err != ZE_OK)
                return err;
            ((ZInt *) *ret)->value = ((ZInt *) a)->value >>
                                     ((ZInt *) b)->value;
            return ZE_OK;
    }
    return ZE_INVALID_ARGUMENT;
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
            return ZE_OK;
        case T_INT:
            ((ZBool *) *ret)->value = ((ZInt *) a)->value <
                                      ((ZInt *) b)->value;
            return ZE_OK;
    }
    return ZE_INVALID_ARGUMENT;
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
            return ZE_OK;
        case T_INT:
            ((ZBool *) *ret)->value = ((ZInt *) a)->value >
                                      ((ZInt *) b)->value;
            return ZE_OK;
    }
    return ZE_INVALID_ARGUMENT;
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
            return ZE_OK;
        case T_INT:
            ((ZBool *) *ret)->value = ((ZInt *) a)->value <=
                                      ((ZInt *) b)->value;
            return ZE_OK;
    }
    return ZE_INVALID_ARGUMENT;
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
            return ZE_OK;
        case T_INT:
            ((ZBool *) *ret)->value = ((ZInt *) a)->value >=
                                      ((ZInt *) b)->value;
            return ZE_OK;
    }
    return ZE_INVALID_ARGUMENT;
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
    struct wrap {
        ZError (*func)(ZList *args, Zob **ret);
        char *name;
        unsigned char arity;
    } wraps[] = {
      {z_copy, "$", 1},
      {z_tname, "tname", 1},
      {z_refc, "refc", 1},
      {z_print, "print", 1},
      {z_repr, "repr", 1},
      {z_len, "len", 1},
      {z_arr, "arr", 1},
      {z_concat, "concat", 2},
      {z_join, "join", 2},
      {z_push, "push", 2},
      {z_peek, "peek", 1},
      {z_pop, "pop", 1},
      {z_append, "append", 2},
      {z_set, "set", 3},
      {z_get, "get", 2},
      {z_ins, "ins", 3},
      {z_ext, "ext", 2},
      {z_rem, "rem", 2},
      {z_has, "has", 2},
      {z_setkey, "setkey", 3},
      {z_getkey, "getkey", 3},
      {z_sum, "+", 2},
      {z_sub, "-", 2},
      {z_mul, "*", 2},
      {z_div, "/", 2},
      {z_mod, "%", 2},
      {z_lshift, "<<", 2},
      {z_rshift, ">>", 2},
      {z_eq, "==", 2},
      {z_neq, "!=", 2},
      {z_lt, "<", 2},
      {z_gt, ">", 2},
      {z_leq, "<=", 2},
      {z_geq, ">=", 2},
      {NULL, "", 0}
    };
    int i;
    ZError err;

    err = znewdict(builtins);
    if (err != ZE_OK)
        return err;
    for (i = 0; wraps[i].func != NULL; i++) {
        err = regfunc(*builtins,
                      wraps[i].func,
                      wraps[i].name,
                      wraps[i].arity);
        if (err != ZE_OK)
            return err;
    }
    return ZE_OK;
}
