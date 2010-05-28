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

/* BigNum Type (header) */

typedef struct {
    Zob type;
    unsigned char refc;
    unsigned int length;
    unsigned int *words;
} BigNum;

char halfstr(char *str);
unsigned int strbitlen(char *str);
BigNum *newbnum();
BigNum *bnumfromstr(char *str);
void delbnum(BigNum **bignum);
BigNum *cpybnum(BigNum *bignum);
Byte *getbit(BigNum *bignum, int index);
void setbit(BigNum *bignum, int index);
void rstbit(BigNum *bignum, int index);
void lshiftbnum(BigNum *bignum, unsigned int shift);
void rshiftbnum(BigNum *bignum, unsigned int shift);
void double_dabble(char *buffer, BigNum *bignum);
int tstbnum(BigNum *bignum);
int cmpbnum(BigNum *bignum, BigNum *other);
unsigned int repbnum(char *buffer, BigNum *bignum);
