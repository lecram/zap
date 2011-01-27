/* Copyright 2010-2011 by Marcel Rodrigues <marcelgmr@gmail.com>
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

/* ZNameTable Type */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "ztypes.h"
#include "zerr.h"
#include "zgc.h"

#include "znametable.h"

#include "zobject.h"

/* Return a pseudo-random number x, such that 0 <= x < 1. */
double
zrandom()
{
    return rand() / ((double) RAND_MAX + 1);
}

/* Return a random level for a new entry in a skip list. */
int
ztrndlevel(ZNameTable *znable)
{
    int level = 0;
    int max;

    if (znable->level == SLHEIGHT - 1)
        max = SLHEIGHT - 1;
    else
        /* Dirty Hack. */
        max = znable->level + 1;
    while (zrandom() < SLPROB && level < max)
        level++;
    return level;
}

/* Create a new ZEntry in 'zentry', with 'name' referencing 'value'.
 * 'level' is the level of this entry in the skip list. It should
 *  be determined by ztrndlevel() and, optionally, by the dirty hack.
 * If there is not enough memory, return ZE_OUT_OF_MEMORY.
 * Otherwise, return ZE_OK.
 */
ZError
znewentry(int level, char *name, Zob *value, ZEntry **zentry)
{
    int i;

    *zentry = (ZEntry *) malloc(sizeof(ZEntry));
    if (*zentry == NULL)
        return ZE_OUT_OF_MEMORY;
    (*zentry)->name = (char *) malloc((strlen(name) + 1) * sizeof(char));
    if ((*zentry)->name == NULL)
        return ZE_OUT_OF_MEMORY;
    strcpy((*zentry)->name, name);
    (*zentry)->value = value;
    if (value != EMPTY)
        zincrefc(value);
    (*zentry)->next = (ZEntry **) malloc((level + 1) * sizeof(ZEntry *));
    if ((*zentry)->next == NULL)
        return ZE_OUT_OF_MEMORY;
    for (i = 0; i <= level; i++) {
        (*zentry)->next[i] = NULL;
    }
    return ZE_OK;
}

/* Remove 'zentry' from memory, unbinding its object. */
void
zdelentry(ZEntry **zentry)
{
    if ((*zentry)->value != EMPTY)
        zdecrefc((*zentry)->value);
    free((*zentry)->name);
    free((*zentry)->next);
    free(*zentry);
    *zentry = NULL;
}

/* Create a new ZNameTable in 'znable'.
 * If there is not enough memory, return ZE_OUT_OF_MEMORY.
 * Otherwise, return ZE_OK.
 */
ZError
znewnable(ZNameTable **znable)
{
    /* [Re]Initialize pseudo-random number generator. */
    srand(time(NULL));

    *znable = (ZNameTable *) malloc(sizeof(ZNameTable));
    if (*znable == NULL)
        return ZE_OUT_OF_MEMORY;
    (*znable)->type = T_NMTB;
    (*znable)->refc = 0;
    (*znable)->level = 0;
    return znewentry(SLHEIGHT - 1, "", EMPTY, &(*znable)->header);
}

/* Remove 'znable' from memory. */
void
zdelnable(ZNameTable **znable)
{
    ZEntry *a, *b;

    a = (*znable)->header;
    do {
        b = a->next[0];
        zdelentry(&a);
        a = b;
    } while (a != NULL);
    free(*znable);
    *znable = NULL;
}

/* Create a new copy of 'source' in 'dest'.
 * If there is not enough memory, return ZE_OUT_OF_MEMORY.
 * Otherwise, return ZE_OK.
 */
ZError
zcpynable(ZNameTable *source, ZNameTable **dest)
{
    ZError err;
    ZEntry *zentry;

    err = znewnable(dest);
    if (err != ZE_OK)
        return err;
    zentry = source->header->next[0];
    while (zentry != NULL) {
        err = ztset(*dest, zentry->name, zentry->value);
        if (err != ZE_OK)
            return err;
        zentry = zentry->next[0];
    }
    return ZE_OK;
}

/* Test the truth value of 'znable'.
 * If 'znable' is empty, return zero.
 * Otherwise, return nonzero.
 */
int
ztstnable(ZNameTable *znable)
{
    if (znable->header->next[0] != NULL)
        return 1;
    else
        return 0;
}

/* Compare 'znable' and 'other'.
 * If they are equal, return zero.
 * Otherwise, return nonzero.
 */
int
zcmpnable(ZNameTable *znable, ZNameTable *other)
{
    ZEntry *zentry;
    Zob *object;

    zentry = znable->header->next[0];
    while (zentry != NULL) {
        if (ztget(other, zentry->name, &object)) {
            if (zcmpobj(zentry->value, object) != 0)
                /* Has key but it is set to a different value. */
                return 1;
        }
        zentry = zentry->next[0];
    }
    return 0;
}

/* Print the textual representation of 'znable' on 'buffer'.
 * Return the number of bytes writen.
 */
int
zrepnable(char *buffer, size_t size, ZNameTable *znable)
{
    if (znable->header->next[0] == NULL)
        return snprintf(buffer, size, "<Empty NameTable>");
    else {
        char nodebff[256];
        ZEntry *cur = znable->header->next[0];
        /* buffer length to return */
        int blen = 1;

        *buffer = '{';
        while (1) {
            if (cur->next[0] == NULL) {
                blen += snprintf(buffer + blen,
                                 size,
                                 "%s:",
                                 cur->name);
                zrepobj(nodebff, 256, cur->value);
                blen += snprintf(buffer + blen,
                                 size,
                                 "%s}",
                                 nodebff);
                break;
            }
            blen += snprintf(buffer + blen,
                             size,
                             "%s:",
                             cur->name);
            zrepobj(nodebff, 256, cur->value);
            blen += snprintf(buffer + blen,
                             size,
                             "%s ",
                             nodebff);
            cur = cur->next[0];
        }
        return blen;
    }
}

void
zrepnable_detail(ZNameTable *znable)
{
    ZEntry *cur[SLHEIGHT];
    ZEntry *step = znable->header;
    int i;

    for (i = 0; i <= znable->level; i++)
        printf("%03d ", i);
    puts("");
    for (i = 0; i <= znable->level; i++)
        printf(" |  ");
    puts("");
    for (i = 0; i <= znable->level; i++) {
        cur[i] = znable->header;
    }
    while (step->next[0] != NULL) {
        for (i = 0; i <= znable->level; i++) {
            if (cur[i]->next[i] == step->next[0]) {
                printf("%03d ", ((int) cur[i]->next[i]) % 1000);
                cur[i] = cur[i]->next[0];
            }
            else
                printf(" |  ");
        }
        puts("");

        for (i = 0; i <= znable->level; i++)
            printf(" |  ");
        puts("");
        step = step->next[0];
    }
    for (i = 0; i <= znable->level; i++)
        printf("--- ");
    puts("");
}

/* Return the number of name-value pairs in 'znable'. */
unsigned int
ztlength(ZNameTable *znable)
{
    ZEntry *zentry;
    unsigned int len = 0;

    zentry = znable->header->next[0];
    while (zentry != NULL) {
        len++;
        zentry = zentry->next[0];
    }
    return len;
}

/* Define or redefine 'name' in 'znable'.
 * If there is not enough memory, return ZE_OUT_OF_MEMORY.
 * Otherwise, return ZE_OK.
 */
ZError
ztset(ZNameTable *znable, char *name, Zob *value)
{
    ZEntry *zentry;
    ZEntry *update[SLHEIGHT];
    int found, i;
    ZError err;

    /* Seek name. */
    zentry = znable->header;
    for (i = znable->level; i >= 0; i--) {
        while (zentry->next[i] != NULL) {
            if (strcmp(zentry->next[i]->name, name) >= 0)
                break;
            zentry = zentry->next[i];
        }
        update[i] = zentry;
    }
    zentry = zentry->next[0];
    found = (zentry != NULL);
    if (found)
        found = (strcmp(zentry->name, name) == 0);
    if (found) {
        /* Set value. */
        zdecrefc(zentry->value);
        zentry->value = value;
        zincrefc(value);
    }
    else {
        /* Name not found. */
        /* New entry. */
        int level = ztrndlevel(znable);

        if (level > znable->level) {
            /* Assuming Dirty Hack, level == znable->level + 1. */
            znable->level++;
            update[level] = znable->header;
        }
        err = znewentry(level, name, value, &zentry);
        if (err != ZE_OK)
            return err;
        /* Place the entry in the skip list. */
        for (i = 0; i <= level; i++) {
            zentry->next[i] = update[i]->next[i];
            update[i]->next[i] = zentry;
        }
    }
    return ZE_OK;
}

/* If 'name' is in 'znable', copy its value to 'value' and return nonzero.
 * Otherwise, return zero.
 */
int
ztget(ZNameTable *znable, char *name, Zob **value)
{
    ZEntry *zentry;
    int i;

    /* Seek name. */
    zentry = znable->header;
    for (i = znable->level; i >= 0; i--) {
        while (zentry->next[i] != NULL) {
            if (strcmp(zentry->next[i]->name, name) >= 0)
                break;
            zentry = zentry->next[i];
        }
    }
    zentry = zentry->next[0];
    if (strcmp(zentry->name, name) == 0) {
        /* Get value. */
        *value = zentry->value;
        return 1;
    }
    /* Name not found. */
    return 0;
}

/* Define or redefine all items from 'other' to 'znable'.
 * If there is not enough memory, return ZE_OUT_OF_MEMORY.
 * Otherwise, return ZE_OK.
 */
ZError
ztupdate(ZNameTable *znable, ZNameTable *other)
{
    ZError err;
    ZEntry *zentry;

    zentry = other->header->next[0];
    while (zentry != NULL) {
        err = ztset(znable, zentry->name, zentry->value);
        if (err != ZE_OK)
            return err;
        zentry = zentry->next[0];
    }
    return ZE_OK;
}

/* If 'name' is in 'znable', remove its pair from 'znable' and return nonzero.
 * Otherwise, return zero.
 */
int
ztremove(ZNameTable *znable, char *name)
{
    ZEntry *zentry;
    ZEntry *update[SLHEIGHT];
    int i;

    /* Seek name. */
    zentry = znable->header;
    for (i = znable->level; i >= 0; i--) {
        while (zentry->next[i] != NULL) {
            if (strcmp(zentry->next[i]->name, name) >= 0)
                break;
            zentry = zentry->next[i];
        }
        update[i] = zentry;
    }
    zentry = zentry->next[0];
    if (strcmp(zentry->name, name) == 0) {
        /* Remove pair. */
        for (i = 0; i <= znable->level; i++) {
            if (update[i]->next[i] != zentry)
                break;
            update[i]->next[i] = zentry->next[i];
        }
        zdelentry(&zentry);
        while (znable->level > 0  &&
               znable->header->next[znable->level] == NULL)
            znable->level--;
        return 1;
    }
    /* Name not found. */
    return 0;
}

/* Delete all name-value pairs in 'znable'. */
void
ztempty(ZNameTable *znable)
{
    ZEntry *a, *b;

    a = znable->header->next[0];
    do {
        b = a->next[0];
        zdelentry(&a);
        a = b;
    } while (a != NULL);
}

/* If 'name' is in 'znable', return nonzero.
 * Otherwise, return zero.
 */
int
zthasname(ZNameTable *znable, char *name)
{
    ZEntry *zentry;
    int i;

    /* Seek name. */
    zentry = znable->header;
    for (i = znable->level; i >= 0; i--) {
        while (zentry->next[i] != NULL) {
            if (strcmp(zentry->next[i]->name, name) >= 0)
                break;
            zentry = zentry->next[i];
        }
    }
    zentry = zentry->next[0];
    if (strcmp(zentry->name, name) == 0) {
        /* Name found. */
        return 1;
    }
    /* Name not found. */
    return 0;
}
