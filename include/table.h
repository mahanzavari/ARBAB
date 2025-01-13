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
} ColumnDef;

typedef struct Table {
    char name[MAX_TABLE_NAME_LENGTH];
    ColumnDef columns[MAX_COLUMNS];
    int num_columns;
    Record* head;
    Record* tail;
    struct RBTreeNode* index_root; // Root of the Red-Black Tree index
} Table;

// Function prototypes
Table* find_table(const char* table_name);
int find_column_index(Table* table, const char* column_name);

#endif