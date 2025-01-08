#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include "constants.h"

typedef struct Record {
    void* data[MAX_COLUMNS];
    struct Record* next;
    struct Record* prev;
} Record;

typedef struct {
    char name[MAX_COLUMN_NAME_LENGTH];
    char type[10]; // "INTEGER" or "STRING"
} ColumnDef;

typedef struct RBTreeNode {
    int student_number;
    Record* record;
    struct RBTreeNode* parent;
    struct RBTreeNode* left;
    struct RBTreeNode* right;
    char color; // 'R' for red, 'B' for black
} RBTreeNode;

typedef struct {
    char name[MAX_TABLE_NAME_LENGTH];
    ColumnDef columns[MAX_COLUMNS];
    int num_columns;
    Record* head;
    Record* tail;
    RBTreeNode* index_root;
} Table;

#endif // DATA_STRUCTURES_H