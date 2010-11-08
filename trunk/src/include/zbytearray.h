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

/* ZByte Array Type (header) */

typedef struct {
    Zob type;
    unsigned char refc;
    unsigned int length;
    char *bytes;
} ZByteArray;

ZError znewyarr(ZByteArray **zbytearray, unsigned int length);
ZError zyarrfromstr(ZByteArray **zbytearray, char *s);
void zdelyarr(ZByteArray **zbytearray);
ZError zcpyyarr(ZByteArray *source, ZByteArray **dest);
int ztstyarr(ZByteArray *zbytearray);
int zcmpyarr(ZByteArray *zbytearray, ZByteArray *other);
int zrepyarr(char *buffer, size_t size, ZByteArray *zbytearray);
int zrepplain(char *buffer, size_t size, ZByteArray *zbytearray);
unsigned int zalength(ZByteArray *zbytearray);
ZError zaget(ZByteArray *zbytearray, int index, ZByte **value);
ZError zaset(ZByteArray *zbytearray, int index, ZByte *zbyte);
ZError zconcatstr(ZByteArray *zbytearray, char *s);
ZError zconcat(ZByteArray *zbytearray, ZByteArray *other);
