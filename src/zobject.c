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

void
delobj(Zob **object)
{
    switch (**object) {
        case EMPTY:
            break;
        case T_NONE:
            delnone((None **) object);
            break;
        case T_BOOL:
            delbool((Bool **) object);
            break;
        case T_BYTE:
            delbyte((Byte **) object);
            break;
        case T_WORD:
            delword((Word **) object);
            break;
        case T_YARR:
            delyarr((ByteArray **) object);
            break;
        case T_BNUM:
            delbnum((BigNum **) object);
            break;
        case T_LIST:
            dellist((List **) object);
            break;
        case T_DICT:
            deldict((Dict **) object);
            break;
        case T_FUNC:
            delfunc((Func **) object);
            break;
        default:
            {
                char errbff[256];

                sprintf(errbff,
                        "Unknown Type Number in delobj: %u.",
                        **object);
                raise(errbff);
            }
    }
}

Zob *
cpyobj(Zob *object)
{
    switch (*object) {
        case EMPTY:
            return EMPTY;
        case T_NONE:
            return (Zob *) cpynone((None *) object);
        case T_BOOL:
            return (Zob *) cpybool((Bool *) object);
        case T_BYTE:
            return (Zob *) cpybyte((Byte *) object);
        case T_WORD:
            return (Zob *) cpyword((Word *) object);
        case T_YARR:
            return (Zob *) cpyyarr((ByteArray *) object);
        case T_BNUM:
            return (Zob *) cpybnum((BigNum *) object);
        case T_LIST:
            return (Zob *) cpylist((List *) object);
        case T_DICT:
            return (Zob *) cpydict((Dict *) object);
        case T_FUNC:
            return (Zob *) cpyfunc((Func *) object);
        default:
            {
                char errbff[256];

                sprintf(errbff,
                        "Unknown Type Number in cpyobj: %u.",
                        *object);
                raise(errbff);
            }
    }
    return EMPTY;
}

/* Truth value testing.
 * The truth value of an object is a value that
 *  qualifies its content under a boolean perspective.
 * A None object always has a FALSE truth value.
 * A Bool object has a truth value equivalent to its own value.
 * A numeric object has a FALSE truth value if and
 *  only if its content is equivalent to 0.
 * A sequence object has a FALSE truth value if and
 *  only if its length is 0.
 * The truth value of an object is TRUE under all other condictions.
 */

int
tstobj(Zob *object)
{
    switch (*object) {
        case EMPTY:
            return 0;
        case T_NONE:
            return tstnone((None *) object);
        case T_BOOL:
            return tstbool((Bool *) object);
        case T_BYTE:
            return tstbyte((Byte *) object);
        case T_WORD:
            return tstword((Word *) object);
        case T_YARR:
            return tstyarr((ByteArray *) object);
        case T_BNUM:
            return tstbnum((BigNum *) object);
        case T_LIST:
            return tstlist((List *) object);
        case T_DICT:
            return tstdict((Dict *) object);
        case T_FUNC:
            return tstfunc((Func *) object);
        default:
            {
                char errbff[256];

                sprintf(errbff,
                        "Unknown Type Number in tstobj: %u.",
                        *object);
                raise(errbff);
            }
    }
    return 0;
}

int
cmpobj(Zob *object, Zob *other)
{
    if (*object != *other) return 1;
    switch (*object) {
        case EMPTY:
            return 0;
        case T_NONE:
            return cmpnone((None *) object, (None *) other);
        case T_BOOL:
            return cmpbool((Bool *) object, (Bool *) other);
        case T_BYTE:
            return cmpbyte((Byte *) object, (Byte *) other);
        case T_WORD:
            return cmpword((Word *) object, (Word *) other);
        case T_YARR:
            return cmpyarr((ByteArray *) object, (ByteArray *) other);
        case T_BNUM:
            return cmpbnum((BigNum *) object, (BigNum *) other);
        case T_LIST:
            return cmplist((List *) object, (List *) other);
        case T_DICT:
            return cmpdict((Dict *) object, (Dict *) other);
        case T_FUNC:
            return cmpfunc((Func *) object, (Func *) other);
        default:
            {
                char errbff[256];

                sprintf(errbff,
                        "Unknown Type Number in cmpobj: %u.",
                        *object);
                raise(errbff);
            }
    }
    return 1;
}

unsigned int
repobj(char *buffer, Zob *object)
{
    switch (*object) {
        case EMPTY:
            return 0;
        case T_NONE:
            return repnone(buffer, (None *) object);
        case T_BOOL:
            return repbool(buffer, (Bool *) object);
        case T_BYTE:
            return repbyte(buffer, (Byte *) object);
        case T_WORD:
            return repword(buffer, (Word *) object);
        case T_YARR:
            return repyarr(buffer, (ByteArray *) object);
        case T_BNUM:
            return repbnum(buffer, (BigNum *) object);
        case T_LIST:
            return replist(buffer, (List *) object);
        case T_DICT:
            return repdict(buffer, (Dict *) object);
        case T_FUNC:
            return repfunc(buffer, (Func *) object);
        default:
            {
                char errbff[256];

                sprintf(errbff,
                        "Unknown Type Number in repobj: %u.",
                        *object);
                raise(errbff);
            }
    }
    return 0;
}

Zob *
typename(Zob *object)
{
    switch (*object) {
        case EMPTY:
            return (Zob *) yarrfromstr("EMPTY");
        case T_NONE:
            return (Zob *) yarrfromstr("None");
        case T_BOOL:
            return (Zob *) yarrfromstr("Bool");
        case T_BYTE:
            return (Zob *) yarrfromstr("Byte");
        case T_WORD:
            return (Zob *) yarrfromstr("Word");
        case T_YARR:
            return (Zob *) yarrfromstr("ByteArray");
        case T_BNUM:
            return (Zob *) yarrfromstr("BigNum");
        case T_LIST:
            return (Zob *) yarrfromstr("List");
        case T_DICT:
            return (Zob *) yarrfromstr("Dict");
        case T_FUNC:
            return (Zob *) yarrfromstr("Func");
        default:
            {
                char errbff[256];

                sprintf(errbff,
                        "Unknown Type Number in typename: %u.",
                        *object);
                raise(errbff);
            }
    }
    return EMPTY;
}
