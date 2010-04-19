/*
 * Truth Value
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
            /* Not Implemented */
            break;
        case T_YARR:
            /* Not Implemented */
            break;
        case T_WARR:
            /* Not Implemented */
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
