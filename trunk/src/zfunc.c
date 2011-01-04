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

/* ZFunc Type */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ztypes.h"
#include "zerr.h"

#include "zlist.h"
#include "zfunc.h"

/* Create a new ZLowFunc in 'zlowfunc'.
 * If there is not enough memory, return ZE_OUT_OF_MEMORY.
 * Otherwise, return ZE_OK.
 */
ZError
znewlowfunc(ZLowFunc **zlowfunc)
{
    *zlowfunc = (ZLowFunc *) malloc(sizeof(ZLowFunc));
    if (*zlowfunc == NULL)
        return ZE_OUT_OF_MEMORY;
    (*zlowfunc)->high = 0;
    return ZE_OK;
}

/* Remove 'zlowfunc' from memory. */
void
zdellowfunc(ZLowFunc **zlowfunc)
{
    free(*zlowfunc);
    *zlowfunc = NULL;
}

/* Create a new ZLowFunc in 'zlowfunc'.
 * If there is not enough memory, return ZE_OUT_OF_MEMORY.
 * Otherwise, return ZE_OK.
 */
ZError
znewhighfunc(ZHighFunc **zhighfunc)
{
    *zhighfunc = (ZHighFunc *) malloc(sizeof(ZHighFunc));
    if (*zhighfunc == NULL)
        return ZE_OUT_OF_MEMORY;
    (*zhighfunc)->high = 1;
    return ZE_OK;
}

/* Remove 'zhighfunc' from memory. */
void
zdelhighfunc(ZHighFunc **zhighfunc)
{
    free(*zhighfunc);
    *zhighfunc = NULL;
}

/* Create a new ZFunc in 'zfunc'.
 * If there is not enough memory, return ZE_OUT_OF_MEMORY.
 * Otherwise, return ZE_OK.
 */
ZError
znewfunc(ZFunc **zfunc, FImp *fimp, unsigned char arity)
{
    *zfunc = (ZFunc *) malloc(sizeof(ZFunc));
    if (*zfunc == NULL)
        return ZE_OUT_OF_MEMORY;
    (*zfunc)->type = T_FUNC;
    (*zfunc)->refc = 0;
    (*zfunc)->fimp = fimp;
    (*zfunc)->arity = arity;
    return ZE_OK;
}

/* Remove 'zfunc' from memory. */
void
zdelfunc(ZFunc **zfunc)
{
    free((*zfunc)->fimp);
    (*zfunc)->fimp = NULL;
    free(*zfunc);
    *zfunc = NULL;
}

/* Create a new copy of 'source' in 'dest'.
 * If there is not enough memory, return ZE_OUT_OF_MEMORY.
 * Otherwise, return ZE_OK.
 */
ZError
zcpyfunc(ZFunc *source, ZFunc **dest)
{
    ZError err;

    err = znewfunc(dest, source->fimp, source->arity);
    if (err != ZE_OK)
        return err;
    return ZE_OK;
}

/* Test the truth value of 'zfunc'.
 * If 'zfunc' do not has an implementation, return zero.
 * Otherwise, return nonzero.
 */
int
ztstfunc(ZFunc *zfunc)
{
    if (zfunc->fimp != NULL)
        return 1;
    else
        return 0;
}

/* Compare 'zfunc' and 'other'.
 * If they are equal (have the same implementation), return zero.
 * Otherwise, return nonzero.
 */
int
zcmpfunc(ZFunc *zfunc, ZFunc *other)
{
    if (other->fimp == zfunc->fimp)
        return 0;
    else
        return 1;
}

/* Print the textual representation of 'zfunc' on 'buffer'.
 * Return the number of bytes writen.
 */
int
zrepfunc(char *buffer, size_t size, ZFunc *zfunc)
{
    if (*zfunc->fimp)
        return snprintf(buffer,
                        size,
                        "<%i-ary zap function>",
                        zfunc->arity);
    else
        return snprintf(buffer,
                        size,
                        "<%i-ary C function>",
                        zfunc->arity);
}
