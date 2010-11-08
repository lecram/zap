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

/* ZDict Type (header) */

typedef struct {
    Zob type;
    unsigned char refc;
    ZList *zlist;
} ZDict;

ZError znewdict(ZDict **zdict);
void zdeldict(ZDict **zdict);
ZError zcpydict(ZDict *source, ZDict **dest);
int ztstdict(ZDict *zdict);
int zcmpdict(ZDict *zdict, ZDict *other);
int zrepdict(char *buffer, size_t size, ZDict *zdict);
unsigned int zdlength(ZDict *zdict);
ZError zdset(ZDict *zdict, Zob *key, Zob *value);
int zdget(ZDict *zdict, Zob *key, Zob **value);
ZError zdupdate(ZDict *zdict, ZDict *other);
int zdremove(ZDict *zdict, Zob *key);
void zdempty(ZDict *zdict);
int zdhaskey(ZDict *zdict, Zob *key);
