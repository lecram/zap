/*
 * Garbage Collector (header)
 */

typedef struct {
    Zob type;
    unsigned char refc;
} RefC;

void increfc(Zob *object);
void decrefc(Zob *object);
