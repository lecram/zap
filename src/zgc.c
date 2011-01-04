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

/* Garbage Collector */

/* Garbage collection is done by simple reference counting. */

#include <stddef.h>

#include "ztypes.h"
#include "zerr.h"

#include "zobject.h"

#include "zgc.h"

void
zincrefc(Zob *object)
{
    ((RefC *) object)->refc++;
}

void
zdecrefc(Zob *object)
{
    if (((RefC *) object)->refc <= 1)
        zdelobj(&object);
    else
        ((RefC *) object)->refc--;
}
