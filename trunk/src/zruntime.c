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
 * - Context handle functions.
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

Context *
newcontext()
{
    Context *context;

    context = (Context *) malloc(sizeof(Context));
    if (context == NULL) {
        raiseOutOfMemory("newcontext");
        exit(EXIT_FAILURE);
    }
    return context;
}

void
delcontext(Context **context)
{
    deldict(&(*context)->global);
    dellist(&(*context)->local);
    free(*context);
    *context = NULL;
}

void
pushlocal(Context *context)
{
    pushitem(context->local, (Zob *) newdict());
}

Zob *
poplocal(Context *context)
{
    Dict *poped;
    ByteArray *retkey;
    Zob *retvalue;

    poped = (Dict *) popitem(context->local);
    retkey = yarrfromstr("_ret_");
    retvalue = getkey(poped, (Zob *) retkey, EMPTY);
    delyarr(&retkey);
    return retvalue;
}

unsigned int
readword(char **entry)
{
    unsigned int i, word;
    unsigned char *cursor;

    cursor = (unsigned char *) *entry;
    word = (unsigned int) *cursor;
    cursor++;
    for (i = 1; i < WL / 8; i++) {
        word <<= 8;
        word += (unsigned int) *cursor;
        cursor++;
    }
    *entry = (char *) cursor;
    return word;
}

void
skip_expr(char **entry)
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
                length = readword(&cursor);
                cursor += length;
            }
            break;
        case T_BNUM:
            /* Currently, there is no literal BigNums. */
            raise("There is no literal BigNums.");
            exit(EXIT_FAILURE);
            break;
        case T_LIST:
            cursor++;
            while (*cursor != '\0')
                skip_expr(&cursor);
            cursor++; /* Skip LIST_END. */
            break;
        case T_DICT:
            cursor++;
            while (*cursor != '\0') {
                skip_expr(&cursor);  /* Skip key. */
                skip_expr(&cursor);  /* Skip value. */
            }
            cursor++; /* Skip DICT_END. */
            break;
        case CALLSTART:
            /* Function Call. */
            cursor++;
            cursor += strlen(cursor) + 1; /* Skip STRING_END. */
            while (*cursor != CALLEND)
                skip_expr(&cursor);
            cursor++; /* Skip CALL_END. */
            break;
        default:
            /* Name. */
            cursor += strlen(cursor) + 1; /* Skip STRING_END. */
    }
    *entry = cursor;
}

Zob *
eval(Dict *namespace, List *tmp, char **entry)
{
    Zob *obj = NULL;
    char *cursor = *entry;

    switch (*cursor) {
        case T_NONE:
            obj = (Zob *) newnone();
            cursor++;
            break;
        case T_BOOL:
            {
                Bool *bool;

                bool = newbool();
                cursor++;
                bool->value = (int) *cursor;
                cursor++;
                obj = (Zob *) bool;
            }
            break;
        case T_BYTE:
            {
                Byte *byte;

                byte = newbyte();
                cursor++;
                byte->value = (unsigned char) *cursor;
                cursor++;
                obj = (Zob *) byte;
            }
            break;
        case T_WORD:
            {
                Word *word;

                word = newword();
                cursor++;
                word->value = readword(&cursor);
                obj = (Zob *) word;
            }
            break;
        case T_YARR:
            {
                ByteArray *bytearray;
                unsigned int length, index;

                cursor++;
                length = readword(&cursor);
                bytearray = newyarr(length);
                for (index = 0; index < length; index++) {
                    bytearray->bytes[index] = *cursor;
                    cursor++;
                }
                obj = (Zob *) bytearray;
            }
            break;
        case T_BNUM:
            {
                BigNum *bignum;
                unsigned int wordlen, index;

                cursor++;
                wordlen = readword(&cursor);
                bignum = newbnum(wordlen * WL);
                for (index = 0; index < wordlen; index++)
                    bignum->words[index] = readword(&cursor);
                obj = (Zob *) bignum;
            }
            break;
        case T_LIST:
            {
                List *list;

                list = newlist();
                cursor++;
                while (*cursor != '\0')
                    /* WARNING: Require reference sharing! */
                    /* Copyied append will cause memory leak! */
                    appitem(list, eval(namespace, tmp, &cursor));
                cursor++; /* Skip LIST_END. */
                obj = (Zob *) list;
            }
            break;
        case T_DICT:
            {
                Dict *dict;
                Zob *key, *value;

                dict = newdict();
                cursor++;
                while (*cursor != '\0') {
                    /* WARNING: Require reference sharing! */
                    /* Copyied key setting will cause memory leak! */
                    key = eval(namespace, tmp, &cursor);
                    value = eval(namespace, tmp, &cursor);
                    setkey(dict, key, value);
                }
                cursor++; /* Skip DICT_END. */
                obj = (Zob *) dict;
            }
            break;
        case CALLSTART:
            /* Function Call. */
            cursor++;
            obj = feval(namespace, tmp, &cursor);
            cursor++; /* Skip CALL_END. */
            break;
        default:
            /* Name. */
            obj = nameval(namespace, &cursor);
    }
    appitem(tmp, obj);
    *entry = cursor;
    return obj;
}

Zob *
nameval(Dict *namespace, char **entry)
{
    Zob *obj;
    ByteArray *key;
    char *cursor = *entry;

    key = yarrfromstr(cursor);
    obj = getkey(namespace, (Zob *) key, EMPTY);
    delyarr(&key);
    if (obj == EMPTY) {
        raiseNameNotDefined(cursor);
        exit(EXIT_FAILURE);
    }
    cursor += strlen(cursor) + 1; /* Skip STRING_END. */
    *entry = cursor;
    return obj;
}

Zob *
feval(Dict *namespace, List *tmp, char **entry)
{
    Zob *ret;
    Func *func;
    List *args;
    ByteArray *key;
    char *fname, *cursor = *entry;

    /* Get func. */
    key = yarrfromstr(cursor);
    func = (Func *) getkey(namespace, (Zob *) key, EMPTY);
    fname = cursor;
    if ((Zob *) func == EMPTY) {
        raiseFunctionNameNotDefined(fname);
        exit(EXIT_FAILURE);
    }
    cursor += strlen(cursor) + 1; /* Skip STRING_END. */
    delyarr(&key);

    /* Get arg list. */
    args = newlist();
    while (*cursor != CALLEND)
        /* WARNING: Require reference sharing! */
        /* Copyied append will cause memory leak! */
        appitem(args, eval(namespace, tmp, &cursor));
    *entry = cursor;
    if (args->length != func->arity) {
        raiseArityError(args->length, func->arity, fname);
        exit(EXIT_FAILURE);
    }
    ret = callimp(func->fimp, args);
    dellist(&args);
    return ret;
}

void
skip_assign(char **entry)
{
    char *cursor = *entry;

    while (*cursor != '\0')
        cursor += strlen(cursor) + 1; /* Skip NAME_END. */
    cursor++; /* Skip ASSIGN_END. */
    *entry = cursor;
}

void
assign(Dict *dict, Zob *value, char **entry)
{
    ByteArray *key;
    char *cursor = *entry;

    while (*cursor != '\0') {
        key = yarrfromstr(cursor);
        cursor += strlen(cursor) + 1; /* Skip NAME_END. */
        if (setkey(dict, (Zob *) key, value) == 0)
            delyarr(&key);
    }
    cursor++; /* Skip ASSIGN_END. */
    *entry = cursor;
}

void
runstatement(Context *context, List *tmp, char **entry)
{
    Zob *value;

    value = eval(context->global, tmp, &(*entry));
    assign(context->global, value, &(*entry));
}

void
skip_block(char **entry)
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
                skip_expr(&cursor);
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
                skip_expr(&cursor);
                skip_block(&cursor);
            }
            while (*cursor == BLOCK  &&
                   *(cursor + 1) == ELIF) {
                cursor += 2;
                skip_expr(&cursor);
                skip_block(&cursor);
            }
            if (*cursor == BLOCK  &&
                *(cursor + 1) == ELSE) {
                cursor += 2;
                skip_block(&cursor);
            }
            else if (*cursor == WHILE) {
                cursor++;
                skip_expr(&cursor);
                skip_block(&cursor);
            }
            else if (*cursor == DEF) {
                cursor++;
                cursor += strlen(cursor) + 1;
                while (*cursor != '\0')
                    cursor += strlen(cursor) + 1;
                cursor++;
                skip_block(&cursor);
            }
        }
        else {
            /* Statement. */
            skip_expr(&cursor);
            skip_assign(&cursor);
        }
    }
    cursor += 2;
    *entry = cursor;
}

unsigned char
run_block(Context *context, List *tmp, char looplev, char **entry)
{
    char *cursor = *entry;
    Dict *namespace = context->global;
    int truth;
    unsigned char be;

    while (*cursor != BLOCKEXIT) {
        if (*cursor == DELETE) {
            ByteArray *key;

            cursor++;
            key = yarrfromstr(cursor);
            if (haskey(namespace, (Zob *) key))
                remkey(namespace, (Zob *) key);
            else {
                raiseNameNotDefined(cursor);
                exit(EXIT_FAILURE);
            }
            delyarr(&key);
            cursor += strlen(cursor) + 1;
        }
        else if (*cursor == BLOCK) {
            cursor++;
            if (*cursor == IF) {
                int ok = 0;

                cursor++;
                truth = tstobj(eval(namespace, tmp, &cursor));
                if (truth) {
                    be = run_block(context, tmp, looplev, &cursor);
                    if (be & (BE_BREAK | BE_CONTINUE | BE_RETURN))
                        return be;
                    ok = 1;
                }
                else
                    skip_block(&cursor);
                while (*cursor == BLOCK  &&
                       *(cursor + 1) == ELIF) {
                    cursor += 2;
                    if (ok) {
                        skip_expr(&cursor);
                        skip_block(&cursor);
                    }
                    else {
                        truth = tstobj(eval(namespace, tmp, &cursor));
                        if (truth) {
                            be = run_block(context, tmp, looplev, &cursor);
                            if (be & (BE_BREAK | BE_CONTINUE | BE_RETURN))
                                return be;
                            ok = 1;
                        }
                        else
                            skip_block(&cursor);
                    }
                }
                if (*cursor == BLOCK  &&
                    *(cursor + 1) == ELSE) {
                    cursor += 2;
                    if (ok)
                        skip_block(&cursor);
                    else {
                        be = run_block(context, tmp, looplev, &cursor);
                        if (be & (BE_BREAK | BE_CONTINUE | BE_RETURN))
                            return be;
                    }
                }
            }
            else if (*cursor == WHILE) {
                char *cond, *block, *blockend = NULL;
                char *b, *c;

                cursor++;
                cond = cursor;
                skip_expr(&cursor);
                block = cursor;
                c = cond;
                truth = tstobj(eval(namespace, tmp, &c));
                while (truth) {
                    b = block;
                    be = run_block(context, tmp, looplev + 1, &b);
                    if (be & BE_RETURN)
                        return be;
                    if (be & BE_BREAK) {
                        if (be - BE_BREAK > 0)
                            /* Propagate. */
                            return be - 1;
                        break;
                    }
                    blockend = b;
                    c = cond;
                    truth = tstobj(eval(namespace, tmp, &c));
                    if (be & BE_CONTINUE)
                        if (!truth)
                            if (be - BE_CONTINUE > 0)
                                /* Propagate. */
                                return be - 1;
                }
                if (blockend == NULL)
                    skip_block(&cursor);
                else
                    cursor = blockend;
            }
            else if (*cursor == DEF) {
                /* Function definition. */
                char *name;
                char *zapfunc;
                unsigned char arity = 0;
                Func *func;
                HighFunc *highfunc;

                cursor++;
                name = cursor;
                cursor += strlen(name) + 1;
                zapfunc = cursor;
                while (*cursor != '\0') {
                    arity++;
                    cursor += strlen(cursor) + 1;
                }
                cursor++;
                skip_block(&cursor);
                highfunc = newhighfunc();
                highfunc->func = zapfunc;
                func = newfunc((FImp *) highfunc, arity);
                setkey(namespace,
                       (Zob *) yarrfromstr(name),
                       (Zob *) func);
            }
        }
        else {
            /* Statement. */
            runstatement(context, tmp, &cursor);
            /* Garbage Collection. */
            emptylist(tmp);
        }
    }
    cursor++;
    if (*cursor == END) {
        cursor++;
        *entry = cursor;
        return BE_END;
    }
    if (*cursor == BREAK) {
        char lev;

        cursor++;
        lev = *cursor;
        if (lev >= looplev) {
            raise("Break without loop.");
            exit(EXIT_FAILURE);
        }
        return BE_BREAK | lev;
    }
    if (*cursor == CONTINUE) {
        char lev;

        cursor++;
        lev = *cursor;
        if (lev >= looplev) {
            raise("Continue without loop.");
            exit(EXIT_FAILURE);
        }
        return BE_CONTINUE | lev;
    }
    if (*cursor == RETURN) {
        /* Function Return. */
        /* NYI. */
    }
    return 0;
}