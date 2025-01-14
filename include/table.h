#ifndef TABLE_H
#define TABLE_H

#include "record.h"
#include "rbtree.h"

#define MAX_TABLE_NAME_LENGTH 32
#define MAX_COLUMN_NAME_LENGTH 32
#define MAX_COLUMNS 8

// Forward declaration of RBTreeNode
struct RBTreeNode;

typedef struct ColumnDef {
    char name[MAX_COLUMN_NAME_LENGTH];
    char type[10]; // "INTEGER" or "STRING"
    int is_unique; // 1 if unique, 0 otherwise
    int is_primary_key; // 1 if primary key, 0 otherwise
    int is_not_null; // 1 if not null, 0 otherwise
} ColumnDef;

typedef struct Table {
    char name[MAX_TABLE_NAME_LENGTH];
    ColumnDef* columns; // Pointer to dynamically allocated array
    int num_columns;
    Record* head;
    Record* tail;
    struct RBTreeNode* index_root;
} Table;

// Function prototypes
Table* find_table(const char* table_name);
int find_column_index(Table* table, const char* column_name);

#endif