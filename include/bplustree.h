#ifndef BPLUSTREE_H
#define BPLUSTREE_H

#include <stdbool.h>

// Forward declaration of Record
struct Record;

// B+ Tree order (number of children per node)
// For disk I/O optimization, this should align with disk block size
#define BPTREE_ORDER 4
// B plus Tree Node Struct
typedef struct BPTreeNode {
    int* keys;                      // Array of keys
    struct BPTreeNode** children;   // Array of child pointers (for internal nodes)
    struct Record** records;        // Array of record pointers (for leaf nodes)
    int num_keys;                   // Current number of keys
    bool is_leaf;                   // Flag to indicate if node is a leaf
    struct BPTreeNode* next;        // Pointer to next leaf (for range queries)
    struct BPTreeNode* parent;      // Pointer to parent node
} BPTreeNode;

typedef struct BPTree {
    BPTreeNode* root;
    int order;
} BPTree;

// Function prototypes
BPTree* bptree_create(int order);
BPTreeNode* bptree_create_node(bool is_leaf);
void bptree_insert(BPTree* tree, int key, struct Record* record);
struct Record* bptree_search(BPTree* tree, int key);
void bptree_delete(BPTree* tree, int key);
void bptree_free(BPTree* tree);
void bptree_free_node(BPTreeNode* node);

// Helper functions
BPTreeNode* bptree_split_child(BPTreeNode* parent, int index, BPTreeNode* child);
void bptree_insert_non_full(BPTreeNode* node, int key, struct Record* record);
BPTreeNode* bptree_search_node(BPTreeNode* node, int key);
int bptree_find_key_index(BPTreeNode* node, int key);

// Range query support (useful for disk I/O optimization)
struct Record** bptree_range_search(BPTree* tree, int start_key, int end_key, int* count);

#endif
