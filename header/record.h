#ifndef RECORD_H
#define RECORD_H

#define MAX_COLUMNS 8


// #include "table.h"
#include <stdlib.h>

// Forward declaration of Table
struct Table;


typedef struct Record {
    void* data[MAX_COLUMNS];
    struct Record* next;
    struct Record* prev;
} Record;


Record* create_record(Table* table, char** values);
void free_record_data(Table* table, Record* record);

void add_record_cmd(const char* command);
void delete_record_cmd(const char* command);
void update_record_cmd(const char* command);
#endif 
