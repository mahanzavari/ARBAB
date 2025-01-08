#include "commands.h"
#include "table.h"
#include "hashmap.h"
#include "rbtree.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void create_table_cmd(const char* command) {
    char table_name[MAX_TABLE_NAME_LENGTH];
    if (sscanf(command, "CREATE TABLE %s", table_name) == 1) {
        if (hashmap_lookup(&hashmap, table_name) != NULL) {
            printf("Error: Table '%s' already exists.\n", table_name);
            return;
        }

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
        new_table->index_root = NULL;

        hashmap_insert(&hashmap, table_name, new_table);
        printf("Table '%s' created successfully.\n", table_name);
    } else {
        printf("Invalid CREATE TABLE command.\n");
    }
}

void delete_table_cmd(const char* command) {
    char table_name[MAX_TABLE_NAME_LENGTH];
    if (sscanf(command, "DELETE TABLE %s", table_name) == 1) {
        Table* table = hashmap_lookup(&hashmap, table_name);
        if (table == NULL) {
            printf("Error: Table '%s' not found.\n", table_name);
            return;
        }

        Record* current = table->head;
        while (current != NULL) {
            Record* next = current->next;
            free_record_data(table, current);
            current = next;
        }

        rbt_free_tree(table->index_root);

        hashmap_remove(&hashmap, table_name);

        free(table);
        printf("Table '%s' deleted successfully.\n", table_name);
    } else {
        printf("Invalid DELETE TABLE command.\n");
    }
}

void create_index_cmd(const char* command) {
    char table_name[MAX_TABLE_NAME_LENGTH];
    if (sscanf(command, "CREATE INDEX %s", table_name) == 1) {
        Table* table = find_table(table_name);
        if (table == NULL) {
            printf("Error: Table '%s' not found.\n", table_name);
            return;
        }

        RBTreeNode* index_root = NULL;
        Record* current = table->head;
        while (current != NULL) {
            index_root = rbt_insert(index_root, *(int*)current->data[0], current);
            current = current->next;
        }
        printf("Index created on table '%s' (student-number).\n", table_name);

        table->index_root = index_root;
    } else {
        printf("Invalid CREATE INDEX command.\n");
    }
}

void add_record_cmd(const char* command) {
    char table_name[MAX_TABLE_NAME_LENGTH];
    char* tokens[50];
    int num_tokens = 0;

    char* rest = (char*)command;
    char* token;

    token = strtok_r(rest, " ", &rest);
    if (token == NULL || strcmp(token, "ADD") != 0) {
        printf("Invalid ADD command.\n");
        return;
    }

    token = strtok_r(rest, " ", &rest);
    if (token == NULL) {
        printf("Invalid ADD command. Missing table name.\n");
        return;
    }
    strcpy(table_name, token);

    while ((token = strtok_r(rest, " ", &rest)) != NULL && num_tokens < 50) {
        tokens[num_tokens++] = token;
    }

    if (num_tokens % 2 != 0) {
        printf("Error: Incorrect number of column-value pairs.\n");
        return;
    }

    Table* table = find_table(table_name);
    if (table == NULL) {
        printf("Error: Table '%s' not found.\n", table_name);
        return;
    }

    char* col_names[MAX_COLUMNS];
    char* values[MAX_COLUMNS];
    int col_index = 0;
    int value_index = 0;

    for (int i = 0; i < num_tokens; i += 2) {
        col_names[col_index++] = tokens[i];
        values[value_index++] = tokens[i + 1];
    }

    char* ordered_values[MAX_COLUMNS];
    for (int i = 0; i < table->num_columns; i++) {
        ordered_values[i] = NULL;
    }

    for (int i = 0; i < col_index; i++) {
        int col_idx = find_column_index(table, col_names[i]);
        if (col_idx != -1) {
            ordered_values[col_idx] = values[i];
        } else {
            printf("Error: Column '%s' not found in table '%s'.\n", col_names[i], table_name);
            return;
        }
    }

    for (int i = 0; i < table->num_columns; i++) {
        if (ordered_values[i] == NULL) {
            printf("Error: Missing value for column '%s'.\n", table->columns[i].name);
            return;
        }
    }

    Record* new_record = create_record(table, ordered_values);
    if (new_record == NULL) {
        printf("Error: Failed to create record.\n");
        return;
    }

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

    if (table->head == NULL) {
        table->head = new_record;
        table->tail = new_record;
    } else {
        new_record->next = table->head;
        table->head->prev = new_record;
        table->head = new_record;
    }

    if (table->index_root != NULL) {
        table->index_root = rbt_insert(table->index_root, student_number, new_record);
    }

    printf("Record added to table '%s'.\n", table_name);
}

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

        if (strcmp(sorted, "SORTED") == 0) {
            matching_records_head = merge_sort(matching_records_head, 0);
        }

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