#include <cs50.h>
#include <stdio.h>

int main(void)
{
    int i = get_int("x: ");

    if(i % 2 == 0)
    {
        printf("%i is even\n", i);
    } else
    {
        printf("%i is odd\n", i);
    }
}
