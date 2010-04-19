/*
 * Copyright 2010 by Marcel Rodrigues <marcelgmr@gmail.com>
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

/*
 * List Type (header)
 */

struct Node {
    Zob *object;
    struct Node *next;
};

typedef struct Node Node;

typedef struct {
    Zob type;
    unsigned char refc;
    unsigned int length;
    Node *first;
    Node *last;
} List;

Node * newnode(Zob *object);
void delnode(Node **node);
List *newlist();
void dellist(List **list);
void emptylist(List *list);
List *cpylist(List *list);
void appitem(List *list, Zob *object);
void setitem(List *list, int index, Zob *object);
void insitem(List *list, int index, Zob *object);
void extlist(List *list, List *ext);
void remfirst(List *list);
void remitem(List *list, int index);
char eqlist(List *list, Zob *other);
unsigned int replist(char *buffer, List *list);
