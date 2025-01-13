#ifndef UTILS_H
#define UTILS_H

#include "table.h"
#include "record.h"

// Function prototypes
int is_valid_score(int score);
Record* merge(Record* left_half, Record* right_half, int col_index, Table* table);
Record* split(Record* head);
Record* merge_sort(Record* head, int col_index, Table* table);

#endif