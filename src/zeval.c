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
    unsigned char *cursor = (unsigned char *) *entry;

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
    Zob *defval = 0;
    ByteArray *key;
    char *cursor = *entry;

    key = yarrfromstr(cursor);
    obj = getkey(namespace, (Zob *) key, defval);
    delyarr(&key);
    if (obj == defval) {
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
    Func *func;
    List *args;
    Zob *defval = 0;
    ByteArray *key;
    char *cursor = *entry;

    /* Get func. */
    /* printf("func: %s\n", cursor); */
    key = yarrfromstr(cursor);
    func = (Func *) getkey(namespace, (Zob *) key, defval);
    if ((Zob *) func == defval) {
        char errbff[256];

        sprintf(errbff,
                "Function Name not defined: %s.",
                cursor);
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
    return callimp(func->fimp, args);
}
