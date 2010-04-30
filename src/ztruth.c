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

/* Truth Value */

/* The truth value of an object is a value that
 *  qualifies its content under a boolean perspective.
 * A None object always has a FALSE truth value.
 * A Bool object has a truth value equivalent to its own value.
 * A numeric object has a FALSE truth value if and
 *  only if its content is equivalent to 0.
 * A sequence object has a FALSE truth value if and
 *  only if its length is 0.
 * The truth value of an object is TRUE under all other condictions.
 */

#include <stdio.h>

#include "zerr.h"
#include "ztypes.h"
#include "zobject.h"
#include "znone.h"
#include "zbool.h"
#include "zbyte.h"
#include "zword.h"
#include "zbitarray.h"
#include "zbytearray.h"
#include "zwordarray.h"
#include "zbignum.h"
#include "zlist.h"
#include "zdict.h"
#include "zfunc.h"

Bool *
objtruth(Zob *object)
{
    char errbff[256];
    Bool *truth = newbool();

    truth->value = 0;
    switch (*object) {
        case T_NONE:
            /* None is always False. */
            break;
        case T_BOOL:
            truth->value = ((Bool *) object)->value;
            break;
        case T_BYTE:
            truth->value = (int) ((Byte *) object)->value;
            break;
        case T_WORD:
            truth->value = ((Word *) object)->value;
            break;
        case T_IARR:
            truth->value = (int) ((BitArray *) object)->length;
            break;
        case T_YARR:
            truth->value = (int) ((ByteArray *) object)->length;
            break;
        case T_WARR:
            truth->value = (int) ((WordArray *) object)->length;
            break;
        case T_BNUM:
            /* Not Implemented */
            break;
        case T_LIST:
            truth->value = (int) ((List *) object)->length;
            break;
        case T_DICT:
            truth->value = (int) ((Dict *) object)->list->length;
            break;
        default:
            sprintf(errbff,
                    "Unknown Type Number: %u.",
                    *object);
            raise(errbff);
    }
    return truth;
}
