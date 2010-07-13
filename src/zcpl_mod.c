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

/* Module Compiler */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "zcpl_expr.h"

void
hidequoted(char *str, char *quoted)
{
    char *s = str, *q = quoted;
    int quoting = 0;
    char qtype = '\0';

    while (*s != '\0') {
        if (quoting) {
            if (*s == qtype)
                quoting = 0;
            else if (*s == '\\') {
                *q = '\\';
                *s = ' ';
                s++;
                q++;
                *q = *s;
                *s = ' ';
                q++;
            }
            else {
                *q = *s;
                *s = ' ';
                q++;
            }
        }
        else if (*s == '"' || *s == '\'') {
                quoting = 1;
                qtype = *s;
        }
        s++;
    }
}

void
showquoted(char *str, char *quoted)
{
    char *s = str, *q = quoted;
    int quoting = 0;
    char qtype = '\0';

    while (*s != '\0') {
        if (quoting) {
            if (*s == qtype)
                quoting = 0;
            else {
                *s = *q;
                q++;
            }
        }
        else if (*s == '"' || *s == '\'') {
                quoting = 1;
                qtype = *s;
        }
        s++;
    }
}

int
cpl_mod(char *srcname)
{
    FILE *fsrc, *fbin;
    char *binname, *expr_entry;
    char line[256], quoted[256], bin[256];
    unsigned int length;

    fsrc = fopen(srcname, "r");
    if (fsrc == NULL) {
        printf("Error: Cannot open file \"%s\".\n", srcname);
        return 0;
    }
    binname = (char *) malloc(strlen(srcname) + 4);
    if (binname == NULL) {
        printf("Error: Out of memory.\n");
        fclose(fsrc);
        return 0;
    }
    strcpy(binname, srcname);
    strcat(binname, "bc_");
    fbin = fopen(binname, "wb");
    if (fbin == NULL) {
        printf("Error: Cannot open file \"%s\".\n", binname);
        fclose(fsrc);
        free(binname);
        binname = NULL;
        return 0;
    }
    while (fgets(line, 256, fsrc) != NULL) {
        hidequoted(line, quoted);
        if (strchr(line, ':') != NULL)
            break;
        showquoted(line, quoted);
        expr_entry = line;
        length = cpl_expr(&expr_entry, bin);
        fwrite(bin, 1, length, fbin);
        /* No Assignment. */
        fwrite("\0", 1, 1, fbin);
    }
    fclose(fsrc);
    /* Block End. */
    fwrite("\xBE\x01", 1, 2, fbin);
    fclose(fbin);
    free(binname);
    binname = NULL;

    return 1;
}
