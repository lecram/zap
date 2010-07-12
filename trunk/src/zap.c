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
#include "zcpl_mod.h"

#include "zap.h"

void
debug_bin(char *bin, unsigned int length)
{
    unsigned int i;
    unsigned char *c;

    c = (unsigned char *) bin;
    for (i = 0U; i < length; i++, c++)
        printf("0x%02X ", *c);
    printf("\n");
}

void
interactive()
{
    char buffer[1024];
    char expr[256], bin[256];
    char *expr_entry, *bin_entry;
    Zob *result;
    Dict *namespace;
    List *tmp;
    unsigned int length;

    namespace = bbuild();
    tmp = newlist();

    while (1) {
        printf("> ");
        fgets(expr, 256, stdin);
        if (strcmp(expr, "exit\n")) {
            expr_entry = expr;
            length = cpl_expr(&expr_entry, bin);
            /* debug_bin(bin, length); */
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
run_mod(char *binname)
{
    FILE *fzbc;
    int size;
    char *szbc, *entry;
    Space *space;
    List *tmp = newlist();

    fzbc = fopen(binname, "rb");
    if (fzbc == NULL) {
        printf("Error: Cannot open file \"%s\".\n", binname);
        exit(EXIT_FAILURE);
    }
    fseek(fzbc, 0L, SEEK_END);
    size = ftell(fzbc);
    fseek(fzbc, 0L, SEEK_SET);
    szbc = (char *) malloc(size * sizeof(char));
    if (szbc == NULL) {
        raiseOutOfMemory("run_mod");
        exit(EXIT_FAILURE);
    }
    fread(szbc, size, 1, fzbc);
    fclose(fzbc);

    space = newspace();
    space->universal = bbuild();
    space->global = newdict();
    space->local = newdict();

    entry = szbc;
    run_block(space, tmp, 0, &entry);

    dellist(&tmp);
    delspace(&space);
    free(szbc);
    szbc = NULL;

    return 0;
}

int
main(int argc, char *argv[])
{
    if (argc == 1) {
        printf("<< zap interpreter >>\n\nInteractive mode.\n\n");
        interactive();
    }

    if (argc == 2) {
        if (!strcmp(strrchr(argv[1], '.'), ".z"))
            cpl_mod(argv[1]);
        else
            run_mod(argv[1]);
    }

    return 0;
}
