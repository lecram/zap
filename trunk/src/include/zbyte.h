/*
 * Byte Type (header)
 */

typedef struct {
    Zob type;
    unsigned char refc;
    unsigned char value;
} Byte;

Byte *newbyte();
void delbyte(Byte **byte);
Byte *cpybyte(Byte *byte);
char eqbyte(Byte *byte, Zob *other);
unsigned int repbyte(char *buffer, Byte *byte);
