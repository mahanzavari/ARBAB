#include "../include/file_io.h"
#include "../include/table.h"
#include "../include/record.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Save table to a CSV file
void save_to_csv(const char* filename, Table* table) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        perror("Failed to open file for saving");
        return;
    }

    // Write column headers
    for (int i = 0; i < table->num_columns; i++) {
        fprintf(file, "%s", table->columns[i].name);
        if (i < table->num_columns - 1) {
            fprintf(file, ",");
        }
    }
    fprintf(file, "\n");

    // Write records
    Record* current = table->head;
    while (current != NULL) {
        for (int i = 0; i < table->num_columns; i++) {
            if (strcmp(table->columns[i].type, "INTEGER") == 0) {
                fprintf(file, "%d", *(int*)current->data[i]);
            } else if (strcmp(table->columns[i].type, "STRING") == 0) {
                fprintf(file, "%s", (char*)current->data[i]);
            }
            if (i < table->num_columns - 1) {
                fprintf(file, ",");
            }
        }
        fprintf(file, "\n");
        current = current->next;
    }

    fclose(file);
    printf("Table '%s' saved to CSV file '%s'.\n", table->name, filename);
}

// Load table from a CSV file
void load_from_csv(const char* filename, Table* table) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file for loading");
        return;
    }

    char line[1024];
    fgets(line, sizeof(line), file); // Skip header line

    while (fgets(line, sizeof(line), file)) {
        char* token;
        char* saveptr;
        token = strtok_r(line, ",\n", &saveptr);

        // Dynamically allocate memory for values
        char** values = (char**)malloc(table->num_columns * sizeof(char*));
        if (!values) {
            perror("Memory allocation failed");
            fclose(file);
            return;
        }

        int i = 0;
        // Parse CSV line
        while (token != NULL) {
            values[i++] = token;
            token = strtok_r(NULL, ",\n", &saveptr);
        }

        // Create and add record
        Record* new_record = create_record(table, values);
        if (new_record == NULL) {
            printf("Error: Failed to create record.\n");
            free(values);  // Free allocated memory
            continue;
        }

        if (table->head == NULL) {
            table->head = new_record;
            table->tail = new_record;
        } else {
            new_record->next = table->head;
            table->head->prev = new_record;
            table->head = new_record;
        }

        free(values);  // Free the dynamically allocated array
    }

    fclose(file);
    printf("Table '%s' loaded from CSV file '%s'.\n", table->name, filename);
}

void save_to_binary(const char* filename, Table* table) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        perror("Failed to open file for saving");
        return;
    }

    // Write table metadata
    fwrite(table, sizeof(Table), 1, file);

    // Write records
    Record* current = table->head;
    while (current != NULL) {
        fwrite(current, sizeof(Record), 1, file);
        current = current->next;
    }

    fclose(file);
    printf("Table '%s' saved to binary file '%s'.\n", table->name, filename);
}

void load_from_binary(const char* filename, Table* table) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        perror("Failed to open file for loading");
        return;
    }

    // Read table metadata
    fread(table, sizeof(Table), 1, file);

    // Read records
    Record* prev = NULL;
    while (!feof(file)) {
        Record* new_record = (Record*)malloc(sizeof(Record));
        if (fread(new_record, sizeof(Record), 1, file) != 1) {
            free(new_record);
            break;
        }

        if (table->head == NULL) {
            table->head = new_record;
        } else {
            prev->next = new_record;
            new_record->prev = prev;
        }
        prev = new_record;
    }

    fclose(file);
    printf("Table '%s' loaded from binary file '%s'.\n", table->name, filename);
}