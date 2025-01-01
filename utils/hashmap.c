#include "hashmap.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Hash function for strings
static unsigned int hash(const char* key, int size) {
    unsigned int hash = 0;
    while (*key) {
        hash = (hash * 31) + *key++;
    }
    return hash % size;
}

// Create a new hashmap
HashMap* create_hashmap(int size) {
    HashMap* map = (HashMap*)malloc(sizeof(HashMap));
    if (!map) {
        perror("Failed to allocate hashmap");
        exit(EXIT_FAILURE);
    }
    map->buckets = (HashEntry**)calloc(size, sizeof(HashEntry*));
    if (!map->buckets) {
        perror("Failed to allocate hashmap buckets");
        free(map);
        exit(EXIT_FAILURE);
    }
    map->size = size;
    return map;
}

// Free the hashmap
void free_hashmap(HashMap* map) {
    for (int i = 0; i < map->size; i++) {
        HashEntry* entry = map->buckets[i];
        while (entry) {
            HashEntry* next = entry->next;
            free(entry->key);
            free(entry);
            entry = next;
        }
    }
    free(map->buckets);
    free(map);
}

// Insert a key-value pair
void hashmap_insert(HashMap* map, const char* key, void* value) {
    unsigned int bucket = hash(key, map->size);
    HashEntry* new_entry = (HashEntry*)malloc(sizeof(HashEntry));
    if (!new_entry) {
        perror("Failed to allocate hashmap entry");
        exit(EXIT_FAILURE);
    }
    new_entry->key = strdup(key);
    new_entry->value = value;
    new_entry->next = map->buckets[bucket];
    map->buckets[bucket] = new_entry;
}

// Retrieve a value by key
void* hashmap_get(HashMap* map, const char* key) {
    unsigned int bucket = hash(key, map->size);
    HashEntry* entry = map->buckets[bucket];
    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            return entry->value;
        }
        entry = entry->next;
    }
    return NULL; // Not found
}

// Remove a key-value pair
void hashmap_remove(HashMap* map, const char* key) {
    unsigned int bucket = hash(key, map->size);
    HashEntry* entry = map->buckets[bucket];
    HashEntry* prev = NULL;
    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            if (prev) {
                prev->next = entry->next;
            } else {
                map->buckets[bucket] = entry->next;
            }
            free(entry->key);
            free(entry);
            return;
        }
        prev = entry;
        entry = entry->next;
    }
}
