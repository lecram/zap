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

/* Expression Compiler */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "ztypes.h"
#include "zerr.h"

#include "zbyte.h"
#include "zbignum.h"
#include "zlist.h"
#include "znametable.h"
#include "zdict.h"

#include "zruntime.h"

#include "zcpl_expr.h"

/* TODO:
 * -Overflow check in cpl_int().
 * -Hex char (\xhh) in cpl_asciibyte() and cpl_bytearray().
 */

int
is_separator(char c)
{
    return strchr(" ,)]}:#\n", c) != NULL;
}

void
skip_space(char **str)
{
    while (isspace(**str))
        (*str)++;
}

unsigned int
write_svlv(int n, signed char vlv[])
{
    int q, r, an;
    unsigned int i, j, k;
    signed char t;

    i = 0;
    an = n;
    an *= (n < 0) ? -1 : 1;
    q = an / 128;
    r = an % 128;
    vlv[0] = r;
    while (q != 0) {
        i++;
        an = q;
        q = an / 128;
        r = (an % 128) | 128;
        vlv[i] = r;
    }
    i++;
    vlv[i] = (n < 0) ? -1 : 1;
    for (j = 0; j < (i + 1) / 2; j++) {
        k = i - j;
        t = vlv[j];
        vlv[j] = vlv[k];
        vlv[k] = t;
    }
    return i + 1;
}

unsigned int
cpl_int(char **expr, char *bin)
{
    int i;

    i = (int) strtol(*expr, expr, 0);
    bin[0] = T_INT;
    return write_svlv(i, (signed char *) bin + 1) + 1;
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
    (*expr)++;
    while (**expr != ']') {
        length = cpl_expr(expr, bin);
        skip_space(expr);
        bin += length;
        total += length;
    }
    (*expr)++;
    *bin = '\x00';
    return total;
}

unsigned int
cpl_dict(char **expr, char *bin)
{
    unsigned int length, total = 2;

    bin[0] = T_DICT;
    bin++;
    (*expr)++;
    while (**expr != '}') {
        length = cpl_expr(expr, bin);
        bin += length;
        total += length;
        (*expr)++; /* ':' */
        length = cpl_expr(expr, bin);
        skip_space(expr);
        bin += length;
        total += length;
    }
    (*expr)++;
    *bin = '\x00';
    return total;
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
    (*expr)++;
    while (**expr != ')') {
        length = cpl_expr(expr, bin);
        skip_space(expr);
        bin += length;
        total += length;
    }
    (*expr)++;
    *bin = '\xF1';
    return total + 3;
}

unsigned int
cpl_expr(char **expr, char *bin)
{
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
            /* None | Bool | Byte | BigNum | Word | Func | Name */
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
                else if (isdigit(**expr) ||
                         (**expr == '-' && isdigit(*(*expr + 1)))) {
                    char *d = *expr;

                    if (*d == '-')
                        d++;
                    while (isdigit(*d))
                        d++;
                    if (*d == '!')
                        return cpl_bignum(expr, bin);
                    else
                        return cpl_int(expr, bin);
                }
                else {
                    /* Func | Name */
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

int
splitstt(char *input, char *output, char *parts[])
{
    int on = 0;
    int depth = 0;
    int length = 0;
    int squoting = 0;
    int dquoting = 0;

    while(*input != '\0') {
        if (on) {
            if (isspace(*input) && !depth && !squoting && !dquoting) {
                on = 0;
                *output = '\0';
                depth = 0;
            }
            else {
                *output = *input;
                if (strchr("([{", *input) != NULL)
                    depth++;
                else if (strchr(")]}", *input) != NULL)
                    depth--;
                else if (*input == '\'')
                    squoting = 1 - squoting;
                else if (*input == '\"')
                    dquoting = 1 - dquoting;
            }
            output++;
        }
        else if (!isspace(*input)) {
            on = 1;
            parts[length] = output;
            *output = *input;
            if (strchr("([{", *input) != NULL)
                depth++;
            else if (*input == '\'')
                squoting = 1 - squoting;
            else if (*input == '\"')
                dquoting = 1 - dquoting;
            output++;
            length++;
        }
        input++;
    }
    *output = '\0';
    *(output + 1) = '\0';
    return length;
}

unsigned int
cpl_stt(char **pstt, char *bin)
{
    char *expr_entry;
    char *assign, *stt;
    char splitbuffer[256];
    char *parts[16];
    unsigned int length, total = 0;
    int splitlen;

    stt = *pstt;
    splitlen = splitstt(stt, splitbuffer, parts);
    expr_entry = parts[splitlen - 1];
    length = cpl_expr(&expr_entry, bin);
    bin += length;
    total += length;
    for (splitlen -= 1; splitlen > 0; splitlen--) {
        /* Compile Assignments. */
        assign = parts[splitlen - 1];
        if (*assign == '(') {
            int depth = 1;
            int namelen;
            char *namechar;

            *bin = '\x10';
            bin++;
            total++;
            assign++;
            while (depth > 0) {
                skip_space(&assign);
                if (*assign == '(') {
                    *bin = '\x10';
                    bin++;
                    total++;
                    depth++;
                    assign++;
                    continue;
                }
                namechar = assign;
                namelen = 0;
                while (!isspace(*namechar) && *namechar != ')') {
                    namechar++;
                    namelen++;
                }
                strncpy(bin, assign, namelen);
                bin += namelen;
                total += namelen;
                *bin = '\0';
                bin++;
                total++;
                assign += namelen;
                skip_space(&assign);
                while (*assign == ')') {
                    *bin = '\x01';
                    bin++;
                    total++;
                    depth--;
                    assign++;
                    skip_space(&assign);
                }
            }
        }
        else {
            strncpy(bin, assign, strlen(assign) + 1);
            bin += strlen(assign) + 1;
            total += strlen(assign) + 1;
        }
    }
    *bin = '\0';
    bin++;
    total++;
    return total;
}
