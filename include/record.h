#ifndef RECORD_H
#define RECORD_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STRING_LENGTH 32
#define MAX_COLUMNS 8

// Forward declaration of Table
struct Table;

typedef struct Record {
    void* data[MAX_COLUMNS]; // Array of pointers to column values
    struct Record* next;
    struct Record* prev;
} Record;

// Function prototypes
Record* create_record(struct Table* table, char** values);
void free_record_data(struct Table* table, Record* record);

#endif