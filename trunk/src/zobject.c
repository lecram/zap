/*
 * Copyright 2010 by Marcel Rodrigues <marcelgmr@gmail.com>
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

/*
 * Object
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

void
delobj(Zob **object)
{
    switch (**object) {
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
        case T_IARR:
            deliarr((BitArray **) object);
            break;
        case T_YARR:
            delyarr((ByteArray **) object);
            break;
        case T_WARR:
            delwarr((WordArray **) object);
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
        case T_NONE:
            return (Zob *) cpynone((None *) object);
        case T_BOOL:
            return (Zob *) cpybool((Bool *) object);
        case T_BYTE:
            return (Zob *) cpybyte((Byte *) object);
        case T_WORD:
            return (Zob *) cpyword((Word *) object);
        case T_IARR:
            return (Zob *) cpyiarr((BitArray *) object);
        case T_YARR:
            return (Zob *) cpyyarr((ByteArray *) object);
        case T_WARR:
            return (Zob *) cpywarr((WordArray *) object);
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
    return " ";
}

int
eqobj(Zob *object, Zob *other)
{
    switch (*object) {
        case T_NONE:
            return eqnone((None *) object, other);
        case T_BOOL:
            return eqbool((Bool *) object, other);
        case T_BYTE:
            return eqbyte((Byte *) object, other);
        case T_WORD:
            return eqword((Word *) object, other);
        case T_IARR:
            return eqiarr((BitArray *) object, other);
        case T_YARR:
            return eqyarr((ByteArray *) object, other);
        case T_WARR:
            return eqwarr((WordArray *) object, other);
        case T_BNUM:
            return eqbnum((BigNum *) object, other);
        case T_LIST:
            return eqlist((List *) object, other);
        case T_DICT:
            return eqdict((Dict *) object, other);
        case T_FUNC:
            return eqfunc((Func *) object, other);
        default:
            {
                char errbff[256];

                sprintf(errbff,
                        "Unknown Type Number in eqobj: %u.",
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
        case T_NONE:
            return repnone(buffer, (None *) object);
        case T_BOOL:
            return repbool(buffer, (Bool *) object);
        case T_BYTE:
            return repbyte(buffer, (Byte *) object);
        case T_WORD:
            return repword(buffer, (Word *) object);
        case T_IARR:
            return repiarr(buffer, (BitArray *) object);
        case T_YARR:
            return repyarr(buffer, (ByteArray *) object);
        case T_WARR:
            return repwarr(buffer, (WordArray *) object);
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
