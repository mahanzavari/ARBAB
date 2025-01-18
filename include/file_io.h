#ifndef FILE_IO_H
#define FILE_IO_H

#include "record.h"
#include "table.h"

// CSV functions
void save_to_csv(const char* filename, Table* table);
void load_from_csv(const char* filename, Table* table);
void save_to_binary(const char* filename, Table* table);
void load_from_binary(const char* filename, Table* table);
#endif