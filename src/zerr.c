/*
 * Errors
 */

#include <stdlib.h>
#include <stdio.h>

#include "zerr.h"

void
raise(char *msg)
{
    printf("Error: %s\n", msg);
    exit(EXIT_FAILURE);
}
