int is_empty(char *str); // returns 1 if first char in string is '\0', otherwise returns 0
char ***slice_for_sort(char ***arr, int start_ix, int length); // returns a new sub-array
char *get_salt(char *str); // partial application of substring; returns a DES 2 char salt from the provided encoded string
char *substring(char *str, int start_ix, int length); // returns the requested partial string
int str_compare(char *str_a, char *str_b,
                int start_ix); // returns 1 if string A should be sorted later, -1 if str A should be sorted earlier, 0 if equal
char *copy_array(char *str, int length); // creates a duplicate of the provided string
char *copy_str(char *str); // partial application of copy_array specific to string
int power(int x, int y); // raises the first integer to the power of the second
