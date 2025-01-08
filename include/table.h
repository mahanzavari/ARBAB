#ifndef TABLE_H
#define TABLE_H

#include "data_structures.h"

Record* create_record(Table* table, char** values);
Table* find_table(const char* table_name);
int find_column_index(Table* table, const char* column_name);
void free_record_data(Table* table, Record* record);

#endif // TABLE_H