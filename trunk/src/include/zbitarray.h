/*
 * Bit Array Type (header)
 */

typedef struct {
    Zob type;
    unsigned char refc;
    unsigned int length;
    char *bytes;
} BitArray;

BitArray *newiarr(unsigned int length);
void deliarr(BitArray **bitarray);
BitArray *cpyiarr(BitArray *bitarray);
Byte *getbititem(BitArray *bitarray, int index);
void setbititem(BitArray *bitarray, int index);
void rstbititem(BitArray *bitarray, int index);
char eqiarr(BitArray *bitarray, Zob *other);
unsigned int repiarr(char *buffer, BitArray *bitarray);
