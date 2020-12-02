// Implements a dictionary's functionality

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "dictionary.h"

// Represents a hash table
node *hashtable[N];

// keeps track of words for size()
bool is_loaded = false;
int word_count = 0;
int case_diff = 'a' - 'A';

// Hashes word to a number between 0 and 25, inclusive, based on its first letter
unsigned int hash(const char *word)
{
    return tolower(word[0]) - 'a';
}

// Loads dictionary into memory, returning true if successful else false
bool load(const char *dictionary)
{
    // Initialize hash table
    for (int i = 0; i < N; i++)
    {
        hashtable[i] = NULL;
    }

    // Open dictionary
    FILE *file = fopen(dictionary, "r");
    if (file == NULL)
    {
        unload();
        return false;
    }

    // Buffer for a word
    char word[LENGTH + 1];

    // Insert words into hash table
    while (fscanf(file, "%s", word) != EOF)
    {
        // compute the hash for the word
        unsigned int h = hash(word);

        // insert the word into the linked list
        node *hash_node = hashtable[h];
        if (hash_node == NULL)
        {
            hashtable[h] = create_node(word, NULL);
        }
        else
        {
            hashtable[h] = create_node(word, hash_node); // insert the word to the front
        }

        if (hashtable[h] == NULL)
        {
            // something went wrong allocating and assigning the new node
            return false;
        }

        word_count++; // increment word count for size()
    }

    // Close dictionary
    fclose(file);

    // Indicate success
    is_loaded = true;
    return true;
}

// Returns number of words in dictionary if loaded else 0 if not yet loaded
unsigned int size(void)
{
    if (is_loaded)
    {
        return word_count;
    }

    return 0;
}

// Returns true if word is in dictionary else false
bool check(const char *word)
{
    // get the hash
    int h = hash(word);
    node *list = hashtable[h];

    return exists_in_linked_list(word, list);
}

// Unloads dictionary from memory, returning true if successful else false
bool unload(void)
{
    for (int i = 0; i < N; i++)
    {
        free(hashtable[i]);
    }

    return true;
}

node *create_node(const char *word, node *next)
{
    node *new_node = malloc(sizeof(node));
    if (new_node == NULL)
    {
        printf("Failed to create new node: out of memory");
        return NULL;
    }

    str_copy(new_node->word, word);
    new_node->next = next;

    return new_node;
}

void free_node(node *ptr)
{
    if (ptr == NULL)
    {
        return;
    }

    free(ptr->next); // first free any children so we don't orphan them
    free(ptr);
}

bool exists_in_linked_list(const char *word, node *linked_list)
{
    bool result = false;

    for (node *n = linked_list; n != NULL; n = n->next)
    {
        // if a linked-list's node's word is equivalent to the search
        // then it exists
        if (str_equal(word, n->word))
        {
            result = true;
            break;
        }
    }

    return result;
}

// case insensitive comparison
bool str_equal(const char *word1, const char *word2)
{
    for (int i = 0; i < LENGTH + 1; i++)
    {
        char c1 = word1[i];
        char c2 = word2[i];

        if (to_lower(c1) != to_lower(c2))
        {
            return false;
        }

        // if the chars are the same, but they're both NULL
        // both strings have ended at same time and words are equal
        if (c1 == '\0' && c2 == '\0')
        {
            break;
        }
    }

    return true;
}

char to_lower(char c)
{
    if ('A' <= c && c <= 'Z')
    {
        return c + case_diff;
    }

    return c;
}

void str_copy(char *destination, const char *source)
{
    int i = 0;
    do
    {
        destination[i] = source[i];
        i++;
    }
    while (destination[i - 1] != '\0');
}
