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

/* Object */

/* In This File:
 * - General handle functions to Objects.
 */

#include <stdlib.h>
#include <stdio.h>

#include "ztypes.h"
#include "zerr.h"

#include "znone.h"
#include "zbool.h"
#include "zbyte.h"
#include "zword.h"
#include "zbytearray.h"
#include "zbignum.h"
#include "zlist.h"
#include "zdict.h"
#include "zfunc.h"

#include "zobject.h"

/* Remove 'zob' from memory. */
void
zdelobj(Zob **zob)
{
    switch (**zob) {
        case EMPTY:
            break;
        case T_NONE:
            zdelnone((ZNone **) zob);
            break;
        case T_BOOL:
            zdelbool((ZBool **) zob);
            break;
        case T_BYTE:
            zdelbyte((ZByte **) zob);
            break;
        case T_WORD:
            zdelint((ZInt **) zob);
            break;
        case T_YARR:
            zdelyarr((ZByteArray **) zob);
            break;
        case T_BNUM:
            zdelbnum((ZBigNum **) zob);
            break;
        case T_LIST:
            zdellist((ZList **) zob);
            break;
        case T_DICT:
            zdeldict((ZDict **) zob);
            break;
        case T_FUNC:
            zdelfunc((ZFunc **) zob);
            break;
        default:
            zraiseUnknownTypeNumber("zdelobj", **zob);
    }
}

/* Create a new copy of 'source' in 'dest'.
 * If 'source' do not has a valid type number, return ZE_UNKNOWN_TYPE_NUMBER.
 * If there is not enough memory, return ZE_OUT_OF_MEMORY.
 * Otherwise, return ZE_OK.
 */
ZError
zcpyobj(Zob *source, Zob **dest)
{
    switch (*source) {
        case EMPTY:
            return EMPTY;
        case T_NONE:
            return zcpynone((ZNone *) source, (ZNone **) dest);
        case T_BOOL:
            return zcpybool((ZBool *) source, (ZBool **) dest);
        case T_BYTE:
            return zcpybyte((ZByte *) source, (ZByte **) dest);
        case T_WORD:
            return zcpyint((ZInt *) source, (ZInt **) dest);
        case T_YARR:
            return zcpyyarr((ZByteArray *) source, (ZByteArray **) dest);
        case T_BNUM:
            return zcpybnum((ZBigNum *) source, (ZBigNum **) dest);
        case T_LIST:
            return zcpylist((ZList *) source, (ZList **) dest);
        case T_DICT:
            return zcpydict((ZDict *) source, (ZDict **) dest);
        case T_FUNC:
            return zcpyfunc((ZFunc *) source, (ZFunc **) dest);
        default:
            return ZE_UNKNOWN_TYPE_NUMBER;
    }
    /* Code never reached. */
    return ZE_OK;
}

/* Truth value testing.
 * The truth value of an object is a value that
 *  qualifies its content under a boolean perspective.
 * A ZNone object always has a FALSE truth value.
 * A ZBool object has a truth value equivalent to its own value.
 * A numeric object has a FALSE truth value if and
 *  only if its content is equivalent to 0.
 * A sequence object has a FALSE truth value if and
 *  only if its length is 0.
 * The truth value of an object is TRUE under all other condictions.
 */

/* Return the truth value of 'zob'. */
int
ztstobj(Zob *zob)
{
    switch (*zob) {
        case EMPTY:
            return 0;
        case T_NONE:
            return ztstnone((ZNone *) zob);
        case T_BOOL:
            return ztstbool((ZBool *) zob);
        case T_BYTE:
            return ztstbyte((ZByte *) zob);
        case T_WORD:
            return ztstint((ZInt *) zob);
        case T_YARR:
            return ztstyarr((ZByteArray *) zob);
        case T_BNUM:
            return ztstbnum((ZBigNum *) zob);
        case T_LIST:
            return ztstlist((ZList *) zob);
        case T_DICT:
            return ztstdict((ZDict *) zob);
        case T_FUNC:
            return ztstfunc((ZFunc *) zob);
        default:
            zraiseUnknownTypeNumber("ztstobj", *zob);
    }
    return 0;
}

/* Compare 'zob' and 'other'.
 * If they are equal, return zero.
 * Otherwise, return nonzero.
 */
int
zcmpobj(Zob *zob, Zob *other)
{
    if (*zob != *other)
        return 1;
    switch (*zob) {
        case EMPTY:
            return 0;
        case T_NONE:
            return zcmpnone((ZNone *) zob, (ZNone *) other);
        case T_BOOL:
            return zcmpbool((ZBool *) zob, (ZBool *) other);
        case T_BYTE:
            return zcmpbyte((ZByte *) zob, (ZByte *) other);
        case T_WORD:
            return zcmpint((ZInt *) zob, (ZInt *) other);
        case T_YARR:
            return zcmpyarr((ZByteArray *) zob, (ZByteArray *) other);
        case T_BNUM:
            return zcmpbnum((ZBigNum *) zob, (ZBigNum *) other);
        case T_LIST:
            return zcmplist((ZList *) zob, (ZList *) other);
        case T_DICT:
            return zcmpdict((ZDict *) zob, (ZDict *) other);
        case T_FUNC:
            return zcmpfunc((ZFunc *) zob, (ZFunc *) other);
        default:
            zraiseUnknownTypeNumber("zcmpobj", *zob);
    }
    return 1;
}

/* Print the textual representation of 'zob' on 'buffer'.
 * Return the number of bytes writen.
 */
unsigned int
zrepobj(char *buffer, Zob *zob)
{
    switch (*zob) {
        case EMPTY:
            return 0;
        case T_NONE:
            return zrepnone(buffer, (ZNone *) zob);
        case T_BOOL:
            return zrepbool(buffer, (ZBool *) zob);
        case T_BYTE:
            return zrepbyte(buffer, (ZByte *) zob);
        case T_WORD:
            return zrepint(buffer, (ZInt *) zob);
        case T_YARR:
            return zrepyarr(buffer, (ZByteArray *) zob);
        case T_BNUM:
            return zrepbnum(buffer, (ZBigNum *) zob);
        case T_LIST:
            return zreplist(buffer, (ZList *) zob);
        case T_DICT:
            return zrepdict(buffer, (ZDict *) zob);
        case T_FUNC:
            return zrepfunc(buffer, (ZFunc *) zob);
        default:
            zraiseUnknownTypeNumber("zrepobj", *zob);
    }
    return 0;
}

/* If 'zob' has a valid type number,
 *  save its type name in 'name' and return ZE_OK.
 * If there is not enough memory, return ZE_OUT_OF_MEMORY.
 * Otherwise, return ZE_UNKNOWN_TYPE_NUMBER.
 */
ZError
ztypename(Zob *zob, Zob **name)
{
    ZError err = ZE_OK;

    switch (*zob) {
        case EMPTY:
            err = zyarrfromstr((ZByteArray **) name, "EMPTY");
            break;
        case T_NONE:
            err = zyarrfromstr((ZByteArray **) name, "None");
            break;
        case T_BOOL:
            err = zyarrfromstr((ZByteArray **) name, "Bool");
            break;
        case T_BYTE:
            err = zyarrfromstr((ZByteArray **) name, "Byte");
            break;
        case T_WORD:
            err = zyarrfromstr((ZByteArray **) name, "Int");
            break;
        case T_YARR:
            err = zyarrfromstr((ZByteArray **) name, "ByteArray");
            break;
        case T_BNUM:
            err = zyarrfromstr((ZByteArray **) name, "BigNum");
            break;
        case T_LIST:
            err = zyarrfromstr((ZByteArray **) name, "List");
            break;
        case T_DICT:
            err = zyarrfromstr((ZByteArray **) name, "Dict");
            break;
        case T_FUNC:
            err = zyarrfromstr((ZByteArray **) name, "Func");
            break;
        default:
            return ZE_UNKNOWN_TYPE_NUMBER;
    }
    return err;
}
