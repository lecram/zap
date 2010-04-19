/*
 * Built-in Functions (header)
 */

Zob *bprint(List *args);
Zob *brepr(List *args);
Zob *blen(List *args);
Zob *bconcat(List *args);
Zob *bget(List *args);

void regfunc(Dict *namespace, char *name, Zob *(*cfunc)(List *args));
Dict *bbuild();
