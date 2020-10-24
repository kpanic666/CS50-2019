// Implements a dictionary's functionality

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "dictionary.h"

// Represents number of children for each node in a trie
#define N 27

// Represents a node in a trie
typedef struct node
{
    bool is_word;
    struct node *children[N];
}
node;

void unloadNode(node *n);

// Represents a trie
node *root;
int dictSize = 0;

// Loads dictionary into memory, returning true if successful else false
bool load(const char *dictionary)
{
    // Initialize trie
    root = malloc(sizeof(node));
    if (root == NULL)
    {
        return false;
    }
    root->is_word = false;
    for (int i = 0; i < N; i++)
    {
        root->children[i] = NULL;
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

    // Insert words into trie
    while (fscanf(file, "%s", word) != EOF)
    {
        bool insRes = insertWord(word);
        if (!insRes)
        {
            unload();
            fclose(file);
            return false;
        }
        else
        {
            dictSize++;
        }
    }

    // Close dictionary
    fclose(file);

    // Indicate success
    return true;
}

// Return char index, which will be used to store pointers in trie
// '\'' - 26 index, 'a' - 0 index, 'b' - 1 index and etc...
unsigned int getCharInd(char c)
{
    return (isalpha(c)) ? c - 'a' : N - 1;
}

// Insert word in to the dictionary. True - if it's ok.
bool insertWord(const char *word)
{
    int chInd = 0;
    node *curNode = root;

    for (int i = 0, n = strlen(word); i < n; i++)
    {
        chInd = getCharInd(word[i]);

        if (curNode->children[chInd] == NULL)
        {
            curNode = curNode->children[chInd] = malloc(sizeof(node));
            if (curNode == NULL)
            {
                return false;
            }
            curNode->is_word = false;
            for (int j = 0; j < N; j++)
            {
                curNode->children[j] = NULL;
            }
        }
        else
        {
            curNode = curNode->children[chInd];
        }
    }

    curNode->is_word = true;
    return true;
}

// Returns number of words in dictionary if loaded else 0 if not yet loaded
unsigned int size(void)
{
    return dictSize;
}

// Returns true if word is in dictionary else false
bool check(const char *word)
{
    int chInd = 0;
    char w;
    node *curNode = root;

    for (int i = 0, n = strlen(word); i < n; i++)
    {
        w = (isalpha(word[i])) ? tolower(word[i]) : word[i];
        chInd = getCharInd(w);

        if (curNode->children[chInd] == NULL)
        {
            return false;
        }
        curNode = curNode->children[chInd];
    }

    return curNode->is_word ? true : false;
}

// Unloads dictionary from memory, returning true if successful else false
bool unload(void)
{
    unloadNode(root);
    return true;
}

void unloadNode(node *n)
{
    for (int i = 0; i < N; i++)
    {
        if (n->children[i])
        {
            unloadNode(n->children[i]);
        }
    }
    free(n);
}
