/*
 * Dict Type (header)
 */

typedef struct {
    Zob type;
    unsigned char refc;
    List *list;
} Dict;

Dict *newdict();
void deldict(Dict **dict);
Dict *cpydict(Dict *dict);
int haskey(Dict *dict, Zob *key);
void setkey(Dict *dict, Zob *key, Zob *value);
Zob *getkey(Dict *dict, Zob *key, Zob *defval);
void remkey(Dict *dict, Zob *key);
void update(Dict *dict, Dict *other);
char eqdict(Dict *dict, Zob *other);
unsigned int repdict(char *buffer, Dict *dict);
