/*
 * Copyright 2010 by Marcel Rodrigues <marcelgmr@gmail.com>
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

/*
 * Evaluation
 */

#include <stdio.h>
#include <string.h>

#include "zap.h"
#include "zeval.h"

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
        case T_IARR:
            /* Currently, there is no literal BitArrays. */
            raise("There is no literal BitArrays.");
            break;
        case T_YARR:
            {
                unsigned int length;

                cursor++;
                length = readword(&cursor);
                cursor += length;
            }
            break;
        case T_WARR:
            /* Currently, this case will not be reached.
             * There is not syntax for WordArrays yet.
             */
            {
                unsigned int length;

                cursor++;
                length = readword(&cursor);
                cursor += length * 4;
            }
            break;
        case T_BNUM:
            /* Currently, there is no literal BigNums. */
            raise("There is no literal BigNums.");
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
        case T_IARR:
            /* Currently, there is no literal BitArrays. */
            raise("There is no literal BitArrays.");
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
        case T_WARR:
            /* Currently, this case will not be reached.
             * There is not syntax for WordArrays yet.
             */
            {
                WordArray *wordarray;
                unsigned int length, index;

                cursor++;
                length = readword(&cursor);
                wordarray = newwarr(length);
                for (index = 0; index < length; index++)
                    wordarray->words[index] = readword(&cursor);
                obj = (Zob *) wordarray;
            }
            break;
        case T_BNUM:
            /* Currently, there is no literal BigNums. */
            raise("There is no literal BigNums.");
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
    obj = getkey(namespace, (Zob *) key, 0);
    delyarr(&key);
    if (obj == 0) {
        char errbff[256];

        sprintf(errbff,
                "Name not defined: %s.",
                cursor);
        raise(errbff);
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
    func = (Func *) getkey(namespace, (Zob *) key, 0);
    fname = cursor;
    if ((Zob *) func == 0) {
        char errbff[256];

        sprintf(errbff,
                "Function Name not defined: %s.",
                fname);
        raise(errbff);
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
        char errbff[256];

        sprintf(errbff,
                "%i argument(s) passed to %i-ary function %s().",
                args->length,
                func->arity,
                fname);
        raise(errbff);
    }
    ret = callimp(func->fimp, args);
    dellist(&args);
    return ret;
}
