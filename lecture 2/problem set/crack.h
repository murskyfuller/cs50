void sort_by_encoded(char ***arr,
                     int length); // sorts the provided array of clear and hash pairings by the hash values alphabetically
char *find_password(char *target); // binary search of the sorted global all_passwords array based on the hash values
int validate_argc(int argc); // prints human readable message and returns error integer value if args don't match requirements
void clean_up(); // frees up any memory allocated during the crack.c program run
