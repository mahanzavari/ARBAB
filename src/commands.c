#include "../header/commands.h"
#include "../header/table.h"
#include "../header/record.h"
#include "../header/rbtree.h"
#include "../header/utils/merge_sort.h" // For Merge Sort
#include <stdio.h>
#include <string.h>




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

// CREATE INDEX <table_name>
void create_index_cmd(const char* command) {
    char table_name[MAX_TABLE_NAME_LENGTH];
    if (sscanf(command, "CREATE INDEX %s", table_name) == 1) {
        Table* table = find_table(table_name);
        if (table == NULL) {
            printf("Error: Table '%s' not found.\n", table_name);
            return;
        }

        // Assuming index is always on student-number (primary key)
        RBTreeNode* index_root = NULL;
        Record* current = table->head;
        while (current != NULL) {
            index_root = rbt_insert(index_root, *(int*)current->data[0], current);
            current = current->next;
        }
        printf("Index created on table '%s' (student-number).\n", table_name);

        // Store the index root in the table structure (not implemented here for simplicity)
        // In a real implementation, you would add a field to the Table struct for the index root.
    } else {
        printf("Invalid CREATE INDEX command.\n");
    }
}

// ADD <table_name> <column_name_1> <value_1> ... <column_name_n> <value_n>
void add_record_cmd(const char* command) {
    char table_name[MAX_TABLE_NAME_LENGTH];
    char* token;
    char* rest = (char*)command;

    token = strtok_r(rest, " ", &rest); // ADD
    token = strtok_r(rest, " ", &rest); // table_name
    if (token == NULL) {
        printf("Invalid ADD command.\n");
        return;
    }
    strcpy(table_name, token);

    Table* table = find_table(table_name);
    if (table == NULL) {
        printf("Error: Table '%s' not found.\n", table_name);
        return;
    }

    // Extract column-value pairs
    char* values[MAX_COLUMNS];
    int value_index = 0;
    char* col_names[MAX_COLUMNS];
    int col_index = 0;

    while ((token = strtok_r(rest, " ", &rest)) != NULL && value_index < table->num_columns) {
        if (value_index % 2 == 0) {
            col_names[col_index++] = token;
        } else {
            values[value_index / 2] = token;
        }
        value_index++;
    }

    if (value_index != table->num_columns * 2) {
        printf("Error: Incorrect number of column-value pairs.\n");
        return;
    }

    // Create a temporary array to hold values in the correct column order
    char* ordered_values[MAX_COLUMNS];
    for (int i = 0; i < table->num_columns; i++) {
        ordered_values[i] = NULL;
    }

    // Map the input values to the correct column order
    for (int i = 0; i < col_index; i++) {
        int col_idx = find_column_index(table, col_names[i]);
        if (col_idx != -1) {
            ordered_values[col_idx] = values[i];
        } else {
            printf("Error: Column '%s' not found in table '%s'.\n", col_names[i], table_name);
            return;
        }
    }

    // Check if all values are provided
    for (int i = 0; i < table->num_columns; i++) {
        if (ordered_values[i] == NULL) {
            printf("Error: Missing value for column '%s'.\n", table->columns[i].name);
            return;
        }
    }

    Record* new_record = create_record(table, ordered_values);

    // Check for duplicate primary key
    int student_number = atoi(ordered_values[0]);
    Record* current = table->head;
    while (current != NULL) {
        if (*(int*)current->data[0] == student_number) {
            printf("Error: Duplicate student number '%d'.\n", student_number);
            free_record_data(table, new_record);
            return;
        }
        current = current->next;
    }

    // Add to linked list
    if (table->head == NULL) {
        table->head = new_record;
        table->tail = new_record;
    } else {
        new_record->next = table->head;
        table->head->prev = new_record;
        table->head = new_record;
    }

    printf("Record added to table '%s'.\n", table_name);
}

// DELETE <table_name> <column_name> <value>
void delete_record_cmd(const char* command) {
    char table_name[MAX_TABLE_NAME_LENGTH];
    char column_name[MAX_COLUMN_NAME_LENGTH];
    char value[MAX_STRING_LENGTH];
    if (sscanf(command, "DELETE %s %s %s", table_name, column_name, value) == 3) {
        Table* table = find_table(table_name);
        if (table == NULL) {
            printf("Error: Table '%s' not found.\n", table_name);
            return;
        }
        int col_index = find_column_index(table, column_name);
        if (col_index == -1) {
            printf("Error: Column '%s' not found in table '%s'.\n", column_name, table_name);
            return;
        }

        Record* current = table->head;
        while (current != NULL) {
            int match = 0;
            if (strcmp(table->columns[col_index].type, "INTEGER") == 0) {
                if (atoi(value) == *(int*)current->data[col_index]) {
                    match = 1;
                }
            } else if (strcmp(table->columns[col_index].type, "STRING") == 0) {
                if (strcmp(value, (char*)current->data[col_index]) == 0) {
                    match = 1;
                }
            }

            if (match) {
                if (current->prev != NULL) {
                    current->prev->next = current->next;
                } else {
                    table->head = current->next;
                }

                if (current->next != NULL) {
                    current->next->prev = current->prev;
                } else {
                    table->tail = current->prev;
                }
                Record* temp = current->next;
                free_record_data(table, current);
                current = temp;
            } else {
                current = current->next;
            }
        }
        printf("Records deleted from table '%s'.\n", table_name);
    } else {
        printf("Invalid DELETE command.\n");
    }
}

// UPDATE <table_name> <column_name> <old_value> <new_value>
void update_record_cmd(const char* command) {
    char table_name[MAX_TABLE_NAME_LENGTH];
    char column_name[MAX_COLUMN_NAME_LENGTH];
    char old_value[MAX_STRING_LENGTH];
    char new_value[MAX_STRING_LENGTH];
    if (sscanf(command, "UPDATE %s %s %s %s", table_name, column_name, old_value, new_value) == 4) {
        Table* table = find_table(table_name);
        if (table == NULL) {
            printf("Error: Table '%s' not found.\n", table_name);
            return;
        }
        int col_index = find_column_index(table, column_name);
        if (col_index == -1) {
            printf("Error: Column '%s' not found in table '%s'.\n", column_name, table_name);
            return;
        }

        Record* current = table->head;
        while (current != NULL) {
            int match = 0;
            if (strcmp(table->columns[col_index].type, "INTEGER") == 0) {
                if (atoi(old_value) == *(int*)current->data[col_index]) {
                    match = 1;
                    *(int*)current->data[col_index] = atoi(new_value);
                }
            } else if (strcmp(table->columns[col_index].type, "STRING") == 0) {
                if (strcmp(old_value, (char*)current->data[col_index]) == 0) {
                    match = 1;
                    strcpy((char*)current->data[col_index], new_value);
                }
            }
            current = current->next;
        }
        printf("Records updated in table '%s'.\n", table_name);
    } else {
        printf("Invalid UPDATE command.\n");
    }
}

void create_table_cmd(const char* command) {
    char table_name[MAX_TABLE_NAME_LENGTH];
    if (sscanf(command, "CREATE TABLE %s", table_name) == 1) {
        if (find_table(table_name)) {
            printf("Table '%s' already exists.\n", table_name);
            return;
        }
        Table* new_table = (Table*)malloc(sizeof(Table));
        // Initialize the table
        hashmap_insert(table_map, table_name, new_table);
        printf("Table '%s' created successfully.\n", table_name);
    } else {
        printf("Invalid CREATE TABLE command.\n");
    }
}

// SELECT <table_name> <column_name> <value> [SORTED]
void select_records_cmd(const char* command) {
    char table_name[MAX_TABLE_NAME_LENGTH];
    char column_name[MAX_COLUMN_NAME_LENGTH];
    char value[MAX_STRING_LENGTH];
    char sorted[10] = "";
    if (sscanf(command, "SELECT %s %s %s %s", table_name, column_name, value, sorted) >= 3) {
        Table* table = find_table(table_name);
        if (table == NULL) {
            printf("Error: Table '%s' not found.\n", table_name);
            return;
        }
        int col_index = find_column_index(table, column_name);
        if (col_index == -1) {
            printf("Error: Column '%s' not found in table '%s'.\n", column_name, table_name);
            return;
        }

        Record* matching_records_head = NULL;
        Record* matching_records_tail = NULL;
        Record* current = table->head;

        while (current != NULL) {
            int match = 0;
            if (strcmp(table->columns[col_index].type, "INTEGER") == 0) {
                if (atoi(value) == *(int*)current->data[col_index]) {
                    match = 1;
                }
            } else if (strcmp(table->columns[col_index].type, "STRING") == 0) {
                if (strcmp(value, (char*)current->data[col_index]) == 0) {
                    match = 1;
                }
            }

            if (match) {
                Record* new_match = (Record*)malloc(sizeof(Record));
                if (!new_match) {
                    perror("Memory allocation failed");
                    exit(EXIT_FAILURE);
                }
                memcpy(new_match, current, sizeof(Record));
                new_match->next = NULL;
                new_match->prev = NULL;

                if (matching_records_head == NULL) {
                    matching_records_head = new_match;
                    matching_records_tail = new_match;
                } else {
                    matching_records_tail->next = new_match;
                    new_match->prev = matching_records_tail;
                    matching_records_tail = new_match;
                }
            }
            current = current->next;
        }

        printf("Selected records from table '%s':\n", table_name);

        if (strcmp(sorted, "SORTED") == 0 && strcmp(table->columns[col_index].type, "INTEGER") == 0) {
            matching_records_head = merge_sort(matching_records_head, col_index);
            Record* sorted_current = matching_records_head;
            while (sorted_current != NULL) {
                for (int i = 0; i < table->num_columns; i++) {
                    printf("%s: ", table->columns[i].name);
                    if (strcmp(table->columns[i].type, "INTEGER") == 0) {
                        printf("%d\t", *(int*)sorted_current->data[i]);
                    } else {
                        printf("%s\t", (char*)sorted_current->data[i]);
                    }
                }
                printf("\n");
                sorted_current = sorted_current->next;
            }
        } else {
            Record* print_current = matching_records_head;
            while (print_current != NULL) {
                for (int i = 0; i < table->num_columns; i++) {
                    printf("%s: ", table->columns[i].name);
                    if (strcmp(table->columns[i].type, "INTEGER") == 0) {
                        printf("%d\t", *(int*)print_current->data[i]);
                    } else {
                        printf("%s\t", (char*)print_current->data[i]);
                    }
                }
                printf("\n");
                print_current = print_current->next;
            }
        }

        // Free the temporary linked list of matching records
        Record* temp = matching_records_head;
        while (temp != NULL) {
            Record* next = temp->next;
            free(temp);
            temp = next;
        }

    } else {
        printf("Invalid SELECT command.\n");
    }
}
