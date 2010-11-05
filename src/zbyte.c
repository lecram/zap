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

/* ZByte Type */

#include <stdlib.h>
#include <stdio.h>

#include "ztypes.h"
#include "zerr.h"

#include "zbyte.h"

/* Create a new ZByte in 'zbyte'.
 * If there is not enough memory, return ZE_OUT_OF_MEMORY.
 * Otherwise, return ZE_OK.
 */
ZError
znewbyte(ZByte **zbyte)
{
    *zbyte = (ZByte *) malloc(sizeof(ZByte));
    if (*zbyte == NULL)
        return ZE_OUT_OF_MEMORY;
    (*zbyte)->type = T_BYTE;
    (*zbyte)->refc = 0;
    return ZE_OK;
}

/* Remove 'zbyte' from memory. */
void
zdelbyte(ZByte **zbyte)
{
    free(*zbyte);
    *zbyte = NULL;
}

/* Create a new copy of 'source' in 'dest'.
 * If there is not enough memory, return ZE_OUT_OF_MEMORY.
 * Otherwise, return ZE_OK.
 */
ZError
zcpybyte(ZByte *source, ZByte **dest)
{
    ZError err;

    err = znewbyte(dest);
    if (err != ZE_OK)
        return err;
    (*dest)->value = source->value;
    return ZE_OK;
}

/* Test the truth value of 'zbyte'.
 * Return 'zbyte->value'.
 */
int
ztstbyte(ZByte *zbyte)
{
    return (int) zbyte->value;
}

/* Compare 'zbyte' and 'other'.
 * If they are equal, return zero.
 * Otherwise, return nonzero.
 */
int
zcmpbyte(ZByte *zbyte, ZByte *other)
{
    if (other->value == zbyte->value)
        return 0;
    else
        return 1;
}

/* Print the textual representation of 'zbyte' on 'buffer'.
 * Return the number of bytes writen.
 */
unsigned int
zrepbyte(char *buffer, ZByte *zbyte)
{
    return sprintf(buffer, "0x%02X", zbyte->value);
}
