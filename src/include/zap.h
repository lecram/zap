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

/* zap (header) */

#include "zerr.h"
#include "ztypes.h"
#include "zobject.h"
#include "zgc.h"
#include "znone.h"
#include "zbool.h"
#include "zbyte.h"
#include "zword.h"
#include "zbitarray.h"
#include "zbytearray.h"
#include "zwordarray.h"
#include "zbignum.h"
#include "zlist.h"
#include "zdict.h"
#include "zfunc.h"

/* Bytecode Tokens */
#define BLOCKEXIT   0xBE
#define DELETE      0xDE
#define BLOCK       0xB0
#define IF          0x01
#define ELIF        0x02
#define ELSE        0x03
#define WHILE       0x04
#define DEF         0x05
#define END         0x01
#define BREAK       0x02
#define CONTINUE    0x03
#define RETURN      0x04

/* Block Exit Flags */
#define BE_END      0x80
#define BE_BREAK    0x40
#define BE_CONTINUE 0x20
#define BE_RETURN   0x10

typedef struct {
    Zob *universal;
    Zob *global;
    Zob *local;
} Space;

Space *newspace();
void delspace(Space **space);
void skip_assign(char **entry);
void assign(Dict *dict, Zob *value, char **entry);
void runstatement(Space *space, List *tmp, char **entry);
void skip_block(char **entry);
unsigned char run_block(Space *space, List *tmp, char looplev, char **entry);
int main(int argc, char *argv[]);
