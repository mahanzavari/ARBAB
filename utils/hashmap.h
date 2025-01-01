#ifndef HASHMAP_H
#define HASHMAP_H

#include "table.h"

// Define a generic hashmap entry
typedef struct HashEntry {
    char* key;              // Table name (or other identifiers)
    void* value;            // Pointer to a Table or any other object
    struct HashEntry* next; // For handling collisions via chaining
} HashEntry;

// Define the hashmap structure
typedef struct {
    HashEntry** buckets; // Array of linked lists for collision resolution
    int size;            // Number of buckets
} HashMap;

HashMap* create_hashmap(int size);
void free_hashmap(HashMap* map);
void hashmap_insert(HashMap* map, const char* key, void* value);
void* hashmap_get(HashMap* map, const char* key);
void hashmap_remove(HashMap* map, const char* key);

#endif
