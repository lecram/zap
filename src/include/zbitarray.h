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

/* Bit Array Type (header) */

typedef struct {
    Zob type;
    unsigned char refc;
    unsigned int length;
    char *bytes;
} BitArray;

BitArray *newiarr(unsigned int length);
void deliarr(BitArray **bitarray);
BitArray *cpyiarr(BitArray *bitarray);
Byte *getbititem(BitArray *bitarray, int index);
void setbititem(BitArray *bitarray, int index);
void rstbititem(BitArray *bitarray, int index);
char eqiarr(BitArray *bitarray, Zob *other);
unsigned int repiarr(char *buffer, BitArray *bitarray);
