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

/* ZList Type (header) */

typedef struct ZNode {
    Zob *object;
    struct ZNode *next;
} ZNode;

typedef struct {
    Zob type;
    unsigned char refc;
    unsigned int length;
    ZNode *first;
    ZNode *last;
} ZList;

ZError znewnode(Zob *zob, ZNode **znode);
void zdelnode(ZNode **znode);
ZError znewlist(ZList **zlist);
void zdellist(ZList **zlist);
ZError zcpylist(ZList *source, ZList **dest);
int ztstlist(ZList *zlist);
int zcmplist(ZList *zlist, ZList *other);
int zreplist(char *buffer, size_t size, ZList *zlist);
unsigned int zllength(ZList *zlist);
ZError zlpush(ZList *zlist, Zob *zob);
Zob *zlpeek(ZList *zlist);
Zob *zlpop(ZList *zlist);
ZError zlappend(ZList *zlist, Zob *zob);
ZError zlset(ZList *zlist, int index, Zob *zob);
ZError zlget(ZList *zlist, int index, Zob **zob);
ZError zlinsert(ZList *zlist, int index, Zob *zob);
ZError zlextend(ZList *zlist, ZList *other);
ZError zlremove(ZList *zlist, int index);
void zlempty(ZList *zlist);
int zlhasitem(ZList *zlist, Zob *zob);
void zlremfirst(ZList *zlist);
