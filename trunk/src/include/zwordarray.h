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

/* Word Array Type (header) */

typedef struct {
    Zob type;
    unsigned char refc;
    unsigned int length;
    unsigned int *words;
} WordArray;

WordArray *newwarr(unsigned int length);
void delwarr(WordArray **wordarray);
WordArray *cpywarr(WordArray *wordarray);
Word *getworditem(WordArray *wordarray, int index);
void setworditem(WordArray *wordarray, int index, Word *word);
char eqwarr(WordArray *wordarray, Zob *other);
unsigned int repwarr(char *buffer, WordArray *wordarray);
