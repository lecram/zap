/*
 * Bool Type (header)
 */

typedef struct {
    Zob type;
    unsigned char refc;
    int value;
} Bool;

Bool *newbool();
void delbool(Bool **bool);
Bool *cpybool(Bool *bool);
char eqbool(Bool *bool, Zob *other);
unsigned int repbool(char *buffer, Bool *bool);
