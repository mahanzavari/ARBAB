#ifndef RECORD_H
#define RECORD_H

#include <stdlib.h>

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
