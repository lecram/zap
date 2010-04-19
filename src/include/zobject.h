/*
 * Object (header)
 */

void delobj(Zob **object);
Zob *cpyobj(Zob *object);
int eqobj(Zob *object, Zob *other);
unsigned int repobj(char *buffer, Zob *object);
