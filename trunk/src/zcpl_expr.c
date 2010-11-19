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

/* Expression Compiler */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "ztypes.h"
#include "zerr.h"

#include "zbyte.h"
#include "zbignum.h"
#include "zlist.h"
#include "zdict.h"

#include "zruntime.h"

#include "zcpl_expr.h"

/* TODO:
 * -cpl_dict().
 * -Overflow check in cpl_word().
 * -Hex char (\xhh) in cpl_asciibyte() and cpl_bytearray().
 */

int
is_separator(char c)
{
    if (strchr(" ,)]}:#\n", c) != NULL)
        return 1;
    else
        return 0;
}

void
skip_space(char **str)
{
    while (isspace(**str))
        (*str)++;
}

unsigned int
cpl_word(char **expr, char *bin)
{
    unsigned int w;
    int i;

    w = (unsigned int) strtol(*expr, expr, 0);
    bin[0] = T_WORD;
    for (i = WL / 8; i > 0; i--) {
        bin[i] = (char) w % 256;
        w /= 256;
    }
    return (unsigned int) WL / 8 + 1;
}

char *
escape_char(char e)
{
    switch (e) {
        case 'a':
            return "\a";
        case 'b':
            return "\b";
        case 't':
            return "\t";
        case 'n':
            return "\n";
        case 'v':
            return "\v";
        case 'f':
            return "\f";
        case 'r':
            return "\r";
        case '\"':
            return "\"";
        case '\'':
            return "\'";
        case '\\':
            return "\\";
        default:
            return "\?";
    }
}

unsigned int
cpl_asciibyte(char **expr, char *bin)
{
    bin[0] = T_BYTE;
    if (*(*expr + 1) == '\\') {
        bin[1] = *escape_char(*(*expr + 2));
        *expr += 4;
    }
    else {
        bin[1] = *(*expr + 1);
        *expr += 3;
    }
    return 2U;
}

unsigned int
cpl_bytearray(char **expr, char *bin)
{
    unsigned int w, length = 0;
    int i;
    char *c, *o;

    c = *expr;
    c++;
    while (*c != '"') {
        if (*c == '\\') {
            o = escape_char(*(c + 1));
            strcpy(bin + length + WL / 8 + 1, o);
            length += strlen(o);
            c += 2;
        }
        else {
            bin[length + WL / 8 + 1] = *c;
            length++;
            c++;
        }
    }
    bin[0] = T_YARR;
    for (i = WL / 8, w = length; i > 0; i--, w /= 256)
        bin[i] = (char) w % 256;
    *expr = ++c;
    return (unsigned int) WL / 8 + 1 + length;
}

unsigned int
cpl_bignum(char **expr, char *bin)
{
    char *end;
    ZBigNum *zbignum;
    int i, n;
    unsigned int w, total, wordlen;

    end = strchr(*expr, '!');
    *end = '\0';
    zbnumfromstr(&zbignum, *expr);
    wordlen = zbignum->length / WL;
    if (zbignum->length % WL)
        wordlen++;
    for (n = 0; n < wordlen; n++) {
        for (i = WL / 8, w = zbignum->words[n]; i > 0; i--, w /= 256)
            bin[i + (n + 1) * WL / 8] = (char) w % 256;
    }
    bin[0] = T_BNUM;
    for (i = WL / 8, w = wordlen; i > 0; i--, w /= 256)
        bin[i] = (char) w % 256;
    *end = '!';
    total = (1 + wordlen) * WL / 8 + 1;
    zdelbnum(&zbignum);
    *expr = end + 1;
    return total;
}

unsigned int
cpl_list(char **expr, char *bin)
{
    unsigned int length, total = 2;

    bin[0] = T_LIST;
    bin++;
    if (*(*expr + 1) != ']') {
        while (**expr != ']') {
            (*expr)++;
            skip_space(expr);
            length = cpl_expr(expr, bin);
            skip_space(expr);
            bin += length;
            total += length;
        }
    }
    else
        (*expr)++;
    (*expr)++;
    *bin = '\x00';
    return total;
}

unsigned int
cpl_dict(char **expr, char *bin)
{
    return 0U;
}

unsigned int
cpl_func(char **expr, char *bin)
{
    char *args;
    unsigned int length, total;

    *bin = '\xF0';
    args = strchr(*expr, '(');
    total = (int) (args - *expr);
    strncpy(bin + 1, *expr, total);
    bin += total + 1;
    *expr += total;
    *bin = '\x00';
    bin++;
    if (*(*expr + 1) != ')') {
        while (**expr != ')') {
            (*expr)++;
            skip_space(expr);
            length = cpl_expr(expr, bin);
            skip_space(expr);
            bin += length;
            total += length;
        }
    }
    else
        (*expr)++;
    (*expr)++;
    *bin = '\xF1';
    return total + 3;
}

unsigned int
cpl_expr(char **expr, char *bin)
{
    if (isdigit(**expr) && strncmp(*expr, "0x", 2) != 0) {
            char *d = *expr;

            while (isdigit(*d))
                d++;
            if (*d == '!')
                return cpl_bignum(expr, bin);
            else
                return cpl_word(expr, bin);
        }
    switch (**expr) {
        case '\'':
            return cpl_asciibyte(expr, bin);
        case '"':
            return cpl_bytearray(expr, bin);
        case '[':
            return cpl_list(expr, bin);
        case '{':
            return cpl_dict(expr, bin);
        default:
            /* ZNone | ZBool | ZByte | ZFunc | Name */
            {
                int si;
                char *c;

                for (si = 0, c = *expr; !is_separator(*c); si++, c++);

                if (si == 4 && strncmp(*expr, "NONE", 4) == 0) {
                    bin[0] = T_NONE;
                    *expr += 4;
                    return 1U;
                }
                else if (si == 5 && strncmp(*expr, "FALSE", 5) == 0) {
                    bin[0] = T_BOOL;
                    bin[1] = '\x00';
                    *expr += 5;
                    return 2U;
                }
                else if (si == 4 && strncmp(*expr, "TRUE", 4) == 0) {
                    bin[0] = T_BOOL;
                    bin[1] = '\x01';
                    *expr += 4;
                    return 2U;
                }
                else if (si == 4 && strncmp(*expr, "0x", 2) == 0) {
                    unsigned char c;

                    c = (unsigned char) strtol(*expr, expr, 16);
                    bin[0] = T_BYTE;
                    bin[1] = c;
                    return 2U;
                }
                else {
                    /* ZFunc | Name */
                    char *c = *expr;
                    unsigned int length = 0;

                    while (!is_separator(*c)) {
                        if (*c == '(')
                            return cpl_func(expr, bin);
                        c++;
                        length++;
                    }
                    strncpy(bin, *expr, length);
                    *expr += length;
                    bin[length] = '\x00';
                    return (unsigned int) length + 1;
                }
            }
    }
}
