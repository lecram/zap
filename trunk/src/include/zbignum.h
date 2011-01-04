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

/* ZBigNum Type (header) */

typedef struct {
    Zob type;
    unsigned char refc;
    unsigned int length;
    unsigned int *words;
} ZBigNum;

char zhalfstr(char *s);
ZError zstrbitlen(char *s, unsigned int *bitlen);
ZError znewbnum(ZBigNum **zbignum, unsigned int length);
ZError zbnumfromstr(ZBigNum **zbignum, char *s);
void zdelbnum(ZBigNum **zbignum);
ZError zcpybnum(ZBigNum *source, ZBigNum **dest);
int ztstbnum(ZBigNum *zbignum);
int zcmpbnum(ZBigNum *zbignum, ZBigNum *other);
int zrepbnum(char *buffer, size_t size, ZBigNum *zbignum);
unsigned int znlength(ZBigNum *zbignum);
ZError znget(ZBigNum *zbignum, int index, ZByte **value);
ZError znset(ZBigNum *zbignum, int index, ZByte *zbyte);
ZError znrst(ZBigNum *zbignum, int index);
void znlshift(ZBigNum *zbignum, unsigned int shift);
void znrshift(ZBigNum *zbignum, unsigned int shift);
