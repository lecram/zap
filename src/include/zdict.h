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

/* Dict Type (header) */

typedef struct {
    Zob type;
    unsigned char refc;
    List *list;
} Dict;

Dict *newdict();
void deldict(Dict **dict);
Dict *cpydict(Dict *dict);
int haskey(Dict *dict, Zob *key);
void setkey(Dict *dict, Zob *key, Zob *value);
Zob *getkey(Dict *dict, Zob *key, Zob *defval);
void remkey(Dict *dict, Zob *key);
void update(Dict *dict, Dict *other);
char eqdict(Dict *dict, Zob *other);
unsigned int repdict(char *buffer, Dict *dict);
