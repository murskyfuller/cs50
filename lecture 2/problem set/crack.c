#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <crypt.h>
#include "crack.h"
#include "password_gen.h"
#include "utils.h"

const int MAX_PASS_LENGTH = 5;
const char *CHAR_SET = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
int g_perm_ix, g_char_set_length, g_total_num_perms;
char *salt;
char ***all_passwords;

int main(int argc, char *argv[])
{
    // throw an error if incorrect number of args is provided
    int arg_error = validate_argc(argc);

    if (arg_error > 0)
    {
        return arg_error;
    }

    // get the salt value for creating all hash values later
    char *input = argv[1];
    salt = get_salt(input);

    g_perm_ix = 0;
    g_char_set_length = strlen(CHAR_SET);

    char *result;
    // generate all possible passwords and populate into the global var
    for (int i = 1; i <= MAX_PASS_LENGTH; i++)
    {
        populate_password_permutations(i);

        // sort the global var by hash values alphabetically, since we need to match hash
        sort_by_encoded(all_passwords, g_total_num_perms);

        // binary search the hash values in all_passwords and print a result if found
        result = find_password(input);

        // free any allocated memory
        clean_up();

        if (!is_empty(result))
        {
            printf("%s\n", result);
            break;
        }
    }

    free(salt);
    return 0;
}

/* FIND PASSWORD */
// binary search of a sorted power set
char *find_password(char *target)
{
    char *result = "";
    int found = 0;
    int start_ix = 0;
    int end_ix = g_total_num_perms - 1;

    do
    {
        // find the middle value of the subset
        // and compare its hash value to the target value
        int range = end_ix - start_ix;
        int mid = start_ix + (range / 2);
        char **item = all_passwords[mid];
        int comparison = str_compare(item[1], target, 2);

        if (comparison == 0)
        {
            // found a match
            found = 1;
            result = copy_str(item[0]);
            break;
        }
        else if (comparison == -1)
        {
            // use the upper half
            start_ix = mid + 1;
        }
        else
        {
            // use the lower half
            end_ix = mid - 1;
        }
    }
    while ((found == 0) && (start_ix <= end_ix));


    return result;
}

// sorts all the plain text and encoded text passwords in place
void sort_by_encoded(char ***arr, int length)
{
    if (length < 2)
    {
        return;
    }

    int second_half_length = length / 2;
    int half_length = length % 2 > 0 ? second_half_length + 1 : second_half_length;

    // get copies of each half to manipulate
    // and sort alphabetically by the nested hash value
    char ***first_half = slice_for_sort(arr, 0, half_length);
    sort_by_encoded(first_half, half_length);

    char ***second_half = slice_for_sort(arr, half_length, second_half_length);
    sort_by_encoded(second_half, second_half_length);

    for (int i = 0, j = 0, k = 0; i < length; i++)
    {
        char *encoded_a = "";
        char *encoded_b = "";

        // if all values in first_half haven't been iterated through
        // set the next hash value to compare
        if (j < half_length)
        {
            encoded_a = first_half[j][1];
        }

        // if all values in second_Half haven't been iterated through
        // set the next hash value to compare
        if (k < second_half_length)
        {
            encoded_b = second_half[k][1];
        }

        // if the hash value from first_half comes before the one from the second half alphabetically
        // move its pointer into the next spot on the results array
        if (is_empty(encoded_b) || (!is_empty(encoded_a) && (str_compare(encoded_a, encoded_b, 2) < 1)))
        {
            arr[i] = first_half[j];
            j++;
            continue;
        }
        else
        {
            // otherwise assume the second half hash value should go next
            arr[i] = second_half[k];
            k++;
            continue;
        }
    }

    free(first_half);
    free(second_half);

    return;
}

// free up all the dynamically allocated memory
void clean_up()
{
    for (int i = 0; i < g_total_num_perms; i++)
    {
        char **plain_encoded = all_passwords[i];
        char *plain = plain_encoded[0];
        char *encoded = plain_encoded[1];

        free(plain);
        free(encoded);
        free(plain_encoded);
    }

    free(all_passwords);
    g_perm_ix = 0;
}

/* END FIND PASSWORD */

/* COMMAND LINE VALIDATION */
int validate_argc(int argc)
{
    int result = 0;

    // one argument of password to crack is required
    if (argc != 2)
    {
        if (argc < 2)
        {
            printf("One argument of encoded password to crack is required\n");
            result = 401;
        }
        else if (argc > 2)
        {
            printf("This program only accepts one command line argument\n");
            result = 402;
        }
    }

    return result;
}
/* END COMMAND LINE VALIDATION */

/* PASSWORD GEN */

// allocates memory for the array storing all the possible password permutations
// and then initiates its population
void populate_password_permutations(int length)
{
    g_total_num_perms = calc_total_perms_with_rep(g_char_set_length, length);
    all_passwords = malloc(g_total_num_perms * sizeof(char *));

    generate_permutations_with_repeat(CHAR_SET, length);
}

// inits the creation of all possible permutations for the provided string and max string length
// from length 1 to max length and utilizing all characters in the provided set
void generate_permutations_with_repeat(const char *char_set, int max_length)
{
    if (max_length > 5 || max_length < 1)
    {
        if (max_length > 5)
        {
            printf("Max password lengths greater than 5 could result in memory issues");
        }
        else if (max_length < 1)
        {
            printf("Max password length should be greater than 0");
        }

        return;
    }

    for (int i = 0; i < g_char_set_length; i++)
    {
        char curChar = CHAR_SET[i];

        for (int j = 1; j <= max_length; j++)
        {
            char *str = malloc(j * sizeof(char));
            passwords_for_char_and_length(0, i, j, str);
        }
    }
}

// recursively generates all possible permutations and stores them in the global
// passwords array
void passwords_for_char_and_length(int str_ix, int char_ix, int length, char *str)
{
    // push the char onto the array
    str[str_ix] = CHAR_SET[char_ix];

    if (str_ix == length - 1)
    {
        // mark the end of the string with null
        str[length] = '\0';

        // create an array of the plain text and encoded version
        char **plain_encoded = malloc(2 * sizeof(char *));
        plain_encoded[0] = str;
        char *encoded = crypt(str, salt);
        plain_encoded[1] = copy_array(encoded,
                                      strlen(encoded)); // crypt returns the same pointer each time, so need to copy it

        // add the set to the all_passwords array and increment the index
        all_passwords[g_perm_ix] = plain_encoded;
        g_perm_ix++;
        return;
    }

    // move to the next str position and loop to add every
    // char from set after
    for (int i = 0; i < g_char_set_length; i++)
    {
        passwords_for_char_and_length(str_ix + 1, i, length, copy_array(str, length));
    }
}

// partial application of copy_array for strings
char *copy_str(char *str)
{
    return copy_array(str, strlen(str) + 1);
}

// makes a duplicate of the string passed in so that a char can be added
char *copy_array(char *str, int length)
{
    // allocate the memory for the copied array
    char *new_array = malloc(length * sizeof(char));

    // add each char from the array onto the new array
    for (int i = 0; i < length; i++)
    {
        new_array[i] = str[i];
    }

    return new_array;
}

// does quick power calc and addition to determine the total number
// of possible permutations with repetition
int calc_total_perms_with_rep(int set_length, int max_str_length)
{
    int result = 0;

    for (int i = 1; i <= max_str_length; i++)
    {
        result += power(set_length, i);
    }

    return result;
}

// returns the first value raised to the power of the second value
int power(int x, int y)
{
    int result = 1;

    for (int i = 0; i < y; i++)
    {
        result *= x;
    }

    return result;
}

/* END PASSWORD GEN */

/* UTILITIES */
// returns a sub-array copy of the password_and_hash pairings provided
char ***slice_for_sort(char ***arr, int start_ix, int length)
{
    char ***sub_arr = malloc(length * sizeof(char **));


    int end_ix = start_ix + length;
    for (int i = start_ix, j = 0; i < start_ix + length; i++, j++)
    {
        sub_arr[j] = arr[i];
    }

    return sub_arr;
}

// partial application of substring that can be used on DES based passwords
char *get_salt(char *str)
{
    return substring(str, 0, 2);
}

// returns a copy of just the requested length string
char *substring(char *str, int start_ix, int length)
{
    int str_length = strlen(str);

    if (start_ix > str_length - 1)
    {
        printf("Substring: Invalid string ix");
        return "";
    }

    // calc the endIx, defaults to end of string
    int end_ix = start_ix + length;
    int substr_arr_length = end_ix - start_ix;

    if (end_ix > str_length)
    {
        end_ix = str_length - 1;
        substr_arr_length = end_ix - start_ix + 1;
    }

    // create the return value
    char *result = malloc(substr_arr_length * sizeof(char));
    for (int i = start_ix, j = 0; i < substr_arr_length; i++, j++)
    {
        result[j] = str[i];
    }
    result[substr_arr_length] = '\0';

    return result;
}

// compares to strings starting at the provided character index
// returns 1 if string A should be sorted later, -1 if string A should be sorted earlier
// and 0 if they are the same
int str_compare(char *str_a, char *str_b, int start_ix)
{
    int using_b = 0;
    int length_a = strlen(str_a);
    int length_b = strlen(str_b);

    // use string a as default
    char *iterate_over = str_a;
    char *compare_to = str_b;
    int iterate_length = length_a;
    int compare_to_length = length_b;

    // but if string b is shorter use that
    // as the iteration driver
    if (length_b < length_a)
    {
        iterate_over = str_b;
        compare_to = str_a;
        iterate_length = length_b;
        using_b = 1;
    }

    // compare character by character and if one
    // charcter is found different alphabetically
    // set the correct result to return
    int result = 0;
    for (int i = start_ix; i < iterate_length; i++)
    {
        if (iterate_over[i] < compare_to[i])
        {
            result = -1;
            break;
        }
        else if (iterate_over[i] > compare_to[i])
        {
            result = 1;
            break;
        }
    }

    // if no differences were found character by character
    // check to see if there is a difference in length
    if (result == 0 && length_a != length_b)
    {
        if (using_b)
        {
            result = -1;
        }
        else
        {
            result = 1;
        }
    }

    return result;
}

// returns a 1 if the string's first character is null
int is_empty(char *str)
{
    int result = 0;

    if (str[0] == '\0')
    {
        result = 1;
    }

    return result;
}
/* END UTILITIES */
