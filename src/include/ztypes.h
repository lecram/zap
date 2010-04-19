/*
 * Types (header)
 */

/* Word length */
#define WL (8 * sizeof(unsigned int))

/* Type signs */
#define T_NONE  1
#define T_BOOL  2
#define T_BYTE  3
#define T_WORD  4
#define T_IARR  5  /* Bit Array  */
#define T_YARR  6  /* Byte Array */
#define T_WARR  7  /* Word Array */
#define T_BNUM  8  /* BigNum     */
#define T_LIST  9
#define T_DICT 10
#define T_FUNC 11

typedef unsigned char Zob;
