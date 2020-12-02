#include <cs50.h>
#include <stdio.h>

int prompt_height();
void print_pyramid_of_height(int height);
void print_line(int line_ix, int pyramid_height);
char *build_char_array(int line_ix, int pyramid_height);

int main(void)
{
    int h = prompt_height();

    print_pyramid_of_height(h);
}

// continues to ask the user for an integer input until
// the range condition is satisfied
int prompt_height()
{
    int h;
    do
    {
        h = get_int("Height: ");
    }
    while (h < 1 || h > 8);

    return h;
}

// side-effect method constructs each line of a pyramid of provided height and print's each line to the console
void print_pyramid_of_height(int height)
{
    // calculate the base array length, this is the char length of each line
    int base_char_length = (height + 1) * 2;

    for (int i = 0; i < height; i++)
    {
        print_line(i, height);
    }
}

// side-effect method constructs and prints the line to the console
void print_line(int line_ix, int pyramid_height)
{
    char *str = build_char_array(line_ix, pyramid_height);

    printf("%s", str);

    free(str);
}

// side-effect method that constructs the character array to print into the pointer 'result' passed in
char *build_char_array(int line_ix, int pyramid_height)
{
    // the number of hashes that should appear in this line
    int hash_count_down = line_ix + 1;
    int array_length = pyramid_height + hash_count_down + 3;

    char *result = (char *) malloc(array_length * sizeof(char));

    // assigns the char values from the middle out
    for (int i = 0; i < pyramid_height + 1; i++)
    {
        // the primary ix and it's mirror compliment
        int ix = pyramid_height - i;
        int comp_ix = pyramid_height + 1 + i;

        // if the line still needs more hashes, then use that
        // otherwise use a space
        char value = ' ', mirror_val = ' ';

        // assign hash to value and mirror_val if still needed
        if (i > 0 && hash_count_down > 0)
        {
            value = '#';
            mirror_val = '#';
        }

        // assign the value to the correct spots in the char array
        result[ix] = value;

        // if the end of the array hasn't been reached add the mirror value
        if (hash_count_down > -1)
        {
            result[comp_ix]  = mirror_val;

            if (i > 0)
            {
                hash_count_down--;
            }
        }
    }

    result[array_length - 1] = '\n';

    return result;
}
