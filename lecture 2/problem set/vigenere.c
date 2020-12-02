#include <cs50.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

int is_string_alpha(const char *str);
char *encode(char *plain, char *key);
char shifted_char(char c, int shift_value);
int shift(char c);

int main(int argc, string argv[])
{
    if (argc != 2)
    {
        // too many or too few keywords
        return 1;
    }

    // check that the key provided via command line is
    // alpha chars only, exit with error code if not
    char *key = argv[1];
    if (!is_string_alpha(key))
    {
        printf("Usage: ./vigenere keyword\n");
        return 1;
    }

    char *plain = get_string("plaintext: ");
    char *cipher = encode(plain, key);

    // print and release the memory
    printf("ciphertext: %s\n", cipher);
    free(cipher);

    return 0;
}

// encodes the provided text with vignere using the provided key
// relies on ASCII ordering; I approached this with
// ASCII values rather than a custom written string / array, because
// it would theoretically allow for easier expansion in allowed characters
char *encode(char *plain, char *key)
{
    int str_length = strlen(plain);
    int key_length = strlen(key);


    char *result = malloc(str_length + 1 * sizeof(char));
    for (int i = 0, j = 0; i < str_length; i++)
    {
        // preserve spaces and punctuation
        if (isspace(plain[i]) || ispunct(plain[i]))
        {
            result[i] = plain[i];
            continue;
        }

        // if the end of the key has been reached
        // then reset the index value
        if (j >= key_length)
        {
            j = 0;
        }

        // calc the shift and set the new character values
        int shift_value = shift(key[j]);
        result[i] = shifted_char(plain[i], shift_value);

        j++; // only increment when it's been used
    }

    result[str_length] = '\0';

    return result;
}

char shifted_char(char c, int shift_value)
{
    // define the character range based on the case value
    char anchor_end = isupper(c) ? (int)'Z' + 1 : (int)'z' + 1;
    char anchor_start = isupper(c) ? 'A' : 'a';

    // calc the new character value and check to see if it ends up
    // overflowing the upper or lower case range
    int sum = (int)c + shift_value;
    int remainder = sum % anchor_end;

    char result;
    // if it overflows then just add the remainder to the start of the range
    if (remainder != sum && remainder > 0)
    {
        result = anchor_start + remainder;
    }
    else
    {
        result = (int)c + shift_value;
    }

    return result;
}

// calculates the shift value from the provided ASCII char
int shift(char c)
{
    // upper and lowercase equivalents should result in the same shift
    // first find the remainder between the provided char and the higher ASCII value 'a'
    // e.g. 'b' % 'a' = 1 and 'B' % 'a' = 66
    // then find the remainder of this value compared to the lower ASCII value 'A'; if it's less than
    // that value it will just return the calculated remainder
    // e.g. 1 % 'A' = a and 66 % 'A' = 1
    return c % 'a' % 'A';
}

// checks that the string contains only alpha characters
int is_string_alpha(const char *str)
{
    int result = 1;
    int length = strlen(str);

    // iterate the string and if a non alpha character
    // is found stop and return 0
    unsigned char c;
    for (int i = 0; i < length; i++)
    {
        c = str[i];
        if (!isalpha(c))
        {
            result = 0;
            break;
        }
    }

    return result;
}
