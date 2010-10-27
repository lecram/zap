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
    Context *context;
    List *tmp;
    unsigned int length;

    context = newcontext();
    context->global = bbuild();
    context->local = newlist();
    tmp = newlist();

    while (1) {
        printf("> ");
        if (fgets(expr, 256, stdin) == NULL)
            break;
        if (strcmp(expr, "exit\n") != 0) {
            expr_entry = expr;
            length = cpl_expr(&expr_entry, bin);
            /* debug_bin(bin, length); */
            bin_entry = bin;
            result = eval(context, tmp, &bin_entry);
            repobj(buffer, result);
            printf("%s\n", buffer);
            emptylist(tmp);
        }
        else
            break;
    }

    dellist(&tmp);
    delcontext(&context);
}

int
run_mod(char *binname)
{
    FILE *fzbc;
    int size;
    char *szbc, *entry;
    Context *context;
    List *tmp = newlist();

    fzbc = fopen(binname, "rb");
    if (fzbc == NULL) {
        raiseOpenFileError(binname);
        exit(EXIT_FAILURE);
    }
    fseek(fzbc, 0L, SEEK_END);
    size = ftell(fzbc);
    fseek(fzbc, 0L, SEEK_SET);
    szbc = (char *) malloc(size * sizeof(char));
    if (szbc == NULL) {
        fclose(fzbc);
        raiseOutOfMemory("run_mod");
        exit(EXIT_FAILURE);
    }
    if (fread(szbc, size, 1, fzbc) == 0) {
        fclose(fzbc);
        raiseOpenFileError(binname);
        exit(EXIT_FAILURE);
    }
    fclose(fzbc);

    context = newcontext();
    context->global = bbuild();
    context->local = newlist();

    entry = szbc;
    run_block(context, tmp, 0, &entry);

    dellist(&tmp);
    delcontext(&context);
    free(szbc);
    szbc = NULL;

    return 0;
}

int
main(int argc, char *argv[])
{
    char *binname, *ext;
    int compile = 0;

    if (argc == 2) {
        ext = strrchr(argv[1], '.');
        if (ext != NULL) {
            if (strcmp(ext, ".zp") == 0)
                compile = 1;
        }
        if (compile) {
            if (cpl_mod(argv[1])) {
                binname = (char *) malloc(strlen(argv[1]) + 5);
                if (binname == NULL) {
                    raiseOutOfMemory("main");
                    return 0;
                }
                strcpy(binname, argv[1]);
                ext = strrchr(binname, '.');
                if (ext != NULL)
                    *ext = '\0';
                strcat(binname, ".zbc");
                run_mod(binname);
                free(binname);
                binname = ext = NULL;
            }
        }
        else
            run_mod(argv[1]);
    }
    else {
        printf("<< zap interpreter >>\n");
        if (argc == 1) {
            printf("\nInteractive mode.\n\n");
            interactive();
        }
    }

    return 0;
}
