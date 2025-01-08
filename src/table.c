#include "../include/table.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Record* create_record(Table* table, char** values) {
    Record* new_record = (Record*)malloc(sizeof(Record));
    if (!new_record) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    new_record->next = NULL;
    new_record->prev = NULL;
    for (int i = 0; i < table->num_columns; i++) {
        if (strcmp(table->columns[i].type, "INTEGER") == 0) {
            new_record->data[i] = malloc(sizeof(int));
            *(int*)new_record->data[i] = atoi(values[i]);
        } else if (strcmp(table->columns[i].type, "STRING") == 0) {
            new_record->data[i] = malloc(MAX_STRING_LENGTH);
            strcpy((char*)new_record->data[i], values[i]);
        }
    }
    return new_record;
}

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

void free_record_data(Table* table, Record* record) {
    for (int i = 0; i < table->num_columns; i++) {
        free(record->data[i]);
    }
    free(record);
}