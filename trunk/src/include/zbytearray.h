/*
 * Byte Array Type (header)
 */

typedef struct {
    Zob type;
    unsigned char refc;
    unsigned int length;
    char *bytes;
} ByteArray;

ByteArray *newyarr(unsigned int length);
ByteArray *yarrfromstr(char *s);
void delyarr(ByteArray **bytearray);
ByteArray * cpyyarr(ByteArray *bytearray);
Byte *getbyteitem(ByteArray *bytearray, int index);
void setbyteitem(ByteArray *bytearray, int index, Byte *byte);
void concatstr(ByteArray *bytearray, char *str);
void concat(ByteArray *bytearray, ByteArray *other);
char eqyarr(ByteArray *bytearray, Zob *other);
unsigned int repyarr(char *buffer, ByteArray *bytearray);
unsigned int repplain(char *buffer, ByteArray *bytearray);
