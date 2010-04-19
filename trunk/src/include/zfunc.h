/*
 * Func Type (header)
 */

typedef int FImp;

typedef struct {
    FImp high; /* 0 */
    /* Pointer to C function. */
    Zob *(*func)(List *args);
} LowFunc;

typedef struct {
    FImp high; /* 1 */
    /* Pointer to zap function. */
    char *func;
} HighFunc;

typedef struct {
    Zob type; /* T_FUNC */
    unsigned char refc;
    /* Pointer to LowFunc or HighFunc. */
    FImp *fimp;
} Func;

LowFunc *newlowfunc();
void dellowfunc(LowFunc **lowfunc);
HighFunc *newhighfunc();
void delhighfunc(Func **highfunc);
Func *newfunc(FImp *fimp);
void delfunc(Func **func);
Func *cpyfunc(Func *func);
char eqfunc(Func *func, Zob *other);
Zob *callimp(FImp *high, List *args);
unsigned int repfunc(char *buffer, Func *func);
