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

#include <ztypes.h>

#include <zbyte.h>
#include <zbytearray.h>
#include <zlist.h>

int
cpl_mod(char *srcname)
{
    FILE *fsrc, *fbin;
    char *binname, line[256];
    List *lines = newlist();
    Node *lineyarr;

    fsrc = fopen(srcname, "r");
    if (fsrc == NULL) {
        printf("Error: Cannot open file \"%s\".\n", srcname);
        return 0;
    }
    while (fgets(line, 256, fsrc) != NULL)
        appitem(lines, (Zob *) yarrfromstr(line));
    fclose(fsrc);
    binname = (char *) malloc(strlen(srcname) + 3);
    if (binname == NULL) {
        printf("Error: Out of memory.\n");
        return 0;
    }
    strcpy(binname, srcname);
    strcat(binname, "bc_");
    fbin = fopen(binname, "wb");
    fclose(fbin);
    printf("%u lines read from %s.\n\n", lines->length, srcname);
    lineyarr = lines->first;
    while (lineyarr != NULL) {
        printf("%s", ((ByteArray *) lineyarr->object)->bytes);
        lineyarr = lineyarr->next;
    }

    free(binname);
    binname = NULL;

    return 1;
}
