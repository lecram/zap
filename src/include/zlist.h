/*
 * List Type (header)
 */

struct Node {
    Zob *object;
    struct Node *next;
};

typedef struct Node Node;

typedef struct {
    Zob type;
    unsigned char refc;
    unsigned int length;
    Node *first;
    Node *last;
} List;

Node * newnode(Zob *object);
void delnode(Node **node);
List *newlist();
void dellist(List **list);
void emptylist(List *list);
List *cpylist(List *list);
void appitem(List *list, Zob *object);
void setitem(List *list, int index, Zob *object);
void insitem(List *list, int index, Zob *object);
void extlist(List *list, List *ext);
void remfirst(List *list);
void remitem(List *list, int index);
char eqlist(List *list, Zob *other);
unsigned int replist(char *buffer, List *list);
