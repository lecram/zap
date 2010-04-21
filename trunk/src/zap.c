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
 * Interpreter
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "zap.h"
#include "zeval.h"
#include "zbuiltin.h"
#include "ztruth.h"

Space *
newspace()
{
    Space *space;

    space = (Space *) malloc(sizeof(Space));
    if (space == NULL)
        raise("Out of memory in newspace().");
    return space;
}

void
delspace(Space **space)
{
    delobj(&(*space)->universal);
    delobj(&(*space)->global);
    delobj(&(*space)->local);
    free(*space);
    *space = NULL;
}

void
skip_assign(char **entry)
{
    char *cursor = *entry;

    while (*cursor)
        cursor += strlen(cursor) + 1; /* To skip NAME_END. */
    cursor++; /* To skip ASSIGN_END. */
    *entry = cursor;
}

void
assign(Dict *dict, Zob *value, char **entry)
{
    ByteArray *key;
    char *cursor = *entry;

    while (*cursor) {
        key = yarrfromstr(cursor);
        cursor += strlen(cursor) + 1; /* To skip NAME_END. */
        setkey(dict, (Zob *) key, value);
        /* increfc(value); */
    }
    cursor++; /* To skip ASSIGN_END. */
    *entry = cursor;
}

void
runstatement(Space *space, List *tmp, char **entry)
{
    Zob *value;

    value = eval((Dict *) space->universal, tmp, &(*entry));
    assign((Dict *) space->universal, value, &(*entry));
}

void
skip_block(char **entry)
{
    char *cursor = *entry;

    while (*cursor != (char) BLOCKEXIT  ||  *(cursor + 1) != (char) END) {
        if (*cursor == (char) BLOCKEXIT) {
            cursor++;
            if (*cursor == (char) BREAK || *cursor == (char) CONTINUE)
                cursor += 2;
            else if (*cursor == (char) RETURN) {
                /* Function Return. */
                /* NYI. */
            }
        }
        else if (*cursor == (char) DELETE) {
            cursor++;
            cursor += strlen(cursor) + 1;
        }
        else if (*cursor == (char) BLOCK) {
            cursor++;
            if (*cursor == (char) IF) {
                cursor++;
                skip_expr(&cursor);
                skip_block(&cursor);
            }
            while (*cursor == (char) BLOCK  &&  *(cursor + 1) == (char) ELIF) {
                cursor += 2;
                skip_expr(&cursor);
                skip_block(&cursor);
            }
            if (*cursor == (char) BLOCK  &&  *(cursor + 1) == (char) ELSE) {
                cursor += 2;
                skip_block(&cursor);
            }
            else if (*cursor == (char) WHILE) {
                cursor++;
                skip_expr(&cursor);
                skip_block(&cursor);
            }
            else if (*cursor == (char) DEF) {
                /* Function definition. */
                /* NYI. */
            }
        }
        else {
            /* Statement. */
            skip_expr(&cursor);
            skip_assign(&cursor);
        }
    }
    cursor += 2;
    *entry = cursor;
}

unsigned char
run_block(Space *space, List *tmp, char looplev, char **entry)
{
    char *cursor = *entry;
    Dict *namespace = (Dict *) space->universal;
    Bool *truth;
    unsigned char be;

    while (*cursor != (char) BLOCKEXIT) {
        if (*cursor == (char) DELETE) {
            ByteArray *key;

            cursor++;
            key = yarrfromstr(cursor);
            if (haskey(namespace, (Zob *) key))
                remkey(namespace, (Zob *) key);
            else {
                char errbff[256];

                sprintf(errbff,
                        "del: Name not defined: %s.",
                        cursor);
                raise(errbff);
            }
            delyarr(&key);
            cursor += strlen(cursor) + 1;
        }
        else if (*cursor == (char) BLOCK) {
            cursor++;
            if (*cursor == (char) IF) {
                int ok = 0;

                cursor++;
                truth = objtruth(eval(namespace, tmp, &cursor));
                if (truth->value) {
                    be = run_block(space, tmp, looplev, &cursor);
                    if (be & (BE_BREAK | BE_CONTINUE | BE_RETURN))
                        return be;
                    ok = 1;
                }
                else
                    skip_block(&cursor);
                while (*cursor == (char) BLOCK  &&  *(cursor + 1) == (char) ELIF) {
                    cursor += 2;
                    if (ok) {
                        skip_expr(&cursor);
                        skip_block(&cursor);
                    }
                    else {
                        truth = objtruth(eval(namespace, tmp, &cursor));
                        if (truth->value) {
                            be = run_block(space, tmp, looplev, &cursor);
                            if (be & (BE_BREAK | BE_CONTINUE | BE_RETURN))
                                return be;
                            ok = 1;
                        }
                        else
                            skip_block(&cursor);
                    }
                }
                if (*cursor == (char) BLOCK  &&  *(cursor + 1) == (char) ELSE) {
                    cursor += 2;
                    if (ok)
                        skip_block(&cursor);
                    else {
                        be = run_block(space, tmp, looplev, &cursor);
                        if (be & (BE_BREAK | BE_CONTINUE | BE_RETURN))
                            return be;
                    }
                }
            }
            else if (*cursor == (char) WHILE) {
                char *cond, *block, *blockend = NULL;
                char *b, *c;

                cursor++;
                cond = cursor;
                skip_expr(&cursor);
                block = cursor;
                c = cond;
                truth = objtruth(eval(namespace, tmp, &c));
                while (truth->value) {
                    b = block;
                    be = run_block(space, tmp, looplev + 1, &b);
                    if (be & BE_RETURN)
                        return be;
                    if (be & BE_BREAK) {
                        if (be - BE_BREAK > 0)
                            /* Propagate. */
                            return be - 1;
                        break;
                    }
                    blockend = b;
                    c = cond;
                    truth = objtruth(eval(namespace, tmp, &c));
                    if (be & BE_CONTINUE)
                        if (!(truth->value))
                            if (be - BE_CONTINUE > 0)
                                /* Propagate. */
                                return be - 1;
                }
                if (blockend == NULL)
                    skip_block(&cursor);
                else
                    cursor = blockend;
            }
            else if (*cursor == (char) DEF) {
                /* Function definition. */
                /* NYI. */
            }
        }
        else {
            /* Statement. */
            runstatement(space, tmp, &cursor);
            /* Garbage Collection. */
            emptylist(tmp);
        }
    }
    cursor++;
    if (*cursor == (char) END) {
        cursor++;
        *entry = cursor;
        return BE_END;
    }
    if (*cursor == (char) BREAK) {
        char lev;

        cursor++;
        lev = *cursor;
        if (lev > looplev)
            raise("Break without loop.");
        return BE_BREAK | lev;
    }
    if (*cursor == (char) CONTINUE) {
        char lev;

        cursor++;
        lev = *cursor;
        if (lev > looplev)
            raise("Continue without loop.");
        return CONTINUE | lev;
    }
    if (*cursor == (char) RETURN) {
        /* Function Return. */
        /* NYI. */
    }
    return 0;
}

int
main(int argc, char *argv[])
{
    FILE *fbbc;
    int size;
    char *sbbc;
    Space *space;
    List *tmp = newlist();

    if (argc == 1)
        printf("<< zap interpreter >>\n\nUsage:\nzap app.zbc\n");

    if (argc != 2)
        return 0;

    fbbc = fopen(argv[1], "rb");
    if (fbbc == NULL) {
        printf("Error: Cannot open file \"%s\".\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    fseek(fbbc, 0L, SEEK_END);
    size = ftell(fbbc);
    fseek(fbbc, 0L, SEEK_SET);
    sbbc = (char *) malloc(size * sizeof(char));
    if (sbbc == NULL)
        raise("Out of memory in main().");
    fread(sbbc, size, 1, fbbc);
    fclose(fbbc);

    space = newspace();
    space->universal = (Zob *) bbuild();
    space->global = (Zob *) newnone();
    space->local = (Zob *) newnone();

    run_block(space, tmp, 0, &sbbc);

    dellist(&tmp);
    delspace(&space);
    free(sbbc);
    sbbc = NULL;
    return 0;
}
