#include <cs50.h>
#include <stdio.h>

typedef struct node
{
    int number;
    struct node *next;
}
node;

int main(void)
{
    // Memory for numbers
    node *numbers = NULL;

    // Prompt for numbers (until EOF)
    while (true)
    {
        // Prompt for number
        int number = get_int("Number: ");

        // Check for EOF
        if (number == INT_MAX)
        {
            printf("\n");
            break;
        }

        // Allocate space for number
        node *n = malloc(sizeof(node));
        if (!n) // out of memory
        {
            printf("Out of memory\n");
            return 1;
        }

        // Add number to end of list
        n->number = number;
        n->next = NULL;
        if (numbers)
        {
            // iterate through and add the node to the end of the list
            for (node *ptr = numbers; ptr != NULL; ptr = ptr->next;)
            {
                if (!ptr->next)
                {
                    ptr->next = n;
                    break;
                }
            }
        }
        else // first item to be added
        {
            numbers = n;
        }
    }
}