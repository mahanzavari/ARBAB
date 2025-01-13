#ifndef HASHMAP_H
#define HASHMAP_H
#include "table.h"


#define HASHMAP_SIZE 10

typedef struct HashMapNode {
    char key[MAX_TABLE_NAME_LENGTH]; // Table name
    Table* table;                    // Pointer to the table
    struct HashMapNode* next;        // Pointer to the next node in the chain
} HashMapNode;

typedef struct {
    HashMapNode* buckets[HASHMAP_SIZE]; // Array of linked lists
} HashMap;
unsigned int hash(const char* key);
void hashmap_insert(HashMap* hashmap, const char* key, Table* table);
Table* hashmap_lookup(HashMap* hashmap, const char* key);
void hashmap_remove(HashMap* hashmap, const char* key);

extern HashMap hashmap;

#endif