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
zdebug_bin(char *bin, unsigned int length)
{
    unsigned int i;
    unsigned char *c;

    c = (unsigned char *) bin;
    for (i = 0U; i < length; i++, c++)
        printf("0x%02X ", *c);
    puts("");
}

ZError
zinteractive()
{
    char buffer[1024];
    char expr[256], bin[256];
    char *expr_entry, *bin_entry;
    Zob *result;
    ZContext *zcontext;
    ZList *tmp;
    unsigned int length;
    ZError err;

    err = znewcontext(&zcontext);
    if (err != ZE_OK)
        return err;
    err = zbuild(&zcontext->global);
    if (err != ZE_OK) {
        zdelcontext(&zcontext);
        return err;
    }
    err = znewlist(&zcontext->local);
    if (err != ZE_OK) {
        zdelcontext(&zcontext);
        return err;
    }
    err = znewlist(&tmp);
    if (err != ZE_OK) {
        zdelcontext(&zcontext);
        return err;
    }

    while (1) {
        printf("> ");
        if (fgets(expr, 256, stdin) == NULL)
            break;
        if (strcmp(expr, "exit\n") != 0) {
            expr_entry = expr;
            length = cpl_expr(&expr_entry, bin);
            /* zdebug_bin(bin, length); */
            bin_entry = bin;
            err = zeval(zcontext, tmp, &bin_entry, &result);
            if (err != ZE_OK) {
                zdelcontext(&zcontext);
                zdellist(&tmp);
                return err;
            }
            zrepobj(buffer, 1024, result);
            printf("%s\n", buffer);
            zlempty(tmp);
        }
        else
            break;
    }

    zdellist(&tmp);
    zdelcontext(&zcontext);
    return ZE_OK;
}

ZError
zrun_mod(char *binname)
{
    FILE *fzbc;
    int size;
    char *szbc, *entry;
    ZContext *zcontext;
    ZList *tmp;
    unsigned char be;
    ZError err;

    err = znewlist(&tmp);
    if (err != ZE_OK)
        return err;
    fzbc = fopen(binname, "rb");
    if (fzbc == NULL) {
        zdellist(&tmp);
        return ZE_OPEN_FILE_ERROR;
    }
    fseek(fzbc, 0L, SEEK_END);
    size = ftell(fzbc);
    fseek(fzbc, 0L, SEEK_SET);
    szbc = (char *) malloc(size * sizeof(char));
    if (szbc == NULL) {
        zdellist(&tmp);
        fclose(fzbc);
        return ZE_OUT_OF_MEMORY;
    }
    if (fread(szbc, (size_t) size, 1, fzbc) == 0) {
        zdellist(&tmp);
        fclose(fzbc);
        free(szbc);
        szbc = NULL;
        return ZE_OPEN_FILE_ERROR;
    }
    fclose(fzbc);

    err = znewcontext(&zcontext);
    if (err != ZE_OK) {
        zdellist(&tmp);
        free(szbc);
        szbc = NULL;
        return err;
    }
    err = zbuild(&zcontext->global);
    if (err != ZE_OK) {
        zdellist(&tmp);
        free(szbc);
        szbc = NULL;
        zdelcontext(&zcontext);
        return err;
    }
    err = znewlist(&zcontext->local);
    if (err != ZE_OK) {
        zdellist(&tmp);
        free(szbc);
        szbc = NULL;
        zdelcontext(&zcontext);
        return err;
    }

    entry = szbc;
    be = 0;
    err = zrun_block(zcontext, tmp, 0, &entry, &be);
    if (err != ZE_OK) {
        zdellist(&tmp);
        free(szbc);
        szbc = NULL;
        zdelcontext(&zcontext);
        return err;
    }

    zdellist(&tmp);
    free(szbc);
    szbc = NULL;
    zdelcontext(&zcontext);

    return 0;
}

int
main(int argc, char *argv[])
{
    char *binname, *ext;
    int compile = 0;
    ZError err = ZE_OK;

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
                    zraiseOutOfMemory("main");
                    return EXIT_FAILURE;
                }
                strcpy(binname, argv[1]);
                ext = strrchr(binname, '.');
                if (ext != NULL)
                    *ext = '\0';
                strcat(binname, ".zbc");
                err = zrun_mod(binname);
                free(binname);
                binname = ext = NULL;
            }
        }
        else
            err = zrun_mod(argv[1]);
    }
    else {
        puts("<< zap interpreter >>");
        if (argc == 1) {
            puts("\nInteractive mode.\n");
            err = zinteractive();
        }
    }

    switch (err) {
        case ZE_OK:
            return EXIT_SUCCESS;
        case ZE_OUT_OF_MEMORY:
            puts("ZE_OUT_OF_MEMORY");
            return EXIT_FAILURE;
        case ZE_INDEX_OUT_OF_RANGE:
            puts("ZE_INDEX_OUT_OF_RANGE");
            return EXIT_FAILURE;
        case ZE_NAME_NOT_DEFINED:
            puts("ZE_NAME_NOT_DEFINED");
            return EXIT_FAILURE;
        case ZE_FUNCTION_NAME_NOT_DEFINED:
            puts("ZE_FUNCTION_NAME_NOT_DEFINED");
            return EXIT_FAILURE;
        case ZE_ARITY_ERROR:
            puts("ZE_ARITY_ERROR");
            return EXIT_FAILURE;
        case ZE_BREAK_WITHOUT_LOOP:
            puts("ZE_BREAK_WITHOUT_LOOP");
            return EXIT_FAILURE;
        case ZE_CONTINUE_WITHOUT_LOOP:
            puts("ZE_CONTINUE_WITHOUT_LOOP");
            return EXIT_FAILURE;
        case ZE_UNKNOWN_TYPE_NUMBER:
            puts("ZE_UNKNOWN_TYPE_NUMBER");
            return EXIT_FAILURE;
        case ZE_OPEN_FILE_ERROR:
            puts("ZE_OPEN_FILE_ERROR");
            return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
