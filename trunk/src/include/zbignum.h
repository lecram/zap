/*
 * BigNum Type (header)
 */

typedef struct {
    Zob type;
    unsigned char refc;
    unsigned int length;
    unsigned int *words;
} BigNum;

char halfstr(char *str);
unsigned int strbitlen(char *str);
BigNum *newbnum();
BigNum *bnumfromstr(char *str);
void delbnum(BigNum **bignum);
BigNum *cpybnum(BigNum *bignum);
Byte *getbit(BigNum *bignum, int index);
void setbit(BigNum *bignum, int index);
void rstbit(BigNum *bignum, int index);
void lshiftbnum(BigNum *bignum, unsigned int shift);
void rshiftbnum(BigNum *bignum, unsigned int shift);
void double_dabble(char *buffer, BigNum *bignum);
char eqbnum(BigNum *bignum, Zob *other);
unsigned int repbnum(char *buffer, BigNum *bignum);
