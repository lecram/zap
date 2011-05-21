/* Copyright 2010-2011 by Marcel Rodrigues <marcelgmr@gmail.com>
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

/* Runtime */

/* In This File:
 * - ZContext handle functions.
 * - Bytecode expression evaluation.
 * - Bytecode execution.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
#include "znametable.h"
#include "zdict.h"
#include "zfunc.h"

#include "zobject.h"

#include "zruntime.h"

/* Create a new ZContext in 'zcontext'.
 * If there is not enough memory, return ZE_OUT_OF_MEMORY.
 * Otherwise, return ZE_OK.
 */
ZError
znewcontext(ZContext **zcontext)
{
    *zcontext = (ZContext *) malloc(sizeof(ZContext));
    if (*zcontext == NULL)
        return ZE_OUT_OF_MEMORY;
    return ZE_OK;
}

/* Remove 'zcontext' from memory. */
void
zdelcontext(ZContext **zcontext)
{
    zdelnable(&(*zcontext)->global);
    zdellist(&(*zcontext)->local);
    free(*zcontext);
    *zcontext = NULL;
}

/* Push a new namespace to 'zcontext'.
 * If there is not enough memory, return ZE_OUT_OF_MEMORY.
 * Otherwise, return ZE_OK.
 */
ZError
zpushlocal(ZContext *zcontext)
{
    ZNameTable *znable;
    ZError err;

    err = znewnable(&znable);
    if (err != ZE_OK)
        return err;
    return zlpush(zcontext->local, (Zob *) znable);
}

/* Pop a namespace from 'zcontext' and save its "_ret_" value in 'ret'.
 * If there is not enough memory, return ZE_OUT_OF_MEMORY.
 * Otherwise, return ZE_OK.
 */
ZError
zpoplocal(ZContext *zcontext, Zob **ret)
{
    ZNameTable *poped;
    ZError err;

    poped = (ZNameTable *) zlpop(zcontext->local);
    if (ztget(poped, "_ret_", ret) == 0) {
        ZNone *znone;

        err = znewnone(&znone);
        if (err != ZE_OK)
            return err;
        *ret = (Zob *) znone;
    }
    zdelnable(&poped);
    return ZE_OK;
}

/* Define or redefine 'name' in 'zcontext'.
 * If 'name' contains a non-ZNameTable object followed by a dot,
 *  return ZE_NOT_A_NODE.
 * If there is not enough memory, return ZE_OUT_OF_MEMORY.
 * Otherwise, return ZE_OK.
 */
ZError
zsetincontext(ZContext *zcontext, char *name, Zob *value)
{
    ZNameTable *nable, **pnable;
    char *oldtoken, *newtoken;
    char *lastname = NULL;
    int nulls = 0;

    if (zcontext->local->length > 0)
        nable = (ZNameTable *) zlpeek(zcontext->local);
    else
        nable = zcontext->global;
    pnable = &nable;
    if (strchr(name, '.') != NULL) {
        oldtoken = strtok(name, ".");
        while (lastname == NULL) {
            if ((newtoken = strtok(NULL, ".")) == NULL)
                lastname = oldtoken;
            else {
                if (ztget(nable, oldtoken, (Zob **) pnable) == 0)
                    return ZE_NAME_NOT_DEFINED;
                if (nable->type != T_NMTB)
                    return ZE_NOT_A_NODE;
                nulls++;
                oldtoken = newtoken;
            }
        }
        /* Replace null characters -- placed by strtok() calls -- 
         *  by the original dots.
         */
        for (; nulls > 0; nulls--)
            name[strlen(name)] = '.';
    }
    else
        lastname = name;
    return ztset(nable, lastname, value);
}

/* If 'name' is in 'zcontext':
 *  * copy its value to 'value';
 *  * set 'self' as the last node (ZNameTable) visited;
 *  * return nonzero.
 * Otherwise:
 *  * set 'self' as the last node (ZNameTable) visited;
 *  * return zero.
 */
int
zgetincontext(ZContext *zcontext,
              char *name,
              ZNameTable **self,
              Zob **pvalue)
{
    Zob *value = *pvalue;
    ZNameTable *nable, **pnable, *local = NULL;
    char *oldtoken, *newtoken;
    char *lastname = NULL;
    int nulls = 0;
    int ok = 0;
    int head = 1;

    if (zcontext->local->length > 0)
        local = (ZNameTable *) zlpeek(zcontext->local);
    else
        head = 0;
    nable = zcontext->global;
    pnable = &nable;
    oldtoken = strtok(name, ".");
    while (lastname == NULL) {
        if ((newtoken = strtok(NULL, ".")) == NULL)
            lastname = oldtoken;
        else {
            if (head) {
                /* The first name should be searched in locals and globals. */
                if (ztget(local, oldtoken, (Zob **) pnable) == 0)
                    if (ztget(nable, oldtoken, (Zob **) pnable) == 0)
                        return 0;
                head = 0;
            }
            else {
                if (ztget(nable, oldtoken, (Zob **) pnable) == 0)
                    return 0;
            }
            if (nable->type != T_NMTB)
                return 0;
            nulls++;
            oldtoken = newtoken;
        }
    }
    *self = nable;
    if (head) {
        /* The first name should be searched in locals and globals. */
        if (ztget(local, lastname, (Zob **) &value) == 0) {
            if (ztget(nable, lastname, (Zob **) &value) == 0)
                return 0;
        }
        else
            *self = local;
        ok = 1;
    }
    else
        ok = ztget(nable, lastname, &value);
    *pvalue = value;
    /* Replace null characters -- placed by strtok() calls -- 
     *  by the original dots.
     */
    for (; nulls > 0; nulls--)
        name[strlen(name)] = '.';
    return ok;
}

/* If 'name' is in 'zcontext',
 *  remove its pair from 'zcontext' and return nonzero.
 * Otherwise, return zero.
 */
int
zremincontext(ZContext *zcontext, char *name)
{
    if (zcontext->local->length > 0) {
        ZNameTable *local;

        local = (ZNameTable *) zlpeek(zcontext->local);
        if (ztremove(local, name) != 0)
            return 1;
    }
    return ztremove(zcontext->global, name);
}

/* If 'name' is in 'zcontext', return nonzero.
 * Otherwise, return zero.
 */
int
zhasincontext(ZContext *zcontext, char *name)
{
    if (zcontext->local->length > 0) {
        ZNameTable *local;

        local = (ZNameTable *) zlpeek(zcontext->local);
        if (zthasname(local, name))
            return 1;
    }
    if (zthasname(zcontext->global, name))
        return 1;
    return 0;
}

unsigned int
zreadword(char **entry)
{
    size_t i;
    unsigned int zint;
    unsigned char *cursor;

    cursor = (unsigned char *) *entry;
    zint = (unsigned int) *cursor;
    cursor++;
    for (i = 1; i < WL / 8; i++) {
        zint <<= 8;
        zint += (unsigned int) *cursor;
        cursor++;
    }
    *entry = (char *) cursor;
    return zint;
}

unsigned int
zread_uvlv(char **entry)
{
    unsigned int n;
    signed char *cursor = (signed char *) *entry;

    n = (unsigned int) (*cursor & 127);
    while (*cursor < 0) {
        n <<= 7;
        cursor++;
        n += (unsigned int) (*cursor & 127);
    }
    cursor++;
    *entry = (char *) cursor;
    return n;
}

int
zread_svlv(char **entry)
{
    int n, s;
    signed char *cursor = (signed char *) *entry;

    s = (*cursor < 0) ? -1 : 1;
    cursor++;
    n = (int) (*cursor & 127);
    while (*cursor < 0) {
        n <<= 7;
        cursor++;
        n += (int) (*cursor & 127);
    }
    cursor++;
    *entry = (char *) cursor;
    return n * s;
}

void
zskip_uvlv(char **entry)
{
    char *cursor = *entry;

    while (*cursor < 0)
        cursor++;
    cursor++;
    *entry = cursor;
}

void
zskip_svlv(char **entry)
{
    char *cursor = *entry;

    cursor++; /* Sign. */
    while (*cursor < 0)
        cursor++;
    cursor++;
    *entry = cursor;
}

void
zskip_expr(char **entry)
{
    char *cursor = *entry;

    switch (*cursor) {
        case T_NONE:
            cursor++;
            break;
        case T_BOOL:
            cursor += 2;
            break;
        case T_BYTE:
            cursor += 2;
            break;
        case T_INT:
            cursor++;
            zskip_svlv(&cursor);
            break;
        case T_YARR:
            {
                unsigned int length;

                cursor++;
                length = zreadword(&cursor);
                cursor += length;
            }
            break;
        case T_BNUM:
            {
                unsigned int length;

                cursor++;
                length = zreadword(&cursor);
                cursor += length * WL / 8;
            }
            break;
        case T_LIST:
            cursor++;
            while (*cursor != '\0')
                zskip_expr(&cursor);
            cursor++; /* Skip LIST_END. */
            break;
        case T_DICT:
            cursor++;
            while (*cursor != '\0') {
                zskip_expr(&cursor);  /* Skip key. */
                zskip_expr(&cursor);  /* Skip value. */
            }
            cursor++; /* Skip DICT_END. */
            break;
        case CALLSTART:
            /* Function Call. */
            cursor++;
            cursor += strlen(cursor) + 1; /* Skip STRING_END. */
            while (*cursor != CALLEND)
                zskip_expr(&cursor);
            cursor++; /* Skip CALL_END. */
            break;
        default:
            /* Name. */
            cursor += strlen(cursor) + 1; /* Skip STRING_END. */
    }
    *entry = cursor;
}

/* Evaluate bytecode expression pointed by 'entry', in 'zcontext'.
 * Upon success, save the result on 'zob'.
 * If there is not enough memory, return ZE_OUT_OF_MEMORY.
 * Otherwise, return ZE_OK.
 */
ZError
zeval(ZContext *zcontext, ZList *tmp, char **entry, Zob **pzob)
{
    char *cursor = *entry;
    Zob *zob = *pzob;
    ZError err;

    switch (*cursor) {
        case T_NONE:
            {
                ZNone *znone;

                err = znewnone(&znone);
                if (err != ZE_OK)
                    return err;
                cursor++;
                zob = (Zob *) znone;
            }
            break;
        case T_BOOL:
            {
                ZBool *zbool;

                err = znewbool(&zbool);
                if (err != ZE_OK)
                    return err;
                cursor++;
                zbool->value = (int) *cursor;
                cursor++;
                zob = (Zob *) zbool;
            }
            break;
        case T_BYTE:
            {
                ZByte *zbyte;

                err = znewbyte(&zbyte);
                if (err != ZE_OK)
                    return err;
                cursor++;
                zbyte->value = (unsigned char) *cursor;
                cursor++;
                zob = (Zob *) zbyte;
            }
            break;
        case T_INT:
            {
                ZInt *zint;

                err = znewint(&zint);
                if (err != ZE_OK)
                    return err;
                cursor++;
                zint->value = zread_svlv(&cursor);
                zob = (Zob *) zint;
            }
            break;
        case T_YARR:
            {
                ZByteArray *zbytearray;
                unsigned int length, index;

                cursor++;
                length = zreadword(&cursor);
                err = znewyarr(&zbytearray, length);
                if (err != ZE_OK)
                    return err;
                for (index = 0; index < length; index++) {
                    zbytearray->bytes[index] = (unsigned char) *cursor;
                    cursor++;
                }
                zob = (Zob *) zbytearray;
            }
            break;
        case T_BNUM:
            {
                ZBigNum *zbignum;
                unsigned int wordlen, index;

                cursor++;
                wordlen = zreadword(&cursor);
                err = znewbnum(&zbignum, (unsigned int) (wordlen * WL));
                if (err != ZE_OK)
                    return err;
                for (index = 0; index < wordlen; index++)
                    zbignum->words[index] = zreadword(&cursor);
                zob = (Zob *) zbignum;
            }
            break;
        case T_LIST:
            {
                ZList *zlist;

                err = znewlist(&zlist);
                if (err != ZE_OK)
                    return err;
                cursor++;
                while (*cursor != '\0') {
                    Zob *item;

                    err = zeval(zcontext, tmp, &cursor, &item);
                    if (err != ZE_OK)
                        return err;
                    err = zlappend(zlist, item);
                    if (err != ZE_OK)
                        return err;
                }
                cursor++; /* Skip LIST_END. */
                zob = (Zob *) zlist;
            }
            break;
        case T_DICT:
            {
                /* FIXME: should prevent memory leak in duplicated keys. */
                ZDict *zdict;
                Zob *key, *value;
                ZError err;

                err = znewdict(&zdict);
                if (err != ZE_OK)
                    return err;
                cursor++;
                while (*cursor != '\0') {
                    err = zeval(zcontext, tmp, &cursor, &key);
                    if (err != ZE_OK)
                        return err;
                    err = zeval(zcontext, tmp, &cursor, &value);
                    if (err != ZE_OK)
                        return err;
                    err = zdset(zdict, key, value);
                    if (err != ZE_OK)
                        return err;
                }
                cursor++; /* Skip DICT_END. */
                zob = (Zob *) zdict;
            }
            break;
        case CALLSTART:
            /* Function Call. */
            cursor++;
            err = zfeval(zcontext, tmp, &cursor, &zob);
            if (err != ZE_OK)
                return err;
            cursor++; /* Skip CALL_END. */
            break;
        default:
            /* Name. */
            err = znameval(zcontext, &cursor, &zob);
            if (err != ZE_OK)
                return err;
    }
    *entry = cursor;
    *pzob = zob;
    return zlappend(tmp, *pzob);
}

ZError
znameval(ZContext *zcontext, char **entry, Zob **pzob)
{
    char *cursor = *entry;
    Zob *zob = *pzob;
    ZNameTable *self;

    if (zgetincontext(zcontext, cursor, &self, &zob) == 0)
        return ZE_NAME_NOT_DEFINED;
    cursor += strlen(cursor) + 1; /* Skip STRING_END. */
    *entry = cursor;
    *pzob = zob;
    return ZE_OK;
}

ZError
zfeval(ZContext *zcontext, ZList *tmp, char **entry, Zob **pret)
{
    Zob *zfunc;
    ZList *args;
    char *fname, *cursor = *entry;
    Zob *ret = *pret;
    ZNameTable *self;
    ZError err;

    /* Get zfunc. */
    if (zgetincontext(zcontext, cursor, &self, &zfunc) == 0) {
        return ZE_FUNCTION_NAME_NOT_DEFINED;
    }
    fname = cursor;
    cursor += strlen(cursor) + 1; /* Skip STRING_END. */

    /* Get arg list. */
    err = znewlist(&args);
    if (err != ZE_OK)
        return err;
    while (*cursor != CALLEND) {
        Zob *arg;

        err = zeval(zcontext, tmp, &cursor, &arg);
        if (err != ZE_OK) {
            zdellist(&args);
            return err;
        }
        err = zlappend(args, arg);
        if (err != ZE_OK) {
            zdellist(&args);
            return err;
        }
    }
    *entry = cursor;
    if (args->length != ((ZFunc *) zfunc)->arity) {
        zdellist(&args);
        return ZE_ARITY_ERROR;
    }
    if (*(((ZFunc *) zfunc)->fimp)) {
        char *zapfunc;
        ZNode *item;
        unsigned char be;

        /* Call zap function. */
        err = zpushlocal(zcontext);
        if (err != ZE_OK) {
            zdellist(&args);
            return err;
        }
        if (self != zcontext->global) {
            /* Set the instance reference. */
            err = zsetincontext(zcontext, "@", (Zob *) self);
            if (err != ZE_OK) {
                zdellist(&args);
                return err;
            }
        }
        zapfunc = ((ZHighFunc *) ((ZFunc *) zfunc)->fimp)->func;
        item = args->first;
        while (*zapfunc != '\0') {
            err = zsetincontext(zcontext, zapfunc, item->object);
            if (err != ZE_OK) {
                zdellist(&args);
                return err;
            }
            zapfunc += strlen(zapfunc) + 1;
            item = item->next;
        }
        zapfunc++;
        be = 0;
        err = zrun_block(zcontext, tmp, 0, &zapfunc, &be);
        if (err != ZE_OK) {
            zdellist(&args);
            return err;
        }
        err = zpoplocal(zcontext, &ret);
        if (err != ZE_OK) {
            zdellist(&args);
            return err;
        }
    }
    else {
        /* Call C function. */
        err = ((ZLowFunc *) ((ZFunc *) zfunc)->fimp)->func(args, &ret);
        if (err != ZE_OK) {
            zdellist(&args);
            return err;
        }
    }
    zdellist(&args);
    *pret = ret;
    return ZE_OK;
}

void
zskip_assign(char **entry)
{
    char *cursor = *entry;

    while (*cursor != '\0') {
        if (*cursor == ASGNOPEN) {
            int depth = 1;

            cursor++;
            while (depth > 0) {
                if (*cursor == ASGNOPEN)
                    depth++;
                else if (*cursor == ASGNCLOSE)
                    depth--;
                cursor++;
            }
        }
        else {
            cursor += strlen(cursor) + 1; /* Skip NAME_END. */
        }
    }
    cursor++; /* Skip ASSIGN_END. */
    *entry = cursor;
}

ZError
zassign(ZContext *zcontext, Zob *value, char **entry)
{
    char *cursor = *entry;
    ZError err;
    ZNode *deepnode;

    while (*cursor != '\0') {
        if (*cursor == ASGNOPEN) {
            cursor++;
            if (*value != T_LIST)
                return ZE_ASSIGN_ERROR;
            deepnode = ((ZList *) value)->first;
            err = zdeepassign(zcontext, deepnode, &cursor);
            if (err != ZE_OK)
                return err;
        }
        else {
            err = zsetincontext(zcontext, cursor, value);
            if (err != ZE_OK)
                return err;
            cursor += strlen(cursor) + 1; /* Skip NAME_END. */
        }
    }
    cursor++; /* Skip ASSIGN_END. */
    *entry = cursor;
    return ZE_OK;
}

ZError
zdeepassign(ZContext *zcontext, ZNode *node, char **entry)
{
    char *cursor = *entry;
    ZError err;
    ZNode *deepnode;

    while (*cursor != ASGNCLOSE) {
        if (node == NULL)
            return ZE_ASSIGN_ERROR;
        if (*cursor == ASGNOPEN) {
            cursor++;
            if (*node->object != T_LIST)
                return ZE_ASSIGN_ERROR;
            deepnode = ((ZList *) node->object)->first;
            err = zdeepassign(zcontext, deepnode, &cursor);
            if (err != ZE_OK)
                return err;
        }
        else {
            err = zsetincontext(zcontext, cursor, node->object);
            if (err != ZE_OK)
                return err;
            cursor += strlen(cursor) + 1; /* Skip NAME_END. */
        }
        node = node->next;
    }
    if (node != NULL)
        return ZE_ASSIGN_ERROR;
    cursor++;
    *entry = cursor;
    return ZE_OK;
}

ZError
zrunstatement(ZContext *zcontext, ZList *tmp, char **entry)
{
    Zob *value;
    ZError err;

    err = zeval(zcontext, tmp, &(*entry), &value);
    if (err != ZE_OK)
        return err;
    return zassign(zcontext, value, &(*entry));
}

void
zskip_block(char **entry)
{
    char *cursor = *entry;

    while (*cursor != BLOCKEXIT  ||
           *(cursor + 1) != END) {
        if (*cursor == BLOCKEXIT) {
            cursor++;
            if (*cursor == BREAK ||
                *cursor == CONTINUE)
                cursor += 2;
            else if (*cursor == RETURN) {
                cursor++;
                zskip_expr(&cursor);
            }
        }
        else if (*cursor == DELETE) {
            cursor++;
            while (*cursor != '\0')
                cursor += strlen(cursor) + 1;
            cursor++;
        }
        else if (*cursor == BLOCK) {
            cursor++;
            if (*cursor == IF) {
                cursor++;
                zskip_expr(&cursor);
                zskip_block(&cursor);
            }
            while (*cursor == BLOCK  &&
                   *(cursor + 1) == ELIF) {
                cursor += 2;
                zskip_expr(&cursor);
                zskip_block(&cursor);
            }
            if (*cursor == BLOCK  &&
                *(cursor + 1) == ELSE) {
                cursor += 2;
                zskip_block(&cursor);
            }
            else if (*cursor == WHILE) {
                cursor++;
                zskip_expr(&cursor);
                zskip_block(&cursor);
            }
            else if (*cursor == DEF) {
                cursor++;
                cursor += strlen(cursor) + 1;
                while (*cursor != '\0')
                    cursor += strlen(cursor) + 1;
                cursor++;
                zskip_block(&cursor);
            }
        }
        else {
            /* Statement. */
            zskip_expr(&cursor);
            zskip_assign(&cursor);
        }
    }
    cursor += 2;
    *entry = cursor;
}

ZError
zrun_block(ZContext *zcontext,
          ZList *tmp,
          char looplev,
          char **entry,
          unsigned char *be)
{
    char *cursor = *entry;
    int truth;
    ZError err;

    while (*cursor != BLOCKEXIT) {
        if (*cursor == DELETE) {
            cursor++;
            while (*cursor != '\0') {
                if (zremincontext(zcontext, cursor) == 0)
                    return ZE_NAME_NOT_DEFINED;
                cursor += strlen(cursor) + 1;
            }
            cursor++;
        }
        else if (*cursor == BLOCK) {
            cursor++;
            if (*cursor == IF) {
                int ok = 0;
                Zob *zob;

                cursor++;
                err = zeval(zcontext, tmp, &cursor, &zob);
                if (err != ZE_OK)
                    return err;
                truth = ztstobj(zob);
                if (err != ZE_OK)
                    return err;
                if (truth) {
                    err = zrun_block(zcontext, tmp, looplev, &cursor, be);
                    if (err != ZE_OK)
                        return err;
                    if (*be & (BE_BREAK | BE_CONTINUE | BE_RETURN))
                        return ZE_OK;
                    ok = 1;
                }
                else
                    zskip_block(&cursor);
                while (*cursor == BLOCK  &&
                       *(cursor + 1) == ELIF) {
                    cursor += 2;
                    if (ok) {
                        zskip_expr(&cursor);
                        zskip_block(&cursor);
                    }
                    else {
                        err = zeval(zcontext, tmp, &cursor, &zob);
                        if (err != ZE_OK)
                            return err;
                        truth = ztstobj(zob);
                        if (err != ZE_OK)
                            return err;
                        if (truth) {
                            err = zrun_block(zcontext, tmp, looplev,
                                             &cursor, be);
                            if (err != ZE_OK)
                                return err;
                            if (*be & (BE_BREAK | BE_CONTINUE | BE_RETURN))
                                return ZE_OK;
                            ok = 1;
                        }
                        else
                            zskip_block(&cursor);
                    }
                }
                if (*cursor == BLOCK  &&
                    *(cursor + 1) == ELSE) {
                    cursor += 2;
                    if (ok)
                        zskip_block(&cursor);
                    else {
                        err = zrun_block(zcontext, tmp, looplev,
                                         &cursor, be);
                        if (err != ZE_OK)
                            return err;
                        if (*be & (BE_BREAK | BE_CONTINUE | BE_RETURN))
                            return ZE_OK;
                    }
                }
            }
            else if (*cursor == WHILE) {
                char *cond, *block, *blockend = NULL;
                char *b, *c;
                Zob *zob;

                cursor++;
                cond = cursor;
                zskip_expr(&cursor);
                block = cursor;
                c = cond;
                err = zeval(zcontext, tmp, &c, &zob);
                if (err != ZE_OK)
                    return err;
                truth = ztstobj(zob);
                if (err != ZE_OK)
                    return err;
                while (truth) {
                    b = block;
                    err = zrun_block(zcontext, tmp, looplev + 1, &b, be);
                    if (err != ZE_OK)
                        return err;
                    if (*be & BE_RETURN)
                        return ZE_OK;
                    if (*be & BE_BREAK) {
                        if (*be - BE_BREAK > 0) {
                            /* Propagate. */
                            (*be)--;
                            return ZE_OK;
                        }
                        break;
                    }
                    blockend = b;
                    c = cond;
                    err = zeval(zcontext, tmp, &c, &zob);
                    if (err != ZE_OK)
                        return err;
                    truth = ztstobj(zob);
                    if (err != ZE_OK)
                        return err;
                    if (*be & BE_CONTINUE)
                        if (!truth)
                            if (*be - BE_CONTINUE > 0) {
                                /* Propagate. */
                                (*be)--;
                                return ZE_OK;
                            }
                }
                if (blockend == NULL)
                    zskip_block(&cursor);
                else
                    cursor = blockend;
            }
            else if (*cursor == DEF) {
                /* Function definition. */
                char *name;
                char *zapfunc;
                unsigned char arity = 0;
                ZFunc *zfunc;
                ZHighFunc *zhighfunc;

                cursor++;
                name = cursor;
                cursor += strlen(name) + 1;
                zapfunc = cursor;
                while (*cursor != '\0') {
                    arity++;
                    cursor += strlen(cursor) + 1;
                }
                cursor++;
                zskip_block(&cursor);
                err = znewhighfunc(&zhighfunc);
                if (err != ZE_OK)
                    return err;
                zhighfunc->func = zapfunc;
                err = znewfunc(&zfunc, (FImp *) zhighfunc, arity);
                if (err != ZE_OK)
                    return err;
                err = zsetincontext(zcontext, name, (Zob *) zfunc);
                if (err != ZE_OK)
                    return err;
            }
        }
        else {
            /* Statement. */
            err = zrunstatement(zcontext, tmp, &cursor);
            if (err != ZE_OK)
                return err;
            /* Garbage Collection. */
            zlempty(tmp);
        }
    }
    cursor++;
    if (*cursor == END) {
        cursor++;
        *entry = cursor;
        *be = BE_END;
        return ZE_OK;
    }
    if (*cursor == BREAK) {
        char lev;

        cursor++;
        lev = *cursor;
        if (lev >= looplev)
            return ZE_BREAK_WITHOUT_LOOP;
        *be = (unsigned char) BE_BREAK | lev;
        return ZE_OK;
    }
    if (*cursor == CONTINUE) {
        char lev;

        cursor++;
        lev = *cursor;
        if (lev >= looplev)
            return ZE_CONTINUE_WITHOUT_LOOP;
        *be = (unsigned char) BE_CONTINUE | lev;
        return ZE_OK;
    }
    if (*cursor == RETURN) {
        Zob *ret;

        /* Function Return. */
        cursor++;
        err = zeval(zcontext, tmp, &cursor, &ret);
        if (err != ZE_OK)
            return err;
        err = zsetincontext(zcontext, "_ret_", ret);
        if (err != ZE_OK)
            return err;
        *be = 0;
        return ZE_OK;
    }
    return ZE_OK;
}
