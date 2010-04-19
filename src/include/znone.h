/*
 * None Type (header)
 */

typedef struct {
    Zob type;
    unsigned char refc;
} None;

None *newnone();
void delnone(None **none);
None *cpynone(None *none);
char eqnone(None *none, Zob *other);
unsigned int repnone(char *buffer, None *none);
