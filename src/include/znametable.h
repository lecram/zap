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

/* ZNameTable Type (header) */

/* A skip list with p = 1/4 and MAX_LEVEL = 8 can safely
 *  accommodate up to 4^8 = 2^16 = 65536 elements.
 */
#define MAX_LEVEL 8

typedef struct ZEntry {
    char *name;
    Zob *value;
    struct ZEntry **next;
} ZEntry;

typedef struct {
    Zob type;
    unsigned char refc;
    int level;
    ZEntry *header;
} ZNameTable;

/* Internal functions. */
/* double zrandom(); */
/* int ztrndlevel(); */

ZError znewentry(int level, char *name, Zob *value, ZEntry **zentry);
void zdelentry(ZEntry **zentry);
ZError znewnable(ZNameTable **znable);
void zdelnable(ZNameTable **znable);
ZError zcpynable(ZNameTable *source, ZNameTable **dest);
int ztstnable(ZNameTable *znable);
int zcmpnable(ZNameTable *znable, ZNameTable *other);
int zrepnable(char *buffer, size_t size, ZNameTable *znable);
void zrepnable_detail(ZNameTable *znable);
unsigned int ztlength(ZNameTable *znable);
ZError ztset(ZNameTable *znable, char *name, Zob *value);
int ztget(ZNameTable *znable, char *name, Zob **value);
ZError ztupdate(ZNameTable *znable, ZNameTable *other);
int ztremove(ZNameTable *znable, char *name);
void ztempty(ZNameTable *znable);
int zthasname(ZNameTable *znable, char *name);
