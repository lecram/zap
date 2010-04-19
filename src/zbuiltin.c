/*
 * Built-in Functions
 */

#include <stdio.h>

#include "zap.h"

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
        case T_IARR:
            rval->value = ((BitArray *) obj)->length;
            break;
        case T_YARR:
            rval->value = ((ByteArray *) obj)->length;
            break;
        case T_WARR:
            rval->value = ((WordArray *) obj)->length;
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
    rval->value = eqobj(a, b);
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
regfunc(Dict *namespace, char *name, Zob *(*cfunc)(List *args))
{
    Func *func;
    LowFunc *lowfunc;

    lowfunc = newlowfunc();
    lowfunc->func = cfunc;
    func = newfunc((FImp *) lowfunc);
    setkey(namespace,
           (Zob *) yarrfromstr(name),
           (Zob *) func);
}

Dict *
bbuild()
{
    Dict *builtins = newdict();

    regfunc(builtins, "print", zprint);
    regfunc(builtins, "repr", zrepr);
    regfunc(builtins, "len", zlen);
    regfunc(builtins, "concat", zconcat);
    regfunc(builtins, "get", zget);
    regfunc(builtins, "+", zsum);
    regfunc(builtins, "==", zeq);
    regfunc(builtins, "<", zlt);
    
    return builtins;
}
