#include "../include/bplustree.h"
#include "../include/record.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Create a new B+ Tree
BPTree* bptree_create(int order) {
    BPTree* tree = (BPTree*)malloc(sizeof(BPTree));
    if (!tree) {
        perror("Memory allocation failed");
        return NULL;
    }
    tree->order = order;
    tree->root = bptree_create_node(true);
    return tree;
}

// Create a new B+ Tree node
BPTreeNode* bptree_create_node(bool is_leaf) {
    BPTreeNode* node = (BPTreeNode*)malloc(sizeof(BPTreeNode));
    if (!node) {
        perror("Memory allocation failed");
        return NULL;
    }
    
    node->keys = (int*)malloc(BPTREE_ORDER * sizeof(int));
    node->children = (BPTreeNode**)malloc((BPTREE_ORDER + 1) * sizeof(BPTreeNode*));
    node->records = (struct Record**)malloc(BPTREE_ORDER * sizeof(struct Record*));
    
    if (!node->keys || !node->children || !node->records) {
        perror("Memory allocation failed");
        free(node->keys);
        free(node->children);
        free(node->records);
        free(node);
        return NULL;
    }
    
    node->num_keys = 0;
    node->is_leaf = is_leaf;
    node->next = NULL;
    node->parent = NULL;
    
    // Initialize arrays
    for (int i = 0; i < BPTREE_ORDER; i++) {
        node->keys[i] = 0;
        node->records[i] = NULL;
    }
    for (int i = 0; i <= BPTREE_ORDER; i++) {
        node->children[i] = NULL;
    }
    
    return node;
}

// Find the index where a key should be inserted or where it exists
int bptree_find_key_index(BPTreeNode* node, int key) {
    int idx = 0;
    while (idx < node->num_keys && node->keys[idx] < key) {
        idx++;
    }
    return idx;
}

// Search for a record with a given key
struct Record* bptree_search(BPTree* tree, int key) {
    if (!tree || !tree->root) {
        return NULL;
    }
    
    BPTreeNode* node = tree->root;
    
    // Traverse down to the leaf node
    while (!node->is_leaf) {
        int idx = bptree_find_key_index(node, key);
        node = node->children[idx];
    }
    
    // Search in the leaf node
    int idx = bptree_find_key_index(node, key);
    if (idx < node->num_keys && node->keys[idx] == key) {
        return node->records[idx];
    }
    
    return NULL;
}

// Insert into a non-full node
void bptree_insert_non_full(BPTreeNode* node, int key, struct Record* record) {
    int i = node->num_keys - 1;
    
    if (node->is_leaf) {
        // Insert key and record into leaf node
        while (i >= 0 && node->keys[i] > key) {
            node->keys[i + 1] = node->keys[i];
            node->records[i + 1] = node->records[i];
            i--;
        }
        node->keys[i + 1] = key;
        node->records[i + 1] = record;
        node->num_keys++;
    } else {
        // Find child to insert into
        while (i >= 0 && node->keys[i] > key) {
            i--;
        }
        i++;
        
        BPTreeNode* child = node->children[i];
        
        // If child is full, split it
        if (child->num_keys == BPTREE_ORDER) {
            bptree_split_child(node, i, child);
            
            // After split, determine which child to insert into
            if (node->keys[i] < key) {
                i++;
            }
        }
        
        bptree_insert_non_full(node->children[i], key, record);
    }
}

// Split a full child node
BPTreeNode* bptree_split_child(BPTreeNode* parent, int index, BPTreeNode* child) {
    int mid = BPTREE_ORDER / 2;
    
    // Create new node with same leaf status as child
    BPTreeNode* new_node = bptree_create_node(child->is_leaf);
    new_node->parent = parent;
    
    if (child->is_leaf) {
        // For leaf nodes, copy the right half
        new_node->num_keys = BPTREE_ORDER - mid;
        for (int j = 0; j < new_node->num_keys; j++) {
            new_node->keys[j] = child->keys[mid + j];
            new_node->records[j] = child->records[mid + j];
        }
        
        // Update the number of keys in the original node
        child->num_keys = mid;
        
        // Link the leaf nodes
        new_node->next = child->next;
        child->next = new_node;
        
        // The key to move up is the first key of the new node
        int key_to_move_up = new_node->keys[0];
        
        // Insert the new key into parent
        for (int j = parent->num_keys; j > index; j--) {
            parent->keys[j] = parent->keys[j - 1];
            parent->children[j + 1] = parent->children[j];
        }
        parent->keys[index] = key_to_move_up;
        parent->children[index + 1] = new_node;
        parent->num_keys++;
    } else {
        // For internal nodes
        new_node->num_keys = BPTREE_ORDER - mid - 1;
        
        // Copy keys and children
        for (int j = 0; j < new_node->num_keys; j++) {
            new_node->keys[j] = child->keys[mid + 1 + j];
        }
        for (int j = 0; j <= new_node->num_keys; j++) {
            new_node->children[j] = child->children[mid + 1 + j];
            if (new_node->children[j]) {
                new_node->children[j]->parent = new_node;
            }
        }
        
        // The middle key moves up to parent
        int key_to_move_up = child->keys[mid];
        child->num_keys = mid;
        
        // Insert the new key into parent
        for (int j = parent->num_keys; j > index; j--) {
            parent->keys[j] = parent->keys[j - 1];
            parent->children[j + 1] = parent->children[j];
        }
        parent->keys[index] = key_to_move_up;
        parent->children[index + 1] = new_node;
        parent->num_keys++;
    }
    
    return new_node;
}

// Insert a key-record pair into the B+ Tree
void bptree_insert(BPTree* tree, int key, struct Record* record) {
    if (!tree || !tree->root) {
        return;
    }
    
    BPTreeNode* root = tree->root;
    
    // If root is full, split it
    if (root->num_keys == BPTREE_ORDER) {
        BPTreeNode* new_root = bptree_create_node(false);
        new_root->children[0] = root;
        root->parent = new_root;
        
        bptree_split_child(new_root, 0, root);
        tree->root = new_root;
        
        bptree_insert_non_full(new_root, key, record);
    } else {
        bptree_insert_non_full(root, key, record);
    }
}

// Range search: find all records with keys in [start_key, end_key]
struct Record** bptree_range_search(BPTree* tree, int start_key, int end_key, int* count) {
    *count = 0;
    if (!tree || !tree->root) {
        return NULL;
    }
    
    // Find the starting leaf node
    BPTreeNode* node = tree->root;
    while (!node->is_leaf) {
        int idx = bptree_find_key_index(node, start_key);
        node = node->children[idx];
    }
    
    // Count matching records first
    BPTreeNode* temp = node;
    int temp_count = 0;
    while (temp) {
        for (int i = 0; i < temp->num_keys; i++) {
            if (temp->keys[i] >= start_key && temp->keys[i] <= end_key) {
                temp_count++;
            } else if (temp->keys[i] > end_key) {
                break;
            }
        }
        if (temp->num_keys > 0 && temp->keys[temp->num_keys - 1] > end_key) {
            break;
        }
        temp = temp->next;
    }
    
    if (temp_count == 0) {
        return NULL;
    }
    
    // Allocate array for results
    struct Record** results = (struct Record**)malloc(temp_count * sizeof(struct Record*));
    if (!results) {
        return NULL;
    }
    
    // Collect matching records
    int result_idx = 0;
    while (node && result_idx < temp_count) {
        for (int i = 0; i < node->num_keys && result_idx < temp_count; i++) {
            if (node->keys[i] >= start_key && node->keys[i] <= end_key) {
                results[result_idx++] = node->records[i];
            } else if (node->keys[i] > end_key) {
                *count = result_idx;
                return results;
            }
        }
        node = node->next;
    }
    
    *count = result_idx;
    return results;
}

// Delete a key from the B+ Tree (simplified version)
void bptree_delete(BPTree* tree, int key) {
    if (!tree || !tree->root) {
        return;
    }
    
    // Find the leaf node containing the key
    BPTreeNode* node = tree->root;
    while (!node->is_leaf) {
        int idx = bptree_find_key_index(node, key);
        node = node->children[idx];
    }
    
    // Find and remove the key from the leaf
    int idx = bptree_find_key_index(node, key);
    if (idx < node->num_keys && node->keys[idx] == key) {
        // Shift keys and records
        for (int i = idx; i < node->num_keys - 1; i++) {
            node->keys[i] = node->keys[i + 1];
            node->records[i] = node->records[i + 1];
        }
        node->num_keys--;
    }
    
    // Note: This is a simplified deletion that doesn't handle underflow
    // A full implementation would need to handle merging and redistribution
}

// Free a node and its children
void bptree_free_node(BPTreeNode* node) {
    if (!node) {
        return;
    }
    
    if (!node->is_leaf) {
        for (int i = 0; i <= node->num_keys; i++) {
            bptree_free_node(node->children[i]);
        }
    }
    
    free(node->keys);
    free(node->children);
    free(node->records);
    free(node);
}

// Free the entire B+ Tree
void bptree_free(BPTree* tree) {
    if (!tree) {
        return;
    }
    
    bptree_free_node(tree->root);
    free(tree);
}
