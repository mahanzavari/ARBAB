#include "hashmap.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

unsigned int hash(const char* key) {
    unsigned int hash_value = 0;
    for (int i = 0; key[i] != '\0'; i++) {
        hash_value = (hash_value * 31) + key[i];
    }
    return hash_value % HASHMAP_SIZE;
}

void hashmap_insert(HashMap* hashmap, const char* key, Table* table) {
    unsigned int index = hash(key);

    HashMapNode* new_node = (HashMapNode*)malloc(sizeof(HashMapNode));
    if (!new_node) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    strcpy(new_node->key, key);
    new_node->table = table;
    new_node->next = NULL;

    if (hashmap->buckets[index] == NULL) {
        hashmap->buckets[index] = new_node;
    } else {
        new_node->next = hashmap->buckets[index];
        hashmap->buckets[index] = new_node;
    }
}

Table* hashmap_lookup(HashMap* hashmap, const char* key) {
    unsigned int index = hash(key);
    HashMapNode* current = hashmap->buckets[index];

    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            return current->table;
        }
        current = current->next;
    }

    return NULL;
}

void hashmap_remove(HashMap* hashmap, const char* key) {
    unsigned int index = hash(key);
    HashMapNode* current = hashmap->buckets[index];
    HashMapNode* prev = NULL;

    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            if (prev == NULL) {
                hashmap->buckets[index] = current->next;
            } else {
                prev->next = current->next;
            }
            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }
}