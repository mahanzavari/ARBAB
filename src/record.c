#include "record.h"
#include "table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Record* create_record(struct Table* table, char** values) {
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
        } else if (strcmp(table->columns[i].type, "FLOAT") == 0) {
            new_record->data[i] = malloc(sizeof(float));
            *(float*)new_record->data[i] = atof(values[i]);
        } else if (strcmp(table->columns[i].type, "BOOLEAN") == 0) {
            new_record->data[i] = malloc(sizeof(int));
            // Accept "true", "false", "1", "0"
            if (strcmp(values[i], "true") == 0 || strcmp(values[i], "TRUE") == 0 || strcmp(values[i], "1") == 0) {
                *(int*)new_record->data[i] = 1;
            } else {
                *(int*)new_record->data[i] = 0;
            }
        } else if (strcmp(table->columns[i].type, "DATE") == 0) {
            // Store DATE as string in format YYYY-MM-DD
            new_record->data[i] = malloc(11); // "YYYY-MM-DD\0"
            strncpy((char*)new_record->data[i], values[i], 10);
            ((char*)new_record->data[i])[10] = '\0';
        } else if (strcmp(table->columns[i].type, "STRING") == 0) {
            new_record->data[i] = malloc(MAX_STRING_LENGTH);
            strcpy((char*)new_record->data[i], values[i]);
        }
    }
    return new_record;
}
void free_record_data(struct Table* table, Record* record) {
    for (int i = 0; i < table->num_columns; i++) {
        free(record->data[i]);
    }
    free(record);
}