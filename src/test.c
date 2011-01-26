#include "zap.h"

ZError
test()
{
    char buffer[1024];
    ZList *zlist;
    Zob *zob;
    ZError err;

    err = znewlist(&zlist);
    if (err != ZE_OK) return err;
    err = znewbyte((ZByte **) &zob);
    if (err != ZE_OK) return err;
    ((ZByte *) zob)->value = 'z';
    err = zlpush(zlist, zob);
    if (err != ZE_OK) return err;
    zrepobj(buffer, 1024, (Zob *) zlist);
    printf("[0x7A] == %s\n", buffer);
    zdellist(&zlist);
    return ZE_OK;
}

ZError
test_dict()
{
    char buffer[1024];
    ZDict *zdict;
    ZByteArray *key;
    Zob *zob;
    ZError err;
    int i ;

    struct pair {
        char *key;
        unsigned char value;
    } pairs[] = {
      {"Zero", 0},
      {"One", 1},
      {"Two", 2},
      {"Three", 3},
      {"Four", 4},
      {"Five", 5},
      {"Six", 6},
      {"", 0}
    };

    err = znewdict(&zdict);
    if (err != ZE_OK) return err;

    for (i = 0; strlen(pairs[i].key) > 0; i++) {
        err = zyarrfromstr(&key, pairs[i].key);
        if (err != ZE_OK) return err;
        err = znewbyte((ZByte **) &zob);
        if (err != ZE_OK) return err;
        ((ZByte *) zob)->value = pairs[i].value;
        err = zdset(zdict, (Zob *) key, zob);
        if (err != ZE_OK) return err;
    }

    zrepobj(buffer, 1024, (Zob *) zdict);
    printf("dict = %s\n", buffer);
    zdeldict(&zdict);
    return ZE_OK;
}

ZError
test_nable()
{
    char buffer[1024];
    ZNameTable *znable;
    Zob *zob;
    ZError err;
    int i ;

    struct pair {
        char *name;
        unsigned char value;
    } pairs[] = {
      {"Zero", 0},
      {"One", 1},
      {"Two", 2},
      {"Three", 3},
      {"Four", 4},
      {"Five", 5},
      {"Six", 6},
      {"", 0}
    };

    err = znewnable(&znable);
    if (err != ZE_OK) return err;

    for (i = 0; strlen(pairs[i].name) > 0; i++) {
        err = znewbyte((ZByte **) &zob);
        if (err != ZE_OK) return err;
        ((ZByte *) zob)->value = pairs[i].value;
        err = ztset(znable, pairs[i].name, zob);
        if (err != ZE_OK) return err;
    }

    zrepobj(buffer, 1024, (Zob *) znable);
    printf("nable = %s\n", buffer);
    /* nable = {Five:0x05 Four:0x04 One:0x01 Six:0x06 Three:0x03 Two:0x02 Zero:0x00} */
    zdelnable(&znable);
    return ZE_OK;
}

int
main(int argc, char *argv[])
{
    return zraiseerr(test_nable());
}
