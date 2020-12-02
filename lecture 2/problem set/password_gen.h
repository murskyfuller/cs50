void populate_password_permutations();
void generate_permutations_with_repeat(const char *char_set, int max_length); // generates all possible permutations
void passwords_for_char_and_length(int str_ix, int char_ix, int length,
                                   char *str); // generates passwords starting with the specific char from the global char_set
int calc_total_perms_with_rep(int set_length,
                              int max_str_length); // calculates the number of possible permutations with repeating characters
