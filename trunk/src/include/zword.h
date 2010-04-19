/*
 * Word Type (header)
 */

typedef struct {
    Zob type;
    unsigned char refc;
    int value;
} Word;

Word *newword();
void delword(Word **word);
Word *cpyword(Word *word);
char eqword(Word *word, Zob *other);
unsigned int repword(char *buffer, Word *word);
