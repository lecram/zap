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

/* zap includes */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "ztypes.h"
#include "zerr.h"
#include "zgc.h"

#include "znone.h"
#include "zbool.h"
#include "zbyte.h"
#include "zint.h"
#include "zbytearray.h"
#include "zbignum.h"
#include "zlist.h"
#include "znametable.h"
#include "zdict.h"
#include "zfunc.h"

#include "zobject.h"
#include "zruntime.h"
#include "zbuiltin.h"

#include "zcpl_expr.h"
#include "zcpl_mod.h"
