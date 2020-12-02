#include <stdio.h>
#include <stdlib.h>

#include "struct.h"

void f(void);

int main(void)
{
    f();
}

void f(void)
{
    int *x = malloc(sizeof(int) * 10);
    x[9] = 50;
    printf("Your input: %i\n", x[9]);
    free(x);
}