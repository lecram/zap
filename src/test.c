#include "zap.h"

ZError
test()
{
    char buffer[1024];
    ZList *list;
    Zob *zob;
    ZError err;

    err = znewlist(&list);
    if (err != ZE_OK)
        return err;
    err = znewbyte((ZByte **) &zob);
    if (err != ZE_OK)
        return err;
    ((ZByte *) zob)->value = 'z';
    err = zlpush(list, zob);
    if (err != ZE_OK)
        return err;
    zrepobj(buffer, 1024, (Zob *) list);
    printf("Test output: %s\n", buffer);
    zdellist(&list);
    return ZE_OK;
}

int
main(int argc, char *argv[])
{
    return zraiseerr(test());
}
