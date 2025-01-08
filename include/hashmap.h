#ifndef HASHMAP_H
#define HASHMAP_H

#include "data_structures.h"

typedef struct HashMapNode {
    char key[MAX_TABLE_NAME_LENGTH];
    Table* table;
    struct HashMapNode* next;
} HashMapNode;

typedef struct {
    HashMapNode* buckets[HASHMAP_SIZE];
} HashMap;

unsigned int hash(const char* key);
void hashmap_insert(HashMap* hashmap, const char* key, Table* table);
Table* hashmap_lookup(HashMap* hashmap, const char* key);
void hashmap_remove(HashMap* hashmap, const char* key);

#endif // HASHMAP_H