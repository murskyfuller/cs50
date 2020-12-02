#include <stdio.h>
#include <stdlib.h>

int string_length(char *string);
char *get_string(char *prompt);
void print_by_char(char *str);

int main(void)
{
    char *s = get_string("Name: ");
    print_by_char(s);

    free(s);
}

void print_by_char(char *str)
{
    for (int i = 0, n = string_length(str); i < n; i++)
    {
        int ascii = (int) str[i];
        printf("%c %i\n", str[i], ascii);
    }
}

char *get_string(char *prompt)
{
    char *response = malloc(256);

    printf("%s", prompt);
    fgets(response, 256, stdin);

    int last_ix = string_length(response) - 1;
    if (response[last_ix] == '\n')
    {
        response[last_ix] = '\0';
    }

    return response;
}

int string_length(char *str)
{
    int n = 0;
    while (str[n] != '\0')
    {
        n++;
    }

    return n;
}
