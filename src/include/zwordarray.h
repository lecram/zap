/*
 * Word Array Type (header)
 */

typedef struct {
    Zob type;
    unsigned char refc;
    unsigned int length;
    unsigned int *words;
} WordArray;

WordArray *newwarr(unsigned int length);
void delwarr(WordArray **wordarray);
WordArray *cpywarr(WordArray *wordarray);
Word *getworditem(WordArray *wordarray, int index);
void setworditem(WordArray *wordarray, int index, Word *word);
char eqwarr(WordArray *wordarray, Zob *other);
unsigned int repwarr(char *buffer, WordArray *wordarray);
