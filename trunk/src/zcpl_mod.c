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
#include <ctype.h>

#include "zerr.h"

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

void
remtail(char *str)
{
    char *tail;

    /* Remove comments. */
    tail = strchr(str, '#');
    if (tail != NULL)
        *tail = '\0';
    /* Remove trailing whitespaces. */
    tail = str + strlen(str) - 1;
    while (isspace(*tail))
        tail--;
    *(tail + 1) = '\0';
}

int
cpl_mod(char *srcname)
{
    FILE *fsrc, *fbin;
    char *binname, *expr_entry, *colon, *assign;
    char line[256], quoted[256], bin[256];
    unsigned int length, linum;

    fsrc = fopen(srcname, "r");
    if (fsrc == NULL) {
        raiseOpenFileError(srcname);
        return 0;
    }
    binname = (char *) malloc(strlen(srcname) + 4);
    if (binname == NULL) {
        raiseOutOfMemory("cpl_mod");
        fclose(fsrc);
        return 0;
    }
    strcpy(binname, srcname);
    strcat(binname, "bc_");
    fbin = fopen(binname, "wb");
    if (fbin == NULL) {
        raiseOpenFileError(binname);
        fclose(fsrc);
        free(binname);
        binname = NULL;
        return 0;
    }
    for (linum = 1; fgets(line, 256, fsrc) != NULL; linum++) {
        hidequoted(line, quoted);
        remtail(line);
        /* Ignore blank lines. */
        if (strlen(line) == 0)
            continue;
        if (strncmp(line, "del ", 4) == 0) {
            char *name;

            /* Compile del statement. */
            fwrite("\xDE", 1, 1, fbin);
            name = (char *) line + 4;
            skip_space(&name);
            fwrite(name, 1, strlen(name) + 1, fbin);
            continue;
        }
        else if (strncmp(line, "break", 5) == 0) {
            if (line[5] == '\0') {
                /* Compile single break statement. */
                fwrite("\xBE\x02\x00", 1, 3, fbin);
                continue;
            }
            else if (line[5] == ' ') {
                char *level, blevel;

                /* Compile compound break statement. */
                fwrite("\xBE\x02", 1, 2, fbin);
                level = (char *) line + 5;
                skip_space(&level);
                blevel = (char) strtol(level, (char **) NULL, 16);
                fwrite(&blevel, 1, 1, fbin);
                continue;
            }
        }
        else if (strncmp(line, "continue", 8) == 0) {
            if (line[8] == '\0') {
                /* Compile single continue statement. */
                fwrite("\xBE\x03\x00", 1, 3, fbin);
                continue;
            }
            else if (line[8] == ' ') {
                char *level, blevel;

                /* Compile compound continue statement. */
                fwrite("\xBE\x03", 1, 2, fbin);
                level = (char *) line + 8;
                skip_space(&level);
                blevel = (char) strtol(level, (char **) NULL, 16);
                fwrite(&blevel, 1, 1, fbin);
                continue;
            }
        }
        colon = strrchr(line, ':');
        if (colon == NULL)
            expr_entry = line;
        else {
            colon += 2;
            if (*colon == '\0')
                /* Blocks NYI. */
                break;
            else {
                expr_entry = colon;
                skip_space(&expr_entry);
            }
        }
        showquoted(line, quoted);
        length = cpl_expr(&expr_entry, bin);
        fwrite(bin, 1, length, fbin);
        if (colon == NULL)
            /* No Assignment. */
            fwrite("\0", 1, 1, fbin);
        else {
            assign = line;
            while (assign != colon) {
                skip_space(&assign);
                while (!isspace(*assign)) {
                    fwrite(assign, 1, 1, fbin);
                    assign++;
                }
                skip_space(&assign);
                assign += 2;
                fwrite("\0", 1, 1, fbin);
            }
            fwrite("\0", 1, 1, fbin);
        }
    }
    fclose(fsrc);
    /* Block End. */
    fwrite("\xBE\x01", 1, 2, fbin);
    fclose(fbin);
    free(binname);
    binname = NULL;

    return 1;
}
