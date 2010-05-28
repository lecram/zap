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

/* Byte Array Type (header) */

typedef struct {
    Zob type;
    unsigned char refc;
    unsigned int length;
    char *bytes;
} ByteArray;

ByteArray *newyarr(unsigned int length);
ByteArray *yarrfromstr(char *s);
void delyarr(ByteArray **bytearray);
ByteArray *cpyyarr(ByteArray *bytearray);
Byte *getbyteitem(ByteArray *bytearray, int index);
void setbyteitem(ByteArray *bytearray, int index, Byte *byte);
void concatstr(ByteArray *bytearray, char *str);
void concat(ByteArray *bytearray, ByteArray *other);
int tstyarr(ByteArray *bytearray);
int cmpyarr(ByteArray *bytearray, ByteArray *other);
unsigned int repyarr(char *buffer, ByteArray *bytearray);
unsigned int repplain(char *buffer, ByteArray *bytearray);
