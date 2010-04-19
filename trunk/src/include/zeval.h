/*
 * Evaluation (header)
 */

unsigned int readword(char **entry);
void skip_expr(char **entry);
Zob *eval(Dict *namespace, List *tmp, char **entry);
Zob *nameval(Dict *namespace, char **entry);
Zob *feval(Dict *namespace, List *tmp, char **entry);
