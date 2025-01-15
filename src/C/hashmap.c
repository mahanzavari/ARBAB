#include "../include/hashmap.h"
#include <stdio.h>
#include <stdlib.h>
#define _POSIX_C_SOURCE 200809L  // Add this line before including string.h
#include <string.h>              // Ensure this is included in command.c
HashMap hashmap;

// hash function
unsigned int hash(const char* key) {
    unsigned int hash_value = 0;
    for (int i = 0; key[i] != '\0'; i++) {
        hash_value = (hash_value * 31) + key[i];
    }
    return hash_value % HASHMAP_SIZE;
}

void hashmap_insert(HashMap* hashmap, const char* key, Table* table) {
    unsigned int index = hash(key);

    // Create a new node
    HashMapNode* new_node = (HashMapNode*)malloc(sizeof(HashMapNode));
    if (!new_node) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    strcpy(new_node->key, key);
    new_node->table = table;
    new_node->next = NULL;

    // Insert at the head of the linked list
    if (hashmap->buckets[index] == NULL) {
        hashmap->buckets[index] = new_node;
    } else {
        new_node->next = hashmap->buckets[index];
        hashmap->buckets[index] = new_node;
    }
}
// Find a table
Table* hashmap_lookup(HashMap* hashmap, const char* key) {
    unsigned int index = hash(key);
    HashMapNode* current = hashmap->buckets[index];

    // Traverse the linked list
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            return current->table; // Table found
        }
        current = current->next;
    }

    return NULL; // Table not found
}
//remove table
void hashmap_remove(HashMap* hashmap, const char* key) {
    unsigned int index = hash(key);
    HashMapNode* current = hashmap->buckets[index];
    HashMapNode* prev = NULL;

    // Traverse the linked list
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            // Remove the node
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