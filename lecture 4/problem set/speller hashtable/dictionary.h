// Declares a dictionary's functionality

#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <stdbool.h>

// Maximum length for a word
// (e.g., pneumonoultramicroscopicsilicovolcanoconiosis)
#define LENGTH 45

// Represents number of buckets in a hash table
#define N 26

// Represents a node in a hash table
typedef struct node
{
    char word[LENGTH + 1];
    struct node *next;
}
node;

// Prototypes
bool load(const char *dictionary);
unsigned int size(void);
bool check(const char *word);
bool unload(void);
node *create_node(const char *word, node *next);
bool exists_in_linked_list(const char *word, node *linked_list);
bool str_equal(const char *word1, const char *word2);
char to_lower(char c);
void str_copy(char *destination, const char *source);

#endif // DICTIONARY_H
