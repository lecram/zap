/*
 * Dict Type
 */

#include <stdlib.h>
#include <stdio.h>

#include "zap.h"

/* A Dict object holds a list where each pair of items is a
 * key-value pair (i.e. ["a", 1, "b", 2] -> {"a": 1, "b": 2}).
 */

Dict *
newdict()
{
    Dict *dict;

    dict = (Dict *) malloc(sizeof(Dict));
    if (dict == NULL)
        raise("Out of memory in newdict().");
    dict->type = T_DICT;
    dict->list = newlist();
    dict->refc = 0;
    return dict;
}

void
deldict(Dict **dict)
{
    dellist(&(*dict)->list);
    free(*dict);
    *dict = NULL;
}

Dict *
cpydict(Dict *dict)
{
    Dict *copy;

    copy = (Dict *) malloc(sizeof(Dict));
    if (copy == NULL)
        raise("Out of memory in cpydict().");
    copy->type = T_DICT;
    copy->list = cpylist(dict->list);
    copy->refc = 0;
    return copy;
}

int
haskey(Dict *dict, Zob *key)
{
    Node *item;

    item = dict->list->first;
    while (item != NULL) {
        if (eqobj(item->object, key))
            return 1;
        item = item->next->next;
    }
    return 0;
}

void
setkey(Dict *dict, Zob *key, Zob *value)
{
    /* Seek key. */
    Node *item;

    item = dict->list->first;
    while (item != NULL) {
        if (eqobj(item->object, key)) {
            /* Set value. */
            decrefc(item->next->object);
            /* delobj(&item->next->object); */
            /* should cpyobj(value) ??? */
            item->next->object = value;
            increfc(value);
            return;
        }
        item = item->next->next;
    }

    /* Key not found. */
    /* New key. */
    appitem(dict->list, key);
    appitem(dict->list, value);
}

Zob *
getkey(Dict *dict, Zob *key, Zob *defval)
{
    /* Seek key. */
    Node *item;

    item = dict->list->first;
    while (item != NULL) {
        if (eqobj(item->object, key))
            /* Get value. */
            /* should cpyobj(item->next->object) ??? */
            return item->next->object;
        item = item->next->next;
    }

    /* Key not found. */
    /* Return default. */
    /* should cpyobj(defval) ??? */
    return defval;
}

void
remkey(Dict *dict, Zob *key)
{
    /* Seek key. */
    Node *item;
    int index = 0;

    item = dict->list->first;
    while (item != NULL) {
        if (eqobj(item->object, key)) {
            remitem(dict->list, index);
            remitem(dict->list, index);
            return;
        }
        item = item->next->next;
        index += 2;
    }

    /* Key not found. */
    raise("Key not found.");
}

void
update(Dict *dict, Dict *other)
{
    Node *key, *value;

    key = other->list->first;
    while (key != NULL) {
        value = key->next;
        setkey(dict, key->object, value->object);
        key = value->next;
    }
}

char
eqdict(Dict *dict, Zob *other)
{
    if (*other != T_DICT) return 0;
    else {
        Dict *odict;

        odict = (Dict *) other;
        return eqlist(dict->list, (Zob *) odict->list);
    }
}

unsigned int
repdict(char *buffer, Dict *dict)
{
    if (dict->list->first == NULL)
        return sprintf(buffer, "<Empty Dict>");
    else {
        char nodebff[256];
        Node *cur = dict->list->first;
        /* buffer length to return */
        unsigned int blen = 1;

        *buffer = '{';
        while (1) {
            if (cur->next->next == NULL) {
                repobj(nodebff, cur->object);
                blen += sprintf(buffer + blen, "%s: ", nodebff);
                repobj(nodebff, cur->next->object);
                blen += sprintf(buffer + blen, "%s}", nodebff);
                break;
            }
            repobj(nodebff, cur->object);
            blen += sprintf(buffer + blen, "%s: ", nodebff);
            repobj(nodebff, cur->next->object);
            blen += sprintf(buffer + blen, "%s, ", nodebff);
            cur = cur->next->next;
        }
        return blen;
    }
}
