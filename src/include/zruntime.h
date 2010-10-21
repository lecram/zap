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

/* Runtime (header) */

/* Bytecode Tokens */
#define CALLSTART   (char) 0xF0
#define CALLEND     (char) 0xF1
#define BLOCKEXIT   (char) 0xBE
#define DELETE      (char) 0xDE
#define BLOCK       (char) 0xB0
#define IF          (char) 0x01
#define ELIF        (char) 0x02
#define ELSE        (char) 0x03
#define WHILE       (char) 0x04
#define DEF         (char) 0x05
#define END         (char) 0x01
#define BREAK       (char) 0x02
#define CONTINUE    (char) 0x03
#define RETURN      (char) 0x04

/* Block Exit Flags */
#define BE_END      (char) 0x80
#define BE_BREAK    (char) 0x40
#define BE_CONTINUE (char) 0x20
#define BE_RETURN   (char) 0x10

typedef struct {
    /* Global namespace. */
    Dict *global;
    /* A stack of local namespaces. */
    List *local;
} Context;

Context *newcontext();
void delcontext(Context **context);
unsigned int readword(char **entry);
void skip_expr(char **entry);
Zob *eval(Dict *namespace, List *tmp, char **entry);
Zob *nameval(Dict *namespace, char **entry);
Zob *feval(Dict *namespace, List *tmp, char **entry);
void skip_assign(char **entry);
void assign(Dict *dict, Zob *value, char **entry);
void runstatement(Context *context, List *tmp, char **entry);
void skip_block(char **entry);
unsigned char run_block(Context *context,
                        List *tmp,
                        char looplev,
                        char **entry);
