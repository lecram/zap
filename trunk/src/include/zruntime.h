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
#define ASGNOPEN    (char) 0x10
#define ASGNCLOSE   (char) 0x01

/* Block Exit Flags */
#define BE_END      (char) 0x80
#define BE_BREAK    (char) 0x40
#define BE_CONTINUE (char) 0x20
#define BE_RETURN   (char) 0x10

typedef struct {
    /* Global namespace. */
    ZNameTable *global;
    /* A stack of local namespaces. */
    ZList *local;
} ZContext;

ZError znewcontext(ZContext **zcontext);
void zdelcontext(ZContext **zcontext);
ZError zpushlocal(ZContext *zcontext);
ZError zpoplocal(ZContext *zcontext, Zob **ret);
ZError zsetincontext(ZContext *zcontext, char *name, Zob *value);
int zgetincontext(ZContext *zcontext, char *name, Zob **pvalue);
int zremincontext(ZContext *zcontext, char *name);
int zhasincontext(ZContext *zcontext, char *name);
unsigned int zreadword(char **entry);
unsigned int zread_uvlv(char **entry);
int zread_svlv(char **entry);
void zskip_uvlv(char **entry);
void zskip_svlv(char **entry);
void zskip_expr(char **entry);
ZError zeval(ZContext *zcontext, ZList *tmp, char **entry, Zob **pzob);
ZError znameval(ZContext *zcontext, char **entry, Zob **pzob);
ZError zfeval(ZContext *zcontext, ZList *tmp, char **entry, Zob **pret);
void zskip_assign(char **entry);
ZError zassign(ZContext *zcontext, Zob *value, char **entry);
ZError zdeepassign(ZContext *zcontext, ZNode *node, char **entry);
ZError zrunstatement(ZContext *zcontext, ZList *tmp, char **entry);
void zskip_block(char **entry);
ZError zrun_block(ZContext *zcontext,
                  ZList *tmp,
                  char looplev,
                  char **entry,
                  unsigned char *be);
