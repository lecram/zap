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
    char quoted[256];

    hidequoted(str, quoted);
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
    showquoted(str, quoted);
}

int
cpl_mod(char *srcname)
{
    FILE *fsrc, *fbin;
    char *binname, *expr_entry, *def, *ext;
    char *assign, *stt;
    char line[256], bin[256], splitbuffer[256];
    char *parts[16];
    unsigned int length, linum;
    int identlevel, identwidth, ident, splitlen;

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
            if (ident % identwidth != 0) {
                zraisecpl("Incorrect identation.", srcname, linum);
                break;
            }
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
        splitlen = splitstt(stt, splitbuffer, parts);
        expr_entry = parts[splitlen - 1];
        if (*parts[0] == '\\') {
            if (strcmp(parts[0], "\\del") == 0) {
                /* Compile del statement. */
                fwrite("\xDE", 1, 1, fbin);
                for (splitlen -= 1; splitlen > 0; splitlen--) {
                    char *name = parts[splitlen];

                    fwrite(name, 1, strlen(name) + 1, fbin);
                }
                fwrite("\0", 1, 1, fbin);
            }
            else if (strcmp(parts[0], "\\break") == 0) {
                if (splitlen == 1) {
                    /* Compile single break statement. */
                    fwrite("\xBE\x02\x00", 1, 3, fbin);
                }
                else {
                    char blevel;

                    /* Compile compound break statement. */
                    fwrite("\xBE\x02", 1, 2, fbin);
                    blevel = (char) strtol(parts[1], (char **) NULL, 16);
                    fwrite(&blevel, 1, 1, fbin);
                }
            }
            else if (strcmp(parts[0], "\\cont") == 0) {
                if (splitlen == 1) {
                    /* Compile single continue statement. */
                    fwrite("\xBE\x03\x00", 1, 3, fbin);
                }
                else {
                    char blevel;

                    /* Compile compound continue statement. */
                    fwrite("\xBE\x03", 1, 2, fbin);
                    blevel = (char) strtol(parts[1], (char **) NULL, 16);
                    fwrite(&blevel, 1, 1, fbin);
                }
            }
            else if (strcmp(parts[0], "\\ret") == 0) {
                if (splitlen == 1) {
                    /* Compile return NONE. */
                    fwrite("\xBE\x04\x01", 1, 3, fbin);
                }
                else {
                    /* Compile return statement. */
                    fwrite("\xBE\x04", 1, 2, fbin);
                    length = cpl_expr(&expr_entry, bin);
                    fwrite(bin, 1, length, fbin);
                }
            }
            else if (strcmp(parts[0], "\\while") == 0) {
                /* Compile while block header. */
                identlevel++;
                fwrite("\xB0\x04", 1, 2, fbin);
                length = cpl_expr(&expr_entry, bin);
                fwrite(bin, 1, length, fbin);
            }
            else if (strcmp(parts[0], "\\if") == 0) {
                /* Compile if block header. */
                identlevel++;
                fwrite("\xB0\x01", 1, 2, fbin);
                length = cpl_expr(&expr_entry, bin);
                fwrite(bin, 1, length, fbin);
            }
            else if (strcmp(parts[0], "\\elif") == 0) {
                /* Compile elif block header. */
                identlevel++;
                fwrite("\xB0\x02", 1, 2, fbin);
                length = cpl_expr(&expr_entry, bin);
                fwrite(bin, 1, length, fbin);
            }
            else if (strcmp(parts[0], "\\else") == 0) {
                /* Compile else block header. */
                identlevel++;
                fwrite("\xB0\x03", 1, 2, fbin);
            }
            else if (strcmp(parts[0], "\\def") == 0) {
                /* Compile function definition header. */
                identlevel++;
                fwrite("\xB0\x05", 1, 2, fbin);
                def = parts[1];
                while (*def != '(') {
                    fwrite(def, 1, 1, fbin);
                    def++;
                }
                fwrite("\0", 1, 1, fbin);
                def++;
                while (*def != ')') {
                    skip_space(&def);
                    while (!is_separator(*def)) {
                        fwrite(def, 1, 1, fbin);
                        def++;
                    }
                    fwrite("\0", 1, 1, fbin);
                    skip_space(&def);
                }
                fwrite("\0", 1, 1, fbin);
            }
            else {
                zraisecpl("Unknown instruction.", srcname, linum);
                break;
            }
        }
        else {
            length = cpl_expr(&expr_entry, bin);
            fwrite(bin, 1, length, fbin);
            for (splitlen -= 1; splitlen > 0; splitlen--) {
                /* Compile Assignments. */
                assign = parts[splitlen - 1];
                if (*assign == '(') {
                    int depth = 1;
                    int namelen;
                    char *namechar;

                    fwrite("\x10", 1, 1, fbin);
                    assign++;
                    while (depth > 0) {
                        skip_space(&assign);
                        if (*assign == '(') {
                            fwrite("\x10", 1, 1, fbin);
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
                        fwrite(assign, 1, namelen, fbin);
                        fwrite("\0", 1, 1, fbin);
                        assign += namelen;
                        skip_space(&assign);
                        while (*assign == ')') {
                            fwrite("\x01", 1, 1, fbin);
                            depth--;
                            assign++;
                            skip_space(&assign);
                        }
                    }
                }
                else
                    fwrite(assign, 1, strlen(assign) + 1, fbin);
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
