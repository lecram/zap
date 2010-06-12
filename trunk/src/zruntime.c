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
 * - Space handle functions.
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

Space *
newspace()
{
    Space *space;

    space = (Space *) malloc(sizeof(Space));
    if (space == NULL) {
        raiseOutOfMemory("newspace");
        exit(EXIT_FAILURE);
    }
    return space;
}

void
delspace(Space **space)
{
    deldict(&(*space)->universal);
    deldict(&(*space)->global);
    deldict(&(*space)->local);
    free(*space);
    *space = NULL;
}

unsigned int
readword(char **entry)
{
    unsigned int i, word;
    char *cursor = *entry;

    word = (unsigned int) *cursor;
    cursor++;
    for (i = 1; i < 4; i++) {
        word <<= 8;
        word += (unsigned int) *cursor;
        cursor++;
    }
    *entry = cursor;
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
            while (*cursor) /* Require LIST_END == 0. */
                skip_expr(&cursor);
            cursor++; /* To skip LIST_END. */
            break;
        case T_DICT:
            cursor++;
            while (*cursor) { /* Require DICT_END == 0. */
                skip_expr(&cursor);  /*  Skip key.  */
                skip_expr(&cursor);  /* Skip value. */
            }
            cursor++; /* To skip DICT_END. */
            break;
        case (char) 0xF0:
            /* Function Call. */
            cursor++;
            cursor += strlen(cursor) + 1; /* To skip STRING_END. */
            while (*cursor != (char) 0xF1)
                skip_expr(&cursor);
            cursor++; /* To skip CALL_END. */
            break;
        default:
            /* Name. */
            cursor += strlen(cursor) + 1; /* To skip STRING_END. */
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
            /* Currently, there is no literal BigNums. */
            raise("There is no literal BigNums.");
            exit(EXIT_FAILURE);
            break;
        case T_LIST:
            {
                List *list;

                list = newlist();
                cursor++;
                while (*cursor) /* Require LIST_END == 0. */
                    /* WARNING: Require reference sharing! */
                    /* Copyied append will cause memory leak! */
                    appitem(list, eval(namespace, tmp, &cursor));
                cursor++; /* To skip LIST_END. */
                obj = (Zob *) list;
            }
            break;
        case T_DICT:
            {
                Dict *dict;
                Zob *key, *value;

                dict = newdict();
                cursor++;
                while (*cursor) { /* Require DICT_END == 0. */
                    /* WARNING: Require reference sharing! */
                    /* Copyied key setting will cause memory leak! */
                    key = eval(namespace, tmp, &cursor);
                    value = eval(namespace, tmp, &cursor);
                    setkey(dict, key, value);
                }
                cursor++; /* To skip DICT_END. */
                obj = (Zob *) dict;
            }
            break;
        case (char) 0xF0:
            /* Function Call. */
            cursor++;
            obj = feval(namespace, tmp, &cursor);
            cursor++; /* To skip CALL_END. */
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
    cursor += strlen(cursor) + 1; /* To skip STRING_END. */
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
    /* printf("func: %s\n", cursor); */
    key = yarrfromstr(cursor);
    func = (Func *) getkey(namespace, (Zob *) key, EMPTY);
    fname = cursor;
    if ((Zob *) func == EMPTY) {
        raiseFunctionNameNotDefined(fname);
        exit(EXIT_FAILURE);
    }
    cursor += strlen(cursor) + 1; /* To skip STRING_END. */
    delyarr(&key);

    /* Get arg list. */
    args = newlist();
    while (*cursor != (char) 0xF1)
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

    while (*cursor)
        cursor += strlen(cursor) + 1; /* To skip NAME_END. */
    cursor++; /* To skip ASSIGN_END. */
    *entry = cursor;
}

void
assign(Dict *dict, Zob *value, char **entry)
{
    ByteArray *key;
    char *cursor = *entry;

    while (*cursor) {
        key = yarrfromstr(cursor);
        cursor += strlen(cursor) + 1; /* To skip NAME_END. */
        setkey(dict, (Zob *) key, value);
    }
    cursor++; /* To skip ASSIGN_END. */
    *entry = cursor;
}

void
runstatement(Space *space, List *tmp, char **entry)
{
    Zob *value;

    value = eval(space->universal, tmp, &(*entry));
    assign(space->universal, value, &(*entry));
}

void
skip_block(char **entry)
{
    char *cursor = *entry;

    while (*cursor != (char) BLOCKEXIT  ||  *(cursor + 1) != (char) END) {
        if (*cursor == (char) BLOCKEXIT) {
            cursor++;
            if (*cursor == (char) BREAK || *cursor == (char) CONTINUE)
                cursor += 2;
            else if (*cursor == (char) RETURN) {
                /* Function Return. */
                /* NYI. */
            }
        }
        else if (*cursor == (char) DELETE) {
            cursor++;
            cursor += strlen(cursor) + 1;
        }
        else if (*cursor == (char) BLOCK) {
            cursor++;
            if (*cursor == (char) IF) {
                cursor++;
                skip_expr(&cursor);
                skip_block(&cursor);
            }
            while (*cursor == (char) BLOCK  &&  *(cursor + 1) == (char) ELIF) {
                cursor += 2;
                skip_expr(&cursor);
                skip_block(&cursor);
            }
            if (*cursor == (char) BLOCK  &&  *(cursor + 1) == (char) ELSE) {
                cursor += 2;
                skip_block(&cursor);
            }
            else if (*cursor == (char) WHILE) {
                cursor++;
                skip_expr(&cursor);
                skip_block(&cursor);
            }
            else if (*cursor == (char) DEF) {
                /* Function definition. */
                /* NYI. */
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
run_block(Space *space, List *tmp, char looplev, char **entry)
{
    char *cursor = *entry;
    Dict *namespace = space->universal;
    int truth;
    unsigned char be;

    while (*cursor != (char) BLOCKEXIT) {
        if (*cursor == (char) DELETE) {
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
        else if (*cursor == (char) BLOCK) {
            cursor++;
            if (*cursor == (char) IF) {
                int ok = 0;

                cursor++;
                truth = tstobj(eval(namespace, tmp, &cursor));
                if (truth) {
                    be = run_block(space, tmp, looplev, &cursor);
                    if (be & (BE_BREAK | BE_CONTINUE | BE_RETURN))
                        return be;
                    ok = 1;
                }
                else
                    skip_block(&cursor);
                while (*cursor == (char) BLOCK  &&  *(cursor + 1) == (char) ELIF) {
                    cursor += 2;
                    if (ok) {
                        skip_expr(&cursor);
                        skip_block(&cursor);
                    }
                    else {
                        truth = tstobj(eval(namespace, tmp, &cursor));
                        if (truth) {
                            be = run_block(space, tmp, looplev, &cursor);
                            if (be & (BE_BREAK | BE_CONTINUE | BE_RETURN))
                                return be;
                            ok = 1;
                        }
                        else
                            skip_block(&cursor);
                    }
                }
                if (*cursor == (char) BLOCK  &&  *(cursor + 1) == (char) ELSE) {
                    cursor += 2;
                    if (ok)
                        skip_block(&cursor);
                    else {
                        be = run_block(space, tmp, looplev, &cursor);
                        if (be & (BE_BREAK | BE_CONTINUE | BE_RETURN))
                            return be;
                    }
                }
            }
            else if (*cursor == (char) WHILE) {
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
                    be = run_block(space, tmp, looplev + 1, &b);
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
            else if (*cursor == (char) DEF) {
                /* Function definition. */
                /* NYI. */
            }
        }
        else {
            /* Statement. */
            runstatement(space, tmp, &cursor);
            /* Garbage Collection. */
            emptylist(tmp);
        }
    }
    cursor++;
    if (*cursor == (char) END) {
        cursor++;
        *entry = cursor;
        return BE_END;
    }
    if (*cursor == (char) BREAK) {
        char lev;

        cursor++;
        lev = *cursor;
        if (lev > looplev) {
            raise("Break without loop.");
            exit(EXIT_FAILURE);
        }
        return BE_BREAK | lev;
    }
    if (*cursor == (char) CONTINUE) {
        char lev;

        cursor++;
        lev = *cursor;
        if (lev > looplev) {
            raise("Continue without loop.");
            exit(EXIT_FAILURE);
        }
        return CONTINUE | lev;
    }
    if (*cursor == (char) RETURN) {
        /* Function Return. */
        /* NYI. */
    }
    return 0;
}

