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

/* List Type */

#include <stdlib.h>
#include <stdio.h>

#include "ztypes.h"
#include "zerr.h"
#include "zgc.h"

#include "zlist.h"

#include "zobject.h"

/* The object in list shares reference
 *  with the object passed as argument!
 */
Node *
newnode(Zob *object)
{
    Node *node;

    node = (Node *) malloc(sizeof(Node));
    if (node == NULL) {
        raiseOutOfMemory("newnode");
        exit(EXIT_FAILURE);
    }
    node->object = object;
    increfc(object);
    node->next = NULL; /* Security. */
    return node;
}

void
delnode(Node **node)
{
    decrefc((*node)->object);
    free(*node);
    *node = NULL;
}

List *
newlist()
{
    List *list;

    list = (List *) malloc(sizeof(List));
    if (list == NULL) {
        raiseOutOfMemory("newlist");
        exit(EXIT_FAILURE);
    }
    list->type = T_LIST;
    list->length = 0;
    list->first = NULL;
    list->last = NULL;
    list->refc = 0;
    return list;
}

void
dellist(List **list)
{
    while ((*list)->length > 0)
        remfirst(*list);
    free(*list);
    *list = NULL;
}

/* Remove all items in list.
 * Used by temp lists to minimize
 *  malloc() & free() calls.
 */
void
emptylist(List *list)
{
    while (list->length > 0)
        remfirst(list);
}

/* Items are duplicated in memory,
 *  i.e.,  items in new list do not
 *  share reference with items
 *  in original list.
 */
List *
cpylist(List *list)
{
    List *copy;
    Node *old = list->first;
    Node *new;

    copy = newlist();
    copy->length = list->length;
    if (old->next == NULL)
        return copy;
    new = newnode(cpyobj(old->object));
    copy->first = new;
    old = old->next;
    while (old != NULL) {
        new->next = newnode(cpyobj(old->object));
        new = new->next;
        old = old->next;
    }
    copy->last = new;
    new->next = NULL;
    return copy;
}

void
remfirst(List *list)
{
    if (list->first == NULL)
        return;
    if (list->first == list->last) {
        delnode(&list->first);
        list->first = NULL;
        list->last = NULL;
    }
    else {
        Node *second;

        second = list->first->next;
        delnode(&list->first);
        list->first = second;
    }
    list->length--;
}

/* Return the first item in the list and remove it. */
Zob *
popitem(List *list)
{
    Zob *item;

    if (list->first == NULL)
        return EMPTY;
    item = list->first->object;
    increfc(item);
    if (list->first == list->last) {
        delnode(&list->first);
        list->first = NULL;
        list->last = NULL;
    }
    else {
        Node *second;

        second = list->first->next;
        delnode(&list->first);
        list->first = second;
    }
    list->length--;
    return item;
}

/* Insert an object in front of the list. */
void
pushitem(List *list, Zob *object)
{
    Node *first;

    first = newnode(object);
    first->next = list->first;
    list->first = first;
    list->length++;
}

/* Return the first item in the list. */
Zob *
peekitem(List *list)
{
    return list->first->object;
}

/* Add object to the end of list. */
void
appitem(List *list, Zob *object)
{
    Node *item;

    item = newnode(object);
    item->next = NULL;
    if (list->first == NULL)
        list->first = item;
    else
        list->last->next = item;
    list->last = item;
    list->length++;
}

/* Set object at index.
 * The old object is deleted.
 */
void
setitem(List *list, int index, Zob *object)
{
    if (index < 0)
        index += list->length;
    if (index < 0 || index >= list->length) {
        raiseIndexOutOfRange("setitem", index, list->length);
        exit(EXIT_FAILURE);
    }
    else {
        int curidx = 0;
        Node *cur = list->first;

        while (curidx < index) {
            cur = cur->next;
            curidx++;
        }
        decrefc(cur->object);
        cur->object = object;
    }
}

/* Insert object in index.
 * If index == length, append object;
 */
void
insitem(List *list, int index, Zob *object)
{
    if (index < 0)
        index += list->length;
    if (index < 0 || index > list->length) {
        raiseIndexOutOfRange("insitem", index, list->length);
        exit(EXIT_FAILURE);
    }
    else if (index == list->length)
        appitem(list, object);
    else {
        int curidx = 0;
        Node *prev = list->first, *next;

        while (curidx < index - 1) {
            prev = prev->next;
            curidx++;
        }
        next = prev->next;
        prev->next = newnode(object);
        prev->next->next = next;
    }
    list->length++;
}

/* WARNING: The function extlist is deprecated.
 * This function was build under "copy police".
 * Should be rewrited under "share police".
 */

/* Concatenates list + ext and save result to list.
 * The same pointer can be passed to both args to double a list.
 * (The local vars count and length prevents an infinity loop
 * when doubling a list)
 */
void
extlist(List *list, List *ext)
{
    unsigned int count;
    unsigned int length = ext->length;
    Node *ecur = ext->first;

    for (count = 0; count < length; count++) {
        list->last->next = newnode(ecur->object);
        list->last = list->last->next;
        list->length++;
        ecur = ecur->next;
    }
}

/* Remove item at index. */
void
remitem(List *list, int index)
{
    if (index < 0)
        index += list->length;
    if (index < 0 || index >= list->length) {
        raiseIndexOutOfRange("remitem", index, list->length);
        exit(EXIT_FAILURE);
    }
    if (index == 0)
        remfirst(list);
    else {
        int curidx = 0;
        Node *prev = list->first, *next;

        while (curidx < index - 1) {
            prev = prev->next;
            curidx++;
        }
        next = prev->next->next;
        delnode(&prev->next);
        prev->next = next;
    }
    list->length--;
}

int
tstlist(List *list)
{
    if (list->length)
        return 1;
    else
        return 0;
}

int
cmplist(List *list, List *other)
{
    Node *item, *oitem;

    if (list->length != other->length)
        return 1;
    item = list->first;
    oitem = other->first;
    while (item != NULL) {
        if (cmpobj(item->object, oitem->object))
            return 1;
        item = item->next;
        oitem = oitem->next;
    }
    return 0;
}

unsigned int
replist(char *buffer, List *list)
{
    if (list->first == NULL)
        return sprintf(buffer, "<Empty List>");
    else {
        char nodebff[256];
        Node *cur = list->first;
        /* buffer length to return */
        unsigned int blen = 1;

        *buffer = '[';
        while (1) {
            if (cur->next == NULL) {
                repobj(nodebff, cur->object);
                blen += sprintf(buffer + blen, "%s]", nodebff);
                break;
            }
            repobj(nodebff, cur->object);
            blen += sprintf(buffer + blen, "%s, ", nodebff);
            cur = cur->next;
        }
        return blen;
    }
}
