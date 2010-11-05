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

/* ZDict Type */

#include <stdlib.h>
#include <stdio.h>

#include "ztypes.h"
#include "zerr.h"
#include "zgc.h"

#include "zlist.h"
#include "zdict.h"

#include "zobject.h"

/* A ZDict object holds a zlist where each pair of items is a
 * key-value pair (i.e. ["a", 1, "b", 2] -> {"a": 1, "b": 2}).
 */

/* Create a new ZDict in 'zdict'.
 * If there is not enough memory, return ZE_OUT_OF_MEMORY.
 * Otherwise, return ZE_OK.
 */
ZError
znewdict(ZDict **zdict)
{
    ZError err;

    *zdict = (ZDict *) malloc(sizeof(ZDict));
    if (*zdict == NULL)
        return ZE_OUT_OF_MEMORY;
    (*zdict)->type = T_DICT;
    err = znewlist(&(*zdict)->zlist);
    if (err != ZE_OK)
        return err;
    (*zdict)->refc = 0;
    return ZE_OK;
}

/* Remove 'zdict' from memory. */
void
zdeldict(ZDict **zdict)
{
    zdellist(&(*zdict)->zlist);
    free(*zdict);
    *zdict = NULL;
}

/* Create a new copy of 'source' in 'dest'.
 * If there is not enough memory, return ZE_OUT_OF_MEMORY.
 * Otherwise, return ZE_OK.
 */
ZError
zcpydict(ZDict *source, ZDict **dest)
{
    ZError err;

    *dest = (ZDict *) malloc(sizeof(ZDict));
    if (*dest == NULL)
        return ZE_OUT_OF_MEMORY;
    (*dest)->type = T_DICT;
    err = zcpylist(source->zlist, &(*dest)->zlist);
    if (err != ZE_OK)
        return err;
    (*dest)->refc = 0;
    return ZE_OK;
}

/* Test the truth value of 'zdict'.
 * If 'zdict' is empty, return zero.
 * Otherwise, return nonzero.
 */
int
ztstdict(ZDict *zdict)
{
    if (zdict->zlist->length)
        return 1;
    else
        return 0;
}

/* Compare 'zdict' and 'other'.
 * If they are equal, return zero.
 * Otherwise, return nonzero.
 */
int
zcmpdict(ZDict *zdict, ZDict *other)
{
    return zcmplist(zdict->zlist, other->zlist);
}

/* Print the textual representation of 'zdict' on 'buffer'.
 * Return the number of bytes writen.
 */
unsigned int
zrepdict(char *buffer, ZDict *zdict)
{
    if (zdict->zlist->first == NULL)
        return sprintf(buffer, "<Empty ZDict>");
    else {
        char nodebff[256];
        ZNode *cur = zdict->zlist->first;
        /* buffer length to return */
        unsigned int blen = 1;

        *buffer = '{';
        while (1) {
            if (cur->next->next == NULL) {
                zrepobj(nodebff, cur->object);
                blen += sprintf(buffer + blen, "%s: ", nodebff);
                zrepobj(nodebff, cur->next->object);
                blen += sprintf(buffer + blen, "%s}", nodebff);
                break;
            }
            zrepobj(nodebff, cur->object);
            blen += sprintf(buffer + blen, "%s: ", nodebff);
            zrepobj(nodebff, cur->next->object);
            blen += sprintf(buffer + blen, "%s, ", nodebff);
            cur = cur->next->next;
        }
        return blen;
    }
}

/* Return the number of key-value pairs in 'zdict'. */
unsigned int
zdlength(ZDict *zdict)
{
    return zdict->zlist->length / 2;
}

/* Define or redefine 'key' in 'zdict'.
 * If there is not enough memory, return ZE_OUT_OF_MEMORY.
 * Otherwise, return ZE_OK.
 */
ZError
zdset(ZDict *zdict, Zob *key, Zob *value)
{
    /* Seek key. */
    ZNode *item;
    ZError err;

    item = zdict->zlist->first;
    while (item != NULL) {
        if (zcmpobj(item->object, key) == 0) {
            /* Set value. */
            zdecrefc(item->next->object);
            item->next->object = value;
            zincrefc(value);
            return ZE_OK;
        }
        item = item->next->next;
    }

    /* Key not found. */
    /* New key. */
    /* FIXME: should check ZE_OUT_OF_MEMORY while appending. */
    err = zlappend(zdict->zlist, key);
    if (err != ZE_OK)
        return err;
    err = zlappend(zdict->zlist, value);
    if (err != ZE_OK)
        return err;
    return ZE_OK;
}

/* If 'key' is in 'zdict', copy its value to 'value' and return nonzero.
 * Otherwise, return zero.
 */
int
zdget(ZDict *zdict, Zob *key, Zob **value)
{
    /* Seek key. */
    ZNode *item;

    item = zdict->zlist->first;
    while (item != NULL) {
        if (zcmpobj(item->object, key) == 0) {
            /* Get value. */
            *value = item->next->object;
            return 1;
        }
        item = item->next->next;
    }

    /* Key not found. */
    return 0;
}

/* Define or redefine all items from 'other' to 'zdict'.
 * If there is not enough memory, return ZE_OUT_OF_MEMORY.
 * Otherwise, return ZE_OK.
 */
ZError
zdupdate(ZDict *zdict, ZDict *other)
{
    ZNode *key, *value;
    ZError err;

    key = other->zlist->first;
    while (key != NULL) {
        value = key->next;
        err = zdset(zdict, key->object, value->object);
        if (err != ZE_OK)
            return err;
        key = value->next;
    }
    return ZE_OK;
}

/* If 'key' is in 'zdict', remove its pair from 'zdict' and return nonzero.
 * Otherwise, return zero.
 */
int
zdremove(ZDict *zdict, Zob *key)
{
    /* Seek key. */
    ZNode *item;
    int index = 0;

    item = zdict->zlist->first;
    while (item != NULL) {
        if (!zcmpobj(item->object, key)) {
            zlremove(zdict->zlist, index);
            zlremove(zdict->zlist, index);
            return 1;
        }
        item = item->next->next;
        index += 2;
    }

    /* Key not found. */
    return 0;
}

/* Delete all key-value pairs in 'zdict'. */
void
zdempty(ZDict *zdict)
{
    zlempty(zdict->zlist);
}

/* If 'key' is in 'zdict', return nonzero.
 * Otherwise, return zero.
 */
int
zdhaskey(ZDict *zdict, Zob *key)
{
    ZNode *item;

    item = zdict->zlist->first;
    while (item != NULL) {
        if (zcmpobj(item->object, key) == 0)
            return 1;
        item = item->next->next;
    }
    return 0;
}
