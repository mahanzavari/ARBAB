#include "../header/table.h"
#include "../header/record.h"
#include "../header/rbtree.h"
#include "../header/utils/hashmap.h" // For HashMap integration
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


int find_column_index(Table* table, const char* column_name) {
    for (int i = 0; i < table->num_columns; i++) {
        if (strcmp(table->columns[i].name, column_name) == 0) {
            return i;
        }
    }
    return -1;
}

// Function to find a table by name
// Table* find_table(const char* table_name) {
//     for (int i = 0; i < num_tables; i++) {
//         if (strcmp(tables[i]->name, table_name) == 0) {
//             return tables[i];
//         }
//     }
//     return NULL;
// }

Table* find_table(const char* table_name) {
    return (Table*)hashmap_get(table_map, table_name);
}


// CREATE TABLE <table_name>
void create_table_cmd(const char* command) {
    char table_name[MAX_TABLE_NAME_LENGTH];
    if (sscanf(command, "CREATE TABLE %s", table_name) == 1) {
        if (find_table(table_name) != NULL) {
            printf("Error: Table '%s' already exists.\n", table_name);
            return;
        }
        if (num_tables < 10) {
            Table* new_table = (Table*)malloc(sizeof(Table));
            if (!new_table) {
                perror("Memory allocation failed");
                exit(EXIT_FAILURE);
            }
            strcpy(new_table->name, table_name);
            new_table->num_columns = 7; // Fixed columns
            strcpy(new_table->columns[0].name, "student-number");
            strcpy(new_table->columns[0].type, "INTEGER");
            strcpy(new_table->columns[1].name, "general-course-name");
            strcpy(new_table->columns[1].type, "STRING");
            strcpy(new_table->columns[2].name, "general-course-instructor");
            strcpy(new_table->columns[2].type, "STRING");
            strcpy(new_table->columns[3].name, "general-course-score");
            strcpy(new_table->columns[3].type, "INTEGER");
            strcpy(new_table->columns[4].name, "core-course-name");
            strcpy(new_table->columns[4].type, "STRING");
            strcpy(new_table->columns[5].name, "core-course-instructor");
            strcpy(new_table->columns[5].type, "STRING");
            strcpy(new_table->columns[6].name, "core-course-score");
            strcpy(new_table->columns[6].type, "INTEGER");
            new_table->head = NULL;
            new_table->tail = NULL;
            tables[num_tables++] = new_table;
            printf("Table '%s' created successfully.\n", table_name);
        } else {
            printf("Error: Maximum number of tables reached.\n");
        }
    } else {
        printf("Invalid CREATE TABLE command.\n");
    }
}


// DELETE TABLE <table_name>
void delete_table_cmd(const char* command) {
    char table_name[MAX_TABLE_NAME_LENGTH];
    if (sscanf(command, "DELETE TABLE %s", table_name) == 1) {
        int found = -1;
        for (int i = 0; i < num_tables; i++) {
            if (strcmp(tables[i]->name, table_name) == 0) {
                found = i;
                break;
            }
        }
        if (found != -1) {
            Table* table_to_delete = tables[found];
            Record* current = table_to_delete->head;
            while (current != NULL) {
                Record* next = current->next;
                free_record_data(table_to_delete, current);
                current = next;
            }
            free(table_to_delete);
            // Shift remaining tables
            for (int i = found; i < num_tables - 1; i++) {
                tables[i] = tables[i + 1];
            }
            num_tables--;
            printf("Table '%s' deleted successfully.\n", table_name);
        } else {
            printf("Error: Table '%s' not found.\n", table_name);
        }
    } else {
        printf("Invalid DELETE TABLE command.\n");
    }
}