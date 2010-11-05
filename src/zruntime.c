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
#include "zword.h"
#include "zbytearray.h"
#include "zbignum.h"
#include "zlist.h"
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
    zdeldict(&(*zcontext)->global);
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
    ZDict *zdict;
    ZError err;

    err = znewdict(&zdict);
    if (err != ZE_OK)
        return err;
    return zlpush(zcontext->local, (Zob *) zdict);
}

/* Pop a namespace from 'zcontext' and save its "_ret_" value in 'ret'.
 * If there is not enough memory, return ZE_OUT_OF_MEMORY.
 * Otherwise, return ZE_OK.
 */
ZError
zpoplocal(ZContext *zcontext, Zob **ret)
{
    ZDict *poped;
    ZByteArray *retkey;
    ZError err;

    poped = (ZDict *) zlpop(zcontext->local);
    err = zyarrfromstr(&retkey, "_ret_");
    if (err != ZE_OK)
        return err;
    zdget(poped, (Zob *) retkey, ret);
    zdelyarr(&retkey);
    zdeldict(&poped);
    return ZE_OK;
}

/* Define or redefine 'key' in 'zcontext'.
 * If there is not enough memory, return ZE_OUT_OF_MEMORY.
 * Otherwise, return ZE_OK.
 */
ZError
zsetincontext(ZContext *zcontext, Zob *key, Zob *value)
{
    if (zcontext->local->length > 0) {
        ZDict *local;

        local = (ZDict *) zlpeek(zcontext->local);
        return zdset(local, key, value);
    }
    return zdset(zcontext->global, key, value);
}

/* If 'key' is in 'zcontext', copy its value to 'value' and return nonzero.
 * Otherwise, return zero.
 */
int
zgetincontext(ZContext *zcontext, Zob *key, Zob **pvalue)
{
    Zob *value = *pvalue;
    int ok = 0;

    if (zcontext->local->length > 0) {
        ZDict *local;

        local = (ZDict *) zlpeek(zcontext->local);
        ok = zdget(local, key, &value);
    }
    if (!ok) {
        ok = zdget(zcontext->global, key, &value);
    }
    *pvalue = value;
    if (!ok)
        return 0;
    return 1;
}

/* If 'key' is in 'zcontext',
 *  remove its pair from 'zcontext' and return nonzero.
 * Otherwise, return zero.
 */
int
zremincontext(ZContext *zcontext, Zob *key)
{
    if (zcontext->local->length > 0) {
        ZDict *local;

        local = (ZDict *) zlpeek(zcontext->local);
        if (zdremove(local, key) != 0)
            return 1;
    }
    return zdremove(zcontext->global, key);
}

/* If 'key' is in 'zcontext', return nonzero.
 * Otherwise, return zero.
 */
int
zhasincontext(ZContext *zcontext, Zob *key)
{
    if (zcontext->local->length > 0) {
        ZDict *local;

        local = (ZDict *) zlpeek(zcontext->local);
        if (zdhaskey(local, key))
            return 1;
    }
    if (zdhaskey(zcontext->global, key))
        return 1;
    return 0;
}

/* Return the current namespace to write in 'zcontext'. */
ZDict *
zcwdict(ZContext *zcontext)
{
    if (zcontext->local->length > 0)
        return (ZDict *) zlpeek(zcontext->local);
    return zcontext->global;
}

unsigned int
zreadword(char **entry)
{
    unsigned int i, zint;
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
        case T_WORD:
            cursor += 5;
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
            err = znewnone((ZNone **) &zob);
            if (err != ZE_OK)
                return err;
            cursor++;
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
        case T_WORD:
            {
                ZInt *zint;

                err = znewint(&zint);
                if (err != ZE_OK)
                    return err;
                cursor++;
                zint->value = zreadword(&cursor);
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
                    zbytearray->bytes[index] = *cursor;
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
                err = znewbnum(&zbignum, wordlen * WL);
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
    ZByteArray *key;
    char *cursor = *entry;
    Zob *zob = *pzob;
    ZError err;

    err = zyarrfromstr(&key, cursor);
    if (err != ZE_OK)
        return err;
    if (zgetincontext(zcontext, (Zob *) key, &zob) == 0) {
        zdelyarr(&key);
        return ZE_NAME_NOT_DEFINED;
    }
    zdelyarr(&key);
    cursor += strlen(cursor) + 1; /* Skip STRING_END. */
    *entry = cursor;
    *pzob = zob;
    return ZE_OK;
}

ZError
zfeval(ZContext *zcontext, ZList *tmp, char **entry, Zob **pret)
{
    ZFunc *zfunc;
    ZList *args;
    ZByteArray *key;
    char *fname, *cursor = *entry;
    Zob *ret = *pret;
    ZError err;

    /* Get zfunc. */
    err = zyarrfromstr(&key, cursor);
    if (err != ZE_OK)
        return err;
    if (zgetincontext(zcontext, (Zob *) key, (Zob **) &zfunc) == 0) {
        zdelyarr(&key);
        return ZE_FUNCTION_NAME_NOT_DEFINED;
    }
    fname = cursor;
    cursor += strlen(cursor) + 1; /* Skip STRING_END. */
    zdelyarr(&key);

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
    if (args->length != zfunc->arity) {
        zdellist(&args);
        return ZE_ARITY_ERROR;
    }
    if (*(zfunc->fimp)) {
        char *zapfunc;
        ZNode *item;
        unsigned char be;

        /* Call zap function. */
        err = zpushlocal(zcontext);
        if (err != ZE_OK) {
            zdellist(&args);
            return err;
        }
        zapfunc = ((ZHighFunc *) zfunc->fimp)->func;
        item = args->first;
        while (*zapfunc != '\0') {
            err = zyarrfromstr(&key, zapfunc);
            if (err != ZE_OK) {
                zdellist(&args);
                return err;
            }
            err = zsetincontext(zcontext, (Zob *) key, item->object);
            if (err != ZE_OK) {
                zdellist(&args);
                zdelyarr(&key);
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
        err = ((ZLowFunc *) zfunc->fimp)->func(args, &ret);
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

    while (*cursor != '\0')
        cursor += strlen(cursor) + 1; /* Skip NAME_END. */
    cursor++; /* Skip ASSIGN_END. */
    *entry = cursor;
}

ZError
zassign(ZContext *zcontext, Zob *value, char **entry)
{
    ZByteArray *key;
    char *cursor = *entry;
    unsigned int wdlen;
    ZError err;

    while (*cursor != '\0') {
        err = zyarrfromstr(&key, cursor);
        if (err != ZE_OK)
            return err;
        cursor += strlen(cursor) + 1; /* Skip NAME_END. */
        wdlen = zdlength(zcwdict(zcontext));
        err = zsetincontext(zcontext, (Zob *) key, value);
        if (err != ZE_OK) {
            zdelyarr(&key);
            return err;
        }
        if (zdlength(zcwdict(zcontext)) == wdlen)
            zdelyarr(&key);
    }
    cursor++; /* Skip ASSIGN_END. */
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
            cursor += strlen(cursor) + 1;
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
            ZByteArray *key;

            cursor++;
            err = zyarrfromstr(&key, cursor);
            if (err != ZE_OK)
                return err;
            if (zremincontext(zcontext, (Zob *) key) == 0) {
                zdelyarr(&key);
                return ZE_NAME_NOT_DEFINED;
            }
            zdelyarr(&key);
            cursor += strlen(cursor) + 1;
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
                unsigned int wdlen;
                ZFunc *zfunc;
                ZHighFunc *zhighfunc;
                ZByteArray *key;

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
                err = zyarrfromstr(&key, name);
                if (err != ZE_OK)
                    return err;
                wdlen = zdlength(zcwdict(zcontext));
                err = zsetincontext(zcontext, (Zob *) key, (Zob *) zfunc);
                if (err != ZE_OK)
                    return err;
                if (zdlength(zcwdict(zcontext)) == wdlen)
                    zdelyarr(&key);
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
        *be = BE_BREAK | lev;
        return ZE_OK;
    }
    if (*cursor == CONTINUE) {
        char lev;

        cursor++;
        lev = *cursor;
        if (lev >= looplev)
            return ZE_CONTINUE_WITHOUT_LOOP;
        *be = BE_CONTINUE | lev;
        return ZE_OK;
    }
    if (*cursor == RETURN) {
        ZByteArray *key;
        Zob *ret;

        /* Function Return. */
        cursor++;
        err = zyarrfromstr(&key, "_ret_");
        if (err != ZE_OK)
            return err;
        err = zeval(zcontext, tmp, &cursor, &ret);
        if (err != ZE_OK)
            return err;
        zsetincontext(zcontext, (Zob *) key, ret);
        *be = 0;
        return ZE_OK;
    }
    return ZE_OK;
}
