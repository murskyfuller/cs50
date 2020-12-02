#include <cs50.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef enum { ANY, INVALID, AMEX, MASTERCARD, VISA } cc_type;

char *validate_cc_company(long number);
char *cc_company(short *cc_array, short length);
bool is_amex(short *cc_array, short length);
bool is_mastercard(short *cc_array, short length);
bool is_visa(short *cc_array, short length);
bool is_valid_cc_for_type(short *cc_array, short length, cc_type type);
bool does_pass_luhns(short *cc_array, short length);
bool has_correct_start_digits(short *cc_array, short length, cc_type type);
bool has_valid_length(short length, cc_type type);
short *to_array(long cc_number, short length);
short int_length(long n);

const bool DEBUG = false;
const cc_type cc_invalid = INVALID;
const cc_type cc_amex = AMEX;
const cc_type cc_mastercard = MASTERCARD;
const cc_type cc_visa = VISA;
const cc_type cc_any = ANY;

int main(void)
{
    long cc_number = get_long("Number: ");

    char *result = validate_cc_company(cc_number);

    printf("%s\n", result);
}

// returns the name the company name of valid credit card number or INVALID
char *validate_cc_company(long number)
{
    char *result = "INVALID";
    short length = int_length(number);

    if (DEBUG == true)
    {
        printf("length: %i\n", length);
    }

    if (has_valid_length(length, cc_any)) // this can help eliminate phone numbers, all number zip codes, etc
    {
        short *cc_array = to_array(number, length);
        char *company = cc_company(cc_array, length); // this is less expensive to run than luhns so start here

        if (DEBUG == true)
        {
            printf("company is %s\n", company);
        }

        if (strcmp(company, "INVALID") != 0) // if it looks like it could be a valid company card, then run luhns as a deeper check
        {
            if (does_pass_luhns(cc_array, length))
            {
                result = company;
            }
        }

        // free the dynamically allocated memory for the array
        free(cc_array);
    }

    return result;
}

// validates the provided number array with Luhn's Algorithm; cc array passed in is digits in reverse order
bool does_pass_luhns(short *cc_array, short length)
{
    // since the array is in reverse order, we don't have to worry about different lengths of CC numbers
    // we can simple start at the beginning of the array and iterate
    int doubled = 0, remainder = 0, doubled_sum = 0, simple_sum = 0;

    for (short i = 0; i < length; i++)
    {
        if (i % 2 == 0) // even i's are part of simple_sum
        {
            simple_sum += cc_array[i];
        }
        else
        {
            doubled = 2 * cc_array[i];

            while (doubled > 0) // add the products' digits to the doubled_sum
            {
                remainder = doubled % 10;
                doubled_sum += remainder;
                doubled /= 10;
            };
        }
    }

    int total = doubled_sum + simple_sum;

    if (DEBUG == true)
    {
        printf("luhn's total: %i | %i | %i\n", total, doubled_sum, simple_sum);
    }

    return total % 10 == 0;
}

// returns the name of the cc company if it passes validation
char *cc_company(short *cc_array, short length)
{
    if (DEBUG == true)
    {
        printf("getting company name\n");
    }

    char *result = "INVALID";

    if (is_amex(cc_array, length))
    {
        result = "AMEX";
    }
    else if (is_mastercard(cc_array, length))
    {
        result = "MASTERCARD";
    }
    else if (is_visa(cc_array, length))
    {
        result = "VISA";
    }

    return result;
}

// partial application of is_valid_cc_for_type; checks to see if the number meets requirements to be an American Express card
bool is_amex(short *cc_array, short length)
{
    if (DEBUG == true)
    {
        printf("checking if amex\n");
    }

    return is_valid_cc_for_type(cc_array, length, cc_amex);
}

// partial application of is_valid_cc_for_type; checks to see if the number meets requirements to be a Mastercard card
bool is_mastercard(short *cc_array, short length)
{
    if (DEBUG == true)
    {
        printf("checking if mastercard\n");
    }

    return is_valid_cc_for_type(cc_array, length, cc_mastercard);
}

// partial application of is_valid_cc_for_type; checks to see if the number meets requirements to be a Visa card
bool is_visa(short *cc_array, short length)
{
    if (DEBUG == true)
    {
        printf("checking if visa\n");
    }

    return is_valid_cc_for_type(cc_array, length, cc_visa);
}


// checks to see if the number meets requirements for the provided card type
bool is_valid_cc_for_type(short *cc_array, short length, cc_type type)
{
    if (DEBUG == true)
    {
        printf("checking if valid for type %i\n", type);
    }

    bool result = false;

    if (has_valid_length(length, type) && has_correct_start_digits(cc_array, length, type))
    {
        result = true;
    }

    return result;
}

// checks to see if the correct starting digits exist for the designated
// type of card; the array passed in is in reverse order
bool has_correct_start_digits(short *cc_array, short length, cc_type type)
{
    if (DEBUG == true)
    {
        printf("checking start digits: %i\n", type);
    }

    // info for reference: https://en.wikipedia.org/wiki/Payment_card_number
    bool result = false;

    // grab any relevant digits from the array, which is in reverse order
    short first_digit = cc_array[length - 1];
    short second_digit = cc_array[length - 2];
    short third_digit = cc_array[length - 3];
    short fourth_digit = cc_array[length - 4];

    if (DEBUG == true)
    {
        printf("digits: %i %i %i %i\n", first_digit, second_digit, third_digit, fourth_digit);
    }

    // make some quick single int references to make the checks below more legible
    short two_digits = (first_digit * 10) + second_digit;
    int four_digits = (first_digit * 1000) + (second_digit * 100) + (third_digit * 10) + fourth_digit;

    if (DEBUG == true)
    {
        printf("two: %i\n", two_digits);
        printf("four: %i\n", four_digits);
    }

    switch (type)
    {
        case cc_amex: // starts with 34 or 37
            if (two_digits == 34 || two_digits == 37)
            {
                if (DEBUG == true)
                {
                    printf("has correct AMEX start: %i\n", two_digits);
                }

                result = true;
            }
            break;
        case cc_mastercard:
            if (two_digits > 50 && two_digits < 56) // this checks for the mastercard numbers discussed in problem description: 51-55
            {
                if (DEBUG == true)
                {
                    printf("has correct MASTERCARD start: %i\n", two_digits);
                }

                result = true;
            }
            else if (four_digits > 2220 && four_digits < 2721) // this checks for new mastercard numbers starting 2017: 2221-2720;
            {
                if (DEBUG == true)
                {
                    printf("has correct AMEX start: %i\n", two_digits);
                }

                result = true;
            }
            break;
        case cc_visa:
            if (first_digit == 4)
            {
                if (DEBUG == true)
                {
                    printf("has correct VISA start: %i\n", two_digits);
                }

                result = true;
            }
            break;
        default:
            break;
    }

    return result;
}

// validates the length of the number based on the credit card type
bool has_valid_length(short length, cc_type type)
{
    // the smallest valid CC should be 12 digits long
    // the largest valid CC should be 19 digits long
    // https://en.wikipedia.org/wiki/Payment_card_number
    // this does not solve the issue of international numbers
    // e.g. UK phone numbers can be up to 15 digits

    bool result = false;

    switch (type)
    {
        case cc_amex:
            result = length == 15;
            break;
        case cc_mastercard:
            result = length == 16;
            break;
        case cc_visa:
            result = length == 16 || length == 13;
            break;
        default:
            result = length > 11 && length < 20;
    }

    return result;
}

// converts the cc_number long into an array of individual shorts in reverse order
short *to_array(long cc_number, short length)
{
    short *result = (short *) malloc(length * sizeof(short)); // dynamically allocate memory, so it doesn't get collected
    short remainder = 0;
    long number = cc_number;
    for (short i = 0; i < length; i++)
    {
        remainder = number % 10;
        result[i] = remainder;
        number = number / 10;
    }

    return result;
}

// this is the easiest way to check the length
// of an integer
short int_length(long n)
{
    return floor(log10(n) + 1);
}
