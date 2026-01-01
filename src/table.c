#include "table.h"
#include "hashmap.h"
#include "bplustree.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

Table* find_table(const char* table_name) {
    return hashmap_lookup(&hashmap, table_name);
}

int find_column_index(Table* table, const char* column_name) {
    for (int i = 0; i < table->num_columns; i++) {
        if (strcmp(table->columns[i].name, column_name) == 0) {
            return i;
        }
    }
    return -1;
}