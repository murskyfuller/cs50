#include <cs50.h>
#include <stdio.h>

int main(void)
{
    int *numbers = NULL;
    int capacity = 0;

    int size = 0;
    while (true)
    {
        int number = get_int("Number: ");

        if (number == INT_MAX) // equates to ctrl + D; which is EOF
        {
            break;
        }

        if (size == capacity)
        {
            int *tmp = realloc(numbers, sizeof(int) * (size + 1));
            if (tmp == NULL) // realloc failed
            {
                // quit
                break;
            }

            numbers = tmp;
            capacity++;
        }

        numbers[size] = number;
        size++;
    }

    for (int i = 0; i < size; i++)
    {
        printf("Input %i: %i\n", i, numbers[i]);
    }

    if (numbers != NULL)
    {
       free(numbers);
    }

    return 0;
}