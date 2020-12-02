// Implements a dictionary's functionality

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "dictionary.h"

// Represents a trie
node *root;
bool is_loaded = false; // flag for size() function
int word_count = 0; // word count for size() function

// Loads dictionary into memory, returning true if successful else false
bool load(const char *dictionary)
{
    // Initialize trie
    root = create_node();

    // Open dictionary
    FILE *file = fopen(dictionary, "r");
    if (file == NULL)
    {
        unload();
        return false;
    }

    // Buffer for a word
    char word[LENGTH + 1];

    // Insert words into trie
    node *trav;
    while (fscanf(file, "%s", word) != EOF)
    {
        trav = root; // reset the traverse pointer to root for a new word

        // iterate each character in the word and follow the
        // trie down to the end, making new nodes where necessary
        int i = 0;
        for (char c = word[i]; c != '\0' && i < LENGTH + 1; i++, c = word[i])
        {
            int child_ix = get_char_ix(c);
            node *next = trav->children[child_ix];

            // if there is no next node yet, create it
            if (next == NULL)
            {
                node *new_node = create_node();
                if (new_node == NULL) // unsuccessful
                {
                    return false;
                }

                trav->children[child_ix] = new_node;
                next = new_node;
            }

            // keep going down the chain
            trav = next;
        }

        // current trav node pointer is the end of the word, so mark it
        // as a word if it hasn't been
        if (!trav->is_word)
        {
            trav->is_word = true;
        }

        // increment the total word count for the size() function
        word_count++;
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
    // TODO
    if (is_loaded)
    {
        return word_count;
    }

    return 0;
}

// Returns true if word is in dictionary else false
bool check(const char *word)
{
    bool result = false;
    node *trav = root;

    // iterate each character in the word and follow the
    // trie down to the end, making new nodes where necessary
    int i = 0;
    for (char c = word[i]; c != '\0' && i < LENGTH + 1; i++, c = word[i])
    {
        int child_ix = get_char_ix(c);
        trav = trav->children[child_ix];

        // if there are still characters and child is NULL
        // then word doesn't exist
        if (trav == NULL)
        {
            break;
        }
    }

    // got through the entire word and found the last node
    // set result to whatever the is_word flag is on the final node
    if (trav != NULL)
    {
        result = trav->is_word;
    }

    return result;
}

// Unloads dictionary from memory, returning true if successful else false
bool unload(void)
{
    free_node(root);
    return true;
}

// returns the node children ix for the provided character
int get_char_ix(const char c)
{
    int ix;

    if (c == '\'') // handle apostrophe
    {
        ix = 26;
    }
    else
    {
        char lower = tolower(c); // make sure it's lower case, spelling isn't case sensitive
        ix = lower - 'a';
    }

    return ix;
}

// creates a new tries node
node *create_node()
{
    node *new_node = malloc(sizeof(node));
    if (new_node == NULL)
    {
        printf("Failed to create new node: out of memory");
        return NULL;
    }

    new_node->is_word = false;

    // make sure the children array is filled with NULL values
    for (int i = 0; i < N; i++)
    {
        new_node->children[i] = NULL;
    }

    return new_node;
}

// releases nodes created by create_node()
void free_node(node *node)
{
    // make sure all the children have been freed first
    // otherwise pointers are lost
    for (int i = 0; i < N; i++)
    {
        struct node *child_node = node->children[i];

        if (child_node != NULL)
        {
            free_node(child_node);
        }
    }

    // free this node once all children have been
    free(node);
}
