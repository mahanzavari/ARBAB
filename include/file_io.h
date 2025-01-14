#ifndef FILE_IO_H
#define FILE_IO_H

#include "table.h"

// CSV functions
void save_to_csv(const char* filename, Table* table);
void load_from_csv(const char* filename, Table* table);

// JSON functions
void save_to_json(const char* filename, Table* table);
void load_from_json(const char* filename, Table* table);

// Excel functions
void save_to_excel(const char* filename, Table* table);
void load_from_excel(const char* filename, Table* table);

#endif