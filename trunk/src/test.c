#include "zap.h"

ZError
test()
{
    char buffer[1024];
    ZList *zlist;
    Zob *zob;
    ZError err;

    puts("-- Test: Basics --");

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

    puts("-- Test: ZDict --");

    err = znewdict(&zdict);
    if (err != ZE_OK) return err;

    puts("Empty dict.");
    zrepobj(buffer, 1024, (Zob *) zdict);
    printf("%s\n", buffer);

    for (i = 0; strlen(pairs[i].key) > 0; i++) {
        err = zyarrfromstr(&key, pairs[i].key);
        if (err != ZE_OK) return err;
        err = znewbyte((ZByte **) &zob);
        if (err != ZE_OK) return err;
        ((ZByte *) zob)->value = pairs[i].value;
        printf("Inserting \"%s\".\n", pairs[i].key);
        err = zdset(zdict, (Zob *) key, zob);
        if (err != ZE_OK) return err;
        zrepobj(buffer, 1024, (Zob *) zdict);
        printf("%s\n", buffer);
    }

    puts("Removing \"Three\".");
    err = zyarrfromstr(&key, "Three");
    if (err != ZE_OK) return err;
    if (zdremove(zdict, (Zob *) key)) {
        zdelyarr(&key);
        puts("Removing \"Zero\".");
        err = zyarrfromstr(&key, "Zero");
        if (err != ZE_OK) return err;
        zdremove(zdict, (Zob *) key);
        zdelyarr(&key);
        puts("Removing \"Six\".");
        err = zyarrfromstr(&key, "Six");
        if (err != ZE_OK) return err;
        zdremove(zdict, (Zob *) key);
        zdelyarr(&key);
    }

    zrepobj(buffer, 1024, (Zob *) zdict);
    printf("%s\n", buffer);

    puts("Removing \"Eight\".");
    err = zyarrfromstr(&key, "Eight");
    if (err != ZE_OK) return err;
    if (zdremove(zdict, (Zob *) key)) {
        zdelyarr(&key);
        puts("Removing \"One\".");
        err = zyarrfromstr(&key, "One");
        if (err != ZE_OK) return err;
        zdremove(zdict, (Zob *) key);
        zdelyarr(&key);
        puts("Removing \"Five\".");
        err = zyarrfromstr(&key, "Five");
        if (err != ZE_OK) return err;
        zdremove(zdict, (Zob *) key);
        zdelyarr(&key);
    }

    zrepobj(buffer, 1024, (Zob *) zdict);
    printf("%s\n", buffer);

    printf("dict has %u pairs.\n", zdlength(zdict));

    err = zyarrfromstr(&key, "Two");
    if (err != ZE_OK) return err;
    if (zdhaskey(zdict, (Zob *) key))
        puts("\"Two\" is in dict.");
    else
        puts("\"Two\" is not in dict.");
    zdelyarr(&key);

    err = zyarrfromstr(&key, "Nine");
    if (err != ZE_OK) return err;
    if (zdhaskey(zdict, (Zob *) key))
        puts("\"Nine\" is in dict.");
    else
        puts("\"Nine\" is not in dict.");
    zdelyarr(&key);

    for (i = 0; strlen(pairs[i].key) > 0; i++) {
        printf("Searching for \"%s\"... ", pairs[i].key);
        err = zyarrfromstr(&key, pairs[i].key);
        if (err != ZE_OK) return err;
        if (zdget(zdict, (Zob *) key, &zob)) {
            zrepobj(buffer, 1024, zob);
            printf("Found with value %s.\n", buffer);
        }
        else {
            puts("Not found.");
        }
        zdelyarr(&key);
    }

    puts("Removing all pairs.");
    zdempty(zdict);

    zrepobj(buffer, 1024, (Zob *) zdict);
    printf("%s\n", buffer);

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

    puts("-- Test: ZNameTable --");

    err = znewnable(&znable);
    if (err != ZE_OK) return err;

    puts("Empty nable.");
    zrepobj(buffer, 1024, (Zob *) znable);
    printf("%s\n", buffer);

    for (i = 0; strlen(pairs[i].name) > 0; i++) {
        err = znewbyte((ZByte **) &zob);
        if (err != ZE_OK) return err;
        ((ZByte *) zob)->value = pairs[i].value;
        printf("Inserting \"%s\".\n", pairs[i].name);
        err = ztset(znable, pairs[i].name, zob);
        if (err != ZE_OK) return err;
        zrepobj(buffer, 1024, (Zob *) znable);
        printf("%s\n", buffer);
    }

    /* zrepnable_detail(znable); */

    puts("Removing \"Six\".");
    if (ztremove(znable, "Six")) {
        puts("Removing \"Five\".");
        ztremove(znable, "Five");
        puts("Removing \"Zero\".");
        ztremove(znable, "Zero");
    }

    zrepobj(buffer, 1024, (Zob *) znable);
    printf("%s\n", buffer);

    puts("Removing \"Eight\".");
    if (ztremove(znable, "Eight")) {
        puts("Removing \"One\".");
        ztremove(znable, "One");
        puts("Removing \"Two\".");
        ztremove(znable, "Two");
    }

    zrepobj(buffer, 1024, (Zob *) znable);
    printf("%s\n", buffer);

    printf("nable has %u entries.\n", ztlength(znable));

    if (zthasname(znable, "Two"))
        puts("\"Two\" is in nable.");
    else
        puts("\"Two\" is not in nable.");

    if (zthasname(znable, "Nine"))
        puts("\"Nine\" is in nable.");
    else
        puts("\"Nine\" is not in nable.");

    for (i = 0; strlen(pairs[i].name) > 0; i++) {
        printf("Searching for \"%s\"... ", pairs[i].name);
        if (ztget(znable, pairs[i].name, &zob)) {
            zrepobj(buffer, 1024, zob);
            printf("Found with value %s.\n", buffer);
        }
        else {
            puts("Not found.");
        }
    }

    puts("Removing all entries.");
    ztempty(znable);

    zrepobj(buffer, 1024, (Zob *) znable);
    printf("%s\n", buffer);

    zdelnable(&znable);
    return ZE_OK;
}

int
main(int argc, char *argv[])
{
    ZError err;
    int i, j, known;
    struct link {
        char *option;
        ZError (*func)();
    } links[] = {
      {"basics", test},
      {"dict", test_dict},
      {"nable", test_nable},
      {"", NULL}
    };

    if (argc == 1) {
        puts("Usage:\ntest test1 [test2 [...]]");
        return EXIT_SUCCESS;
    }
    for (i = 1; i < argc; i++) {
        known = 0;
        for (j = 0; links[j].func != NULL; j++) {
            if (strcmp(links[j].option, argv[i]) == 0) {
                err = links[j].func();
                if (err != ZE_OK) return zraiseerr(err);
                known = 1;
            }
        }
        if (!known)
            printf("-- Unknown option: %s --\n", argv[i]);
        puts("");
    }
    return EXIT_SUCCESS;
}
