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

// Delete a key from the B+ Tree (complete implementation with underflow handling)
void bptree_delete(BPTree* tree, int key) {
    if (!tree || !tree->root) {
        return;
    }
    
    bptree_delete_from_node(tree->root, key);
    
    // If root becomes empty after deletion, make its only child the new root
    if (tree->root->num_keys == 0 && !tree->root->is_leaf) {
        BPTreeNode* old_root = tree->root;
        tree->root = tree->root->children[0];
        if (tree->root) {
            tree->root->parent = NULL;
        }
        free(old_root->keys);
        free(old_root->children);
        free(old_root->records);
        free(old_root);
    }
}

// Delete a key from a node (recursive)
void bptree_delete_from_node(BPTreeNode* node, int key) {
    int idx = bptree_find_key_index(node, key);
    
    if (node->is_leaf) {
        // Delete from leaf node
        if (idx < node->num_keys && node->keys[idx] == key) {
            for (int i = idx; i < node->num_keys - 1; i++) {
                node->keys[i] = node->keys[i + 1];
                node->records[i] = node->records[i + 1];
            }
            node->num_keys--;
        }
    } else {
        // Internal node: recurse to appropriate child
        BPTreeNode* child = node->children[idx];
        bptree_delete_from_node(child, key);
        
        // Handle underflow in child (only if it has a parent)
        if (child->num_keys < MIN_KEYS && child->parent != NULL) {
            // Try to borrow from left sibling
            if (idx > 0 && node->children[idx - 1]->num_keys > MIN_KEYS) {
                bptree_borrow_from_left(node, idx);
            }
            // Try to borrow from right sibling
            else if (idx < node->num_keys && node->children[idx + 1]->num_keys > MIN_KEYS) {
                bptree_borrow_from_right(node, idx);
            }
            // Merge with left sibling
            else if (idx > 0) {
                bptree_merge_nodes(node, idx - 1, node->children[idx - 1], child);
            }
            // Merge with right sibling
            else if (idx < node->num_keys) {
                bptree_merge_nodes(node, idx, child, node->children[idx + 1]);
            }
        }
    }
}

// Borrow a key from the left sibling
void bptree_borrow_from_left(BPTreeNode* parent, int idx) {
    BPTreeNode* child = parent->children[idx];
    BPTreeNode* left_sibling = parent->children[idx - 1];
    
    if (child->is_leaf) {
        // Shift all keys and records in child to the right
        for (int i = child->num_keys; i > 0; i--) {
            child->keys[i] = child->keys[i - 1];
            child->records[i] = child->records[i - 1];
        }
        
        // Move the last key from left sibling to child
        child->keys[0] = left_sibling->keys[left_sibling->num_keys - 1];
        child->records[0] = left_sibling->records[left_sibling->num_keys - 1];
        child->num_keys++;
        left_sibling->num_keys--;
        
        // Update parent key
        parent->keys[idx - 1] = child->keys[0];
    } else {
        // Shift all keys and children in child to the right
        for (int i = child->num_keys; i > 0; i--) {
            child->keys[i] = child->keys[i - 1];
        }
        for (int i = child->num_keys + 1; i > 0; i--) {
            child->children[i] = child->children[i - 1];
        }
        
        // Move key from parent to child
        child->keys[0] = parent->keys[idx - 1];
        child->children[0] = left_sibling->children[left_sibling->num_keys];
        if (child->children[0]) {
            child->children[0]->parent = child;
        }
        child->num_keys++;
        
        // Move key from left sibling to parent
        parent->keys[idx - 1] = left_sibling->keys[left_sibling->num_keys - 1];
        left_sibling->num_keys--;
    }
}

// Borrow a key from the right sibling
void bptree_borrow_from_right(BPTreeNode* parent, int idx) {
    BPTreeNode* child = parent->children[idx];
    BPTreeNode* right_sibling = parent->children[idx + 1];
    
    if (child->is_leaf) {
        // Move the first key from right sibling to child
        child->keys[child->num_keys] = right_sibling->keys[0];
        child->records[child->num_keys] = right_sibling->records[0];
        child->num_keys++;
        
        // Shift all keys and records in right sibling to the left
        for (int i = 0; i < right_sibling->num_keys - 1; i++) {
            right_sibling->keys[i] = right_sibling->keys[i + 1];
            right_sibling->records[i] = right_sibling->records[i + 1];
        }
        right_sibling->num_keys--;
        
        // Update parent key
        parent->keys[idx] = right_sibling->keys[0];
    } else {
        // Move key from parent to child
        child->keys[child->num_keys] = parent->keys[idx];
        child->children[child->num_keys + 1] = right_sibling->children[0];
        if (child->children[child->num_keys + 1]) {
            child->children[child->num_keys + 1]->parent = child;
        }
        child->num_keys++;
        
        // Move key from right sibling to parent
        parent->keys[idx] = right_sibling->keys[0];
        
        // Shift all keys and children in right sibling to the left
        for (int i = 0; i < right_sibling->num_keys - 1; i++) {
            right_sibling->keys[i] = right_sibling->keys[i + 1];
        }
        for (int i = 0; i < right_sibling->num_keys; i++) {
            right_sibling->children[i] = right_sibling->children[i + 1];
        }
        right_sibling->num_keys--;
    }
}

// Merge two nodes
void bptree_merge_nodes(BPTreeNode* parent, int idx, BPTreeNode* left, BPTreeNode* right) {
    if (left->is_leaf) {
        // Merge leaf nodes
        for (int i = 0; i < right->num_keys; i++) {
            left->keys[left->num_keys + i] = right->keys[i];
            left->records[left->num_keys + i] = right->records[i];
        }
        left->num_keys += right->num_keys;
        left->next = right->next;
    } else {
        // Merge internal nodes: move key from parent down
        left->keys[left->num_keys] = parent->keys[idx];
        left->num_keys++;
        
        // Copy keys and children from right to left
        for (int i = 0; i < right->num_keys; i++) {
            left->keys[left->num_keys + i] = right->keys[i];
        }
        for (int i = 0; i <= right->num_keys; i++) {
            left->children[left->num_keys + i] = right->children[i];
            if (left->children[left->num_keys + i]) {
                left->children[left->num_keys + i]->parent = left;
            }
        }
        left->num_keys += right->num_keys;
    }
    
    // Remove the key from parent
    for (int i = idx; i < parent->num_keys - 1; i++) {
        parent->keys[i] = parent->keys[i + 1];
        parent->children[i + 1] = parent->children[i + 2];
    }
    parent->num_keys--;
    
    // Free the right node
    free(right->keys);
    free(right->children);
    free(right->records);
    free(right);
}

// Bulk load: efficient way to load sorted data into B+ Tree
void bptree_bulk_load(BPTree* tree, int* keys, struct Record** records, int count) {
    if (!tree || count == 0) {
        return;
    }
    
    // For simplicity, use regular insertion
    // A true bulk load would build the tree bottom-up for better performance
    for (int i = 0; i < count; i++) {
        bptree_insert(tree, keys[i], records[i]);
    }
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
