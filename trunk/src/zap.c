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

/* Interpreter */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ztypes.h"
#include "zerr.h"

#include "zlist.h"
#include "zdict.h"

#include "zobject.h"
#include "zruntime.h"
#include "zbuiltin.h"

#include "zcpl_expr.h"

#include "zap.h"

void
interactive()
{
    char buffer[1024];
    char expr[256], bin[256];
    char *expr_entry, *bin_entry;
    Zob *result;
    Dict *namespace;
    List *tmp;

    namespace = bbuild();
    tmp = newlist();

    while (1) {
        printf("> ");
        fgets(expr, 256, stdin);
        if (strcmp(expr, "exit\n")) {
            expr_entry = expr;
            cpl_expr(&expr_entry, bin);
            bin_entry = bin;
            result = eval(namespace, tmp, &bin_entry);
            repobj(buffer, result);
            printf("%s\n", buffer);
            emptylist(tmp);
        }
        else
            break;
    }

    dellist(&tmp);
    deldict(&namespace);
}

int
main(int argc, char *argv[])
{
    FILE *fbbc;
    int size;
    char *sbbc, *entry;
    Space *space;
    List *tmp = newlist();

    if (argc == 1) {
        printf("<< zap interpreter >>\n\nInteractive mode.\n\n");
        interactive();
    }

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
    if (sbbc == NULL) {
        raiseOutOfMemory("main");
        exit(EXIT_FAILURE);
    }
    fread(sbbc, size, 1, fbbc);
    fclose(fbbc);

    space = newspace();
    space->universal = bbuild();
    space->global = newdict();
    space->local = newdict();

    entry = sbbc;
    run_block(space, tmp, 0, &entry);

    dellist(&tmp);
    delspace(&space);
    free(sbbc);
    sbbc = NULL;
    return 0;
}
