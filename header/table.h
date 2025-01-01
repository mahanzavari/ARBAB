#ifndef TABLE_H
#define TABLE_H

#include "record.h"
#include "rbtree.h"

#define MAX_TABLE_NAME_LENGTH 32
#define MAX_COLUMN_NAME_LENGTH 32
#define MAX_STRING_LENGTH 32
#define MAX_COLUMNS 8

typedef struct{
     char name[MAX_COLUMN_NAME_LENGTH];
     char type[10];// int or string 
}ColumnDef;

typedef struct {
     char name[MAX_TABLE_NAME_LENGTH];
     ColumnDef columns[MAX_COLUMNS];
     int num_columns;
     Record* head;
     Record* tail;
     RBTreeNode* index_root; // Root of the record
}Table;

int find_column_index(Table* table, const char* column_name);
void create_table_cmd(const char* command);
void delete_table_cmd(const char* command);
Table* find_table(const char* table_name);

#endif // TABLE_H
