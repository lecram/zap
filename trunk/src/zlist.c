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

/* ZList Type */

#include <stdlib.h>
#include <stdio.h>

#include "ztypes.h"
#include "zerr.h"
#include "zgc.h"

#include "zlist.h"

#include "zobject.h"

/* Create a new ZNode in 'znode', referencing 'zob'.
 * If there is not enough memory, return ZE_OUT_OF_MEMORY.
 * Otherwise, return ZE_OK.
 */
ZError
znewnode(Zob *zob, ZNode **znode)
{
    *znode = (ZNode *) malloc(sizeof(ZNode));
    if (*znode == NULL)
        return ZE_OUT_OF_MEMORY;
    (*znode)->object = zob;
    zincrefc(zob);
    (*znode)->next = NULL; /* Security. */
    return ZE_OK;
}

/* Remove 'znode' from memory, unbinding its object. */
void
zdelnode(ZNode **znode)
{
    zdecrefc((*znode)->object);
    free(*znode);
    *znode = NULL;
}

/* Create a new ZList in 'zlist'.
 * If there is not enough memory, return ZE_OUT_OF_MEMORY.
 * Otherwise, return ZE_OK.
 */
ZError
znewlist(ZList **zlist)
{
    *zlist = (ZList *) malloc(sizeof(ZList));
    if (*zlist == NULL)
        return ZE_OUT_OF_MEMORY;
    (*zlist)->type = T_LIST;
    (*zlist)->length = 0;
    (*zlist)->first = NULL;
    (*zlist)->last = NULL;
    (*zlist)->refc = 0;
    return ZE_OK;
}

/* Remove 'zlist' from memory. */
void
zdellist(ZList **zlist)
{
    while ((*zlist)->length > 0)
        zlremfirst(*zlist);
    free(*zlist);
    *zlist = NULL;
}

/* Create a new copy of 'source' in 'dest'.
 * Items are duplicated in memory, i.e.,
 * items in 'dest' do not share reference with items in 'source'.
 * If there is not enough memory, return ZE_OUT_OF_MEMORY.
 * Otherwise, return ZE_OK.
 */
ZError
zcpylist(ZList *source, ZList **dest)
{
    ZNode *old = source->first;
    ZNode *new;
    Zob *newzob;
    ZError err;

    err = znewlist(dest);
    if (err != ZE_OK)
        return err;
    (*dest)->length = source->length;
    if (old->next == NULL)
        return ZE_OK;
    err = zcpyobj(old->object, &newzob);
    if (err != ZE_OK)
        return err;
    err = znewnode(newzob, &new);
    if (err != ZE_OK)
        return err;
    (*dest)->first = new;
    old = old->next;
    while (old != NULL) {
        err = zcpyobj(old->object, &newzob);
        if (err != ZE_OK)
            return err;
        err = znewnode(newzob, &new->next);
        if (err != ZE_OK)
            return err;
        new = new->next;
        old = old->next;
    }
    (*dest)->last = new;
    new->next = NULL;
    return ZE_OK;
}

/* Test the truth value of 'zlist'.
 * If 'zlist' is empty, return zero.
 * Otherwise, return nonzero.
 */
int
ztstlist(ZList *zlist)
{
    if (zlist->length)
        return 1;
    else
        return 0;
}

/* Compare 'zlist' and 'other'.
 * If they are equal, return zero.
 * Otherwise, return nonzero.
 */
int
zcmplist(ZList *zlist, ZList *other)
{
    ZNode *item, *oitem;

    if (zlist->length != other->length)
        return 1;
    item = zlist->first;
    oitem = other->first;
    while (item != NULL) {
        if (zcmpobj(item->object, oitem->object))
            return 1;
        item = item->next;
        oitem = oitem->next;
    }
    return 0;
}

/* Print the textual representation of 'zlist' on 'buffer'.
 * Return the number of bytes writen.
 */
int
zreplist(char *buffer, size_t size, ZList *zlist)
{
    if (zlist->first == NULL)
        return snprintf(buffer, size, "<Empty ZList>");
    else {
        char nodebff[256];
        ZNode *cur = zlist->first;
        /* buffer length to return */
        int blen = 1;

        *buffer = '[';
        while (1) {
            if (cur->next == NULL) {
                zrepobj(nodebff, 256, cur->object);
                blen += snprintf(buffer + blen, size, "%s]", nodebff);
                break;
            }
            zrepobj(nodebff, 256, cur->object);
            blen += snprintf(buffer + blen, size, "%s, ", nodebff);
            cur = cur->next;
        }
        return blen;
    }
}

/* Return the number of items in 'zlist'. */
unsigned int
zllength(ZList *zlist)
{
    return zlist->length;
}

/* Insert 'zob' in front of 'zlist'.
 * If there is not enough memory, return ZE_OUT_OF_MEMORY.
 * Otherwise, return ZE_OK.
 */
ZError
zlpush(ZList *zlist, Zob *zob)
{
    ZNode *first;
    ZError err;

    err = znewnode(zob, &first);
    if (err != ZE_OK)
        return err;
    first->next = zlist->first;
    zlist->first = first;
    zlist->length++;
    return ZE_OK;
}

/* If 'zlist' is empty, return EMPTY.
 * Otherwise, return the first item in 'zlist'. */
Zob *
zlpeek(ZList *zlist)
{
    if (zlist->first == NULL)
        return EMPTY;
    return zlist->first->object;
}

/* Remove the first item from 'zlist' and return it. */
Zob *
zlpop(ZList *zlist)
{
    Zob *item;

    if (zlist->first == NULL)
        return EMPTY;
    item = zlist->first->object;
    zincrefc(item);
    if (zlist->first == zlist->last) {
        zdelnode(&zlist->first);
        zlist->first = NULL;
        zlist->last = NULL;
    }
    else {
        ZNode *second;

        second = zlist->first->next;
        zdelnode(&zlist->first);
        zlist->first = second;
    }
    zlist->length--;
    return item;
}

/* Add 'zob' to the end of 'zlist'.
 * If there is not enough memory, return ZE_OUT_OF_MEMORY.
 * Otherwise, return ZE_OK.
 */
ZError
zlappend(ZList *zlist, Zob *zob)
{
    ZNode *item;
    ZError err;

    err = znewnode(zob, &item);
    if (err != ZE_OK)
        return err;
    item->next = NULL;
    if (zlist->first == NULL)
        zlist->first = item;
    else
        zlist->last->next = item;
    zlist->last = item;
    zlist->length++;
    return ZE_OK;
}

/* Replace the 'index'-th item of 'zlist' by 'zob'.
 * If 'index' is negative, use zllength('zlist') + 'index'.
 * If 'index' is out of range, return ZE_INDEX_OUT_OF_RANGE.
 * Otherwise, return ZE_OK.
 */
ZError
zlset(ZList *zlist, int index, Zob *zob)
{
    if (index < 0)
        index += zlist->length;
    if (index < 0 || index >= (int) zlist->length)
        return ZE_INDEX_OUT_OF_RANGE;
    else {
        int curidx = 0;
        ZNode *cur = zlist->first;

        while (curidx < index) {
            cur = cur->next;
            curidx++;
        }
        zdecrefc(cur->object);
        cur->object = zob;
    }
    return ZE_OK;
}

/* Insert 'zob' before 'index'-th position in 'zlist'.
 * If 'index' == 'length', append 'zob'.
 * If 'index' is negative, use zllength('zlist') + 'index'.
 * If 'index' is out of range, return ZE_INDEX_OUT_OF_RANGE.
 * If there is not enough memory, return ZE_OUT_OF_MEMORY.
 * Otherwise, return ZE_OK.
 */
ZError
zlinsert(ZList *zlist, int index, Zob *zob)
{
    if (index < 0)
        index += zlist->length;
    if (index < 0 || index > (int) zlist->length)
        return ZE_INDEX_OUT_OF_RANGE;
    else if (index == (int) zlist->length) {
        ZError err;

        err = zlappend(zlist, zob);
        if (err != ZE_OK)
            return err;
    }
    else {
        int curidx = 0;
        ZNode *prev = zlist->first;
        ZNode *next;
        ZError err;

        while (curidx < index - 1) {
            prev = prev->next;
            curidx++;
        }
        next = prev->next;
        err = znewnode(zob, &prev->next);
        if (err != ZE_OK)
            return err;
        prev->next->next = next;
    }
    zlist->length++;
    return ZE_OK;
}

/* Concatenate 'other' to 'zlist'.
 * The same pointer can be passed to both args to double 'zlist'.
 * (The local vars 'count' and 'length' prevents an infinity loop
 * when doubling 'zlist').
 * If there is not enough memory, return ZE_OUT_OF_MEMORY.
 * Otherwise, return ZE_OK.
 */
ZError
zlextend(ZList *zlist, ZList *other)
{
    unsigned int count;
    unsigned int length = other->length;
    ZNode *ecur = other->first;
    ZError err;

    for (count = 0; count < length; count++) {
        err = znewnode(ecur->object, &zlist->last->next);
        if (err != ZE_OK)
            return err;
        zlist->last = zlist->last->next;
        zlist->length++;
        ecur = ecur->next;
    }
    return ZE_OK;
}

/* Remove 'index'-th item from 'zlist'.
 * If 'index' is negative, use zllength('zlist') + 'index'.
 * If 'index' is out of range, return ZE_INDEX_OUT_OF_RANGE.
 * Otherwise, return ZE_OK.
 */
ZError
zlremove(ZList *zlist, int index)
{
    if (index < 0)
        index += zlist->length;
    if (index < 0 || index >= (int) zlist->length)
        return ZE_INDEX_OUT_OF_RANGE;
    if (index == 0)
        zlremfirst(zlist);
    else {
        int curidx = 0;
        ZNode *prev = zlist->first;
        ZNode *next;

        while (curidx < index - 1) {
            prev = prev->next;
            curidx++;
        }
        next = prev->next->next;
        zdelnode(&prev->next);
        prev->next = next;
    }
    zlist->length--;
    return ZE_OK;
}

/* Remove all items from 'zlist'.
 * Used by temp lists to minimize malloc() & free() calls.
 */
void
zlempty(ZList *zlist)
{
    while (zlist->length > 0)
        zlremfirst(zlist);
}

/* If 'zob' is in 'zlist', return nonzero.
 * Otherwise, return zero.
 */
int
zlhasitem(ZList *zlist, Zob *zob)
{
    ZNode *item = zlist->first;

    while (item != NULL) {
        if (zcmpobj(item->object, zob) == 0)
            return 1;
        item = item->next;
    }
    return 0;
}

/* Remove the first item from 'zlist'. */
void
zlremfirst(ZList *zlist)
{
    if (zlist->first == NULL)
        return;
    if (zlist->first == zlist->last) {
        zdelnode(&zlist->first);
        zlist->first = NULL;
        zlist->last = NULL;
    }
    else {
        ZNode *second;

        second = zlist->first->next;
        zdelnode(&zlist->first);
        zlist->first = second;
    }
    zlist->length--;
}
