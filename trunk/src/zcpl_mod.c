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
hidekeyed(char *str, char *keyed)
{
    int keying = 0;

    while (*str != '\0') {
        if (keying) {
            if (*str == '}')
                keying = 0;
            else {
                *keyed = *str;
                *str = ' ';
                keyed++;
            }
        }
        else if (*str == '{')
            keying = 1;
        str++;
    }
}

void
showkeyed(char *str, char *keyed)
{
    int keying = 0;

    while (*str != '\0') {
        if (keying) {
            if (*str == '}')
                keying = 0;
            else {
                *str = *keyed;
                keyed++;
            }
        }
        else if (*str == '{')
            keying = 1;
        str++;
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
    if (strlen(str) > 0) {
        tail = str + strlen(str) - 1;
        if (isspace(*tail)) {
            do {
                tail--;
            } while (isspace(*tail) && tail >= str);
            *(tail + 1) = '\0';
        }
    }
}

int
cpl_mod(char *srcname)
{
    FILE *fsrc, *fbin;
    char *binname, *expr_entry, *def, *ext;
    char *colon, *assign, *stt;
    char line[256], quoted[256], keyed[256], bin[256];
    unsigned int length, linum;
    int identlevel, identwidth, ident;

    fsrc = fopen(srcname, "r");
    if (fsrc == NULL) {
        zraiseOpenFileError(srcname);
        return 0;
    }
    binname = (char *) malloc(strlen(srcname) + 5);
    if (binname == NULL) {
        zraiseOutOfMemory("cpl_mod");
        fclose(fsrc);
        return 0;
    }
    strcpy(binname, srcname);
    ext = strrchr(binname, '.');
    if (ext != NULL)
        *ext = '\0';
    strcat(binname, ".zbc");
    fbin = fopen(binname, "wb");
    ext = NULL;
    if (fbin == NULL) {
        zraiseOpenFileError(binname);
        fclose(fsrc);
        free(binname);
        binname = NULL;
        return 0;
    }
    identlevel = 0;
    identwidth = 0;
    for (linum = 1; fgets(line, 256, fsrc) != NULL; linum++) {
        hidequoted(line, quoted);
        remtail(line);
        if (strlen(line) == 0)
            /* Ignore blank lines. */
            continue;
        ident = 0;
        stt = line;
        while (isspace(*stt)) {
            stt++;
            ident++;
        }
        if (identwidth > 0) {
            int level;

            level = ident / identwidth;
            if (level > identlevel) {
                zraisecpl("Incorrect identation.", srcname, linum);
                break;
            }
            for (; level < identlevel; identlevel--)
                /* Compile end of block. */
                fwrite("\xBE\x01", 1, 2, fbin);
        }
        else if (identlevel > 0) {
            /* Define identation width. */
            if (ident == 0) {
                zraisecpl("Incorrect identation.", srcname, linum);
                break;
            }
            else
                identwidth = ident;
        }
        if (strncmp(stt, "del ", 4) == 0) {
            char *name;

            /* Compile del statement. */
            fwrite("\xDE", 1, 1, fbin);
            name = (char *) stt + 4;
            skip_space(&name);
            fwrite(name, 1, strlen(name) + 1, fbin);
            continue;
        }
        else if (strncmp(stt, "break", 5) == 0) {
            if (stt[5] == '\0') {
                /* Compile single break statement. */
                fwrite("\xBE\x02\x00", 1, 3, fbin);
                continue;
            }
            else if (stt[5] == ' ') {
                char *level, blevel;

                /* Compile compound break statement. */
                fwrite("\xBE\x02", 1, 2, fbin);
                level = (char *) stt + 5;
                skip_space(&level);
                blevel = (char) strtol(level, (char **) NULL, 16);
                fwrite(&blevel, 1, 1, fbin);
                continue;
            }
        }
        else if (strncmp(stt, "continue", 8) == 0) {
            if (stt[8] == '\0') {
                /* Compile single continue statement. */
                fwrite("\xBE\x03\x00", 1, 3, fbin);
                continue;
            }
            else if (stt[8] == ' ') {
                char *level, blevel;

                /* Compile compound continue statement. */
                fwrite("\xBE\x03", 1, 2, fbin);
                level = (char *) stt + 8;
                skip_space(&level);
                blevel = (char) strtol(level, (char **) NULL, 16);
                fwrite(&blevel, 1, 1, fbin);
                continue;
            }
        }
        else if (strncmp(stt, "return", 6) == 0) {
            if (stt[6] == '\0') {
                /* Compile return NONE. */
                fwrite("\xBE\x04\x01", 1, 3, fbin);
                continue;
            }
            else if (stt[6] == ' ') {
                /* Compile return statement. */
                fwrite("\xBE\x04", 1, 2, fbin);
                expr_entry = (char *) stt + 6;
                skip_space(&expr_entry);
                showquoted(line, quoted);
                length = cpl_expr(&expr_entry, bin);
                fwrite(bin, 1, length, fbin);
                continue;
            }
        }
        hidekeyed(line, keyed);
        colon = strrchr(stt, ':');
        showkeyed(line, keyed);
        showquoted(line, quoted);
        if (colon == NULL)
            expr_entry = stt;
        else {
            colon += 2;
            if (*colon == '\0') {
                identlevel++;
                fwrite("\xB0", 1, 1, fbin);
                if (strncmp(stt, "while", 5) == 0) {
                    /* Compile while block header. */
                    fwrite("\x04", 1, 1, fbin);
                    expr_entry = (char *) stt + 5;
                    skip_space(&expr_entry);
                    length = cpl_expr(&expr_entry, bin);
                    fwrite(bin, 1, length, fbin);
                    continue;
                }
                else if (strncmp(stt, "if", 2) == 0) {
                    /* Compile if block header. */
                    fwrite("\x01", 1, 1, fbin);
                    expr_entry = (char *) stt + 2;
                    skip_space(&expr_entry);
                    length = cpl_expr(&expr_entry, bin);
                    fwrite(bin, 1, length, fbin);
                    continue;
                }
                else if (strncmp(stt, "elif", 4) == 0) {
                    /* Compile elif block header. */
                    fwrite("\x02", 1, 1, fbin);
                    expr_entry = (char *) stt + 4;
                    skip_space(&expr_entry);
                    length = cpl_expr(&expr_entry, bin);
                    fwrite(bin, 1, length, fbin);
                    continue;
                }
                else if (strncmp(stt, "else", 4) == 0) {
                    /* Compile else block header. */
                    fwrite("\x03", 1, 1, fbin);
                    continue;
                }
                else if (strncmp(stt, "def", 3) == 0) {
                    /* Compile function definition header. */
                    fwrite("\x05", 1, 1, fbin);
                    def = (char *) stt + 3;
                    skip_space(&def);
                    while (*def != '(') {
                        fwrite(def, 1, 1, fbin);
                        def++;
                    }
                    fwrite("\0", 1, 1, fbin);
                    if (*(def + 1) != ')') {
                        while (*def != ')') {
                            def++;
                            skip_space(&def);
                            while (!is_separator(*def)) {
                                fwrite(def, 1, 1, fbin);
                                def++;
                            }
                            fwrite("\0", 1, 1, fbin);
                            skip_space(&def);
                        }
                    }
                    fwrite("\0", 1, 1, fbin);
                    continue;
                }
            }
            else {
                expr_entry = colon;
                skip_space(&expr_entry);
            }
        }
        length = cpl_expr(&expr_entry, bin);
        fwrite(bin, 1, length, fbin);
        if (colon == NULL)
            /* No Assignment. */
            fwrite("\0", 1, 1, fbin);
        else {
            assign = stt;
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
    while (identlevel >= 0) {
        fwrite("\xBE\x01", 1, 2, fbin);
        identlevel--;
    }
    fclose(fbin);
    free(binname);
    binname = NULL;

    return 1;
}
