#include "../include/command.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>              
#include "../include/table.h"
#include "../include/record.h"  
#include "../include/rbtree.h"
#include "../include/utils.h"
#include "../include/hashmap.h"
#include"../include/file_io.h"

// CREATE TABLE <table_name>
void create_table_cmd(const char* command) {
    char table_name[MAX_TABLE_NAME_LENGTH];
    int num_columns;

    // Parse the command to get table name and number of columns
    if (sscanf(command, "CREATE TABLE %s %d", table_name, &num_columns) != 2) {
        printf("Invalid CREATE TABLE command. Usage: CREATE TABLE <table_name> <num_columns>\n");
        return;
    }

    if (find_table(table_name) != NULL) {
        printf("Error: Table '%s' already exists.\n", table_name);
        return;
    }

    // Allocate memory for the table
    Table* new_table = (Table*)malloc(sizeof(Table));
    if (!new_table) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    strcpy(new_table->name, table_name);
    new_table->num_columns = num_columns;

    // Allocate memory for the columns
    new_table->columns = (ColumnDef*)malloc(num_columns * sizeof(ColumnDef));
    if (!new_table->columns) {
        perror("Memory allocation failed");
        free(new_table);
        exit(EXIT_FAILURE);
    }

    // Get column names and types from the user
    for (int i = 0; i < num_columns; i++) {
        char column_name[MAX_COLUMN_NAME_LENGTH];
        char column_type[10];

        printf("Enter column %d name: ", i + 1);
        scanf("%s", column_name);
        printf("Enter column %d type (INTEGER or STRING): ", i + 1);
        scanf("%s", column_type);

        if (strcmp(column_type, "INTEGER") != 0 && strcmp(column_type, "STRING") != 0) {
            printf("Error: Invalid column type. Use 'INTEGER' or 'STRING'.\n");
            free(new_table->columns);
            free(new_table);
            return;
        }

        strcpy(new_table->columns[i].name, column_name);
        strcpy(new_table->columns[i].type, column_type);
    }

    new_table->head = NULL;
    new_table->tail = NULL;
    new_table->index_root = NULL;

    // Insert the new table into the hashmap
    extern HashMap hashmap;
    hashmap_insert(&hashmap, table_name, new_table);
    printf("Table '%s' created successfully with %d columns.\n", table_name, num_columns);
}
// DELETE TABLE <table_name>
void delete_table_cmd(const char* command) {
    char table_name[MAX_TABLE_NAME_LENGTH];
    if (sscanf(command, "DELETE TABLE %s", table_name) == 1) {
        Table* table = find_table(table_name);
        if (table == NULL) {
            printf("Error: Table '%s' not found.\n", table_name);
            return;
        }

        // Free all records in the table
        Record* current = table->head;
        while (current != NULL) {
            Record* next = current->next;
            free_record_data(table, current);
            current = next;
        }

        // Free the Red-Black Tree index
        rbt_free_tree(table->index_root);

        // Free the columns array
        free(table->columns);

        // Remove the table from the hashmap
        extern HashMap hashmap;
        hashmap_remove(&hashmap, table_name);

        // Free the table structure
        free(table);
        printf("Table '%s' deleted successfully.\n", table_name);
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
        table->index_root = index_root;
        printf("Index created on table '%s' (student-number).\n", table_name);
    } else {
        printf("Invalid CREATE INDEX command.\n");
    }
}

int parse_command(const char* command, char** tokens, int max_tokens) {
    int token_index = 0;
    const char* ptr = command;
    while (token_index < max_tokens && *ptr != '\0') {
        // Skip any leading spaces
        while (*ptr == ' ' && *ptr != '\0') ptr++;
        if (*ptr == '"') {
            // Start of quoted string
            ptr++;
            const char* token_start = ptr;
            while (*ptr != '"' && *ptr != '\0') ptr++;
            if (*ptr == '"') {
                // Allocate memory for the token
                tokens[token_index] = malloc(ptr - token_start + 1);
                if (tokens[token_index] == NULL) {
                    // Handle memory allocation error
                    return -1;
                }
                strncpy((char*)tokens[token_index], (char*)token_start, ptr - token_start);
                tokens[token_index][ptr - token_start] = '\0';
                token_index++;
                ptr++;
            } else {
                // Unterminated quote
                return -1;
            }
        } else if (*ptr != '\0') {
            // Start of unquoted token
            const char* token_start = ptr;
            while (*ptr != ' ' && *ptr != '\0') ptr++;
            int token_len = ptr - token_start;
            tokens[token_index] = malloc(token_len + 1);
            if (tokens[token_index] == NULL) {
                // Handle memory allocation error
                return -1;
            }
            strncpy((char*)tokens[token_index], (char*)token_start, token_len);
            tokens[token_index][token_len] = '\0';
            token_index++;
        }
    }
    return token_index;
}

// ADD <table_name> <column_name_1> <value_1> ... <column_name_n> <value_n>
void add_record_cmd(const char* command) {
    char table_name[MAX_TABLE_NAME_LENGTH];
    char* tokens[50]; // Array to hold tokens (column names and values)
    int num_tokens = 0;

    // Parse the command into tokens
    char* rest = (char*)command;
    char* token;

    // Skip the "ADD" command
    token = strtok_s(rest, " ", &rest);
    if (token == NULL || strcmp(token, "ADD") != 0) {
        printf("Invalid ADD command.\n");
        return;
    }

    // Extract the table name
    token = strtok_s(rest, " ", &rest);
    if (token == NULL) {
        printf("Invalid ADD command. Missing table name.\n");
        return;
    }
    strcpy(table_name, token);

    // Parse the rest of the command into tokens
    while ((token = strtok_s(rest, " ", &rest)) != NULL && num_tokens < 50) {
        tokens[num_tokens++] = token;
    }

    // Validate the number of tokens
    Table* table = find_table(table_name);
    if (table == NULL) {
        printf("Error: Table '%s' not found.\n", table_name);
        return;
    }

    if (num_tokens != table->num_columns * 2) {
        printf("Error: Incorrect number of column-value pairs. Expected %d pairs.\n", table->num_columns);
        return;
    }

    // Dynamically allocate memory for ordered_values
    char** ordered_values = (char**)malloc(table->num_columns * sizeof(char*));
    if (!ordered_values) {
        perror("Memory allocation failed");
        return;
    }

    // Extract column-value pairs
    for (int i = 0; i < table->num_columns; i++) {
        ordered_values[i] = tokens[i * 2 + 1]; // Values are at odd indices
    }

    // Create a new record
    Record* new_record = create_record(table, ordered_values);
    if (new_record == NULL) {
        printf("Error: Failed to create record.\n");
        free(ordered_values); // Free allocated memory
        return;
    }

    // Add the record to the linked list
    if (table->head == NULL) {
        table->head = new_record;
        table->tail = new_record;
    } else {
        new_record->next = table->head;
        table->head->prev = new_record;
        table->head = new_record;
    }

    // Update the Red-Black Tree index (if applicable)
    if (table->index_root != NULL) {
        int student_number = atoi(ordered_values[0]); // Assuming first column is student-number
        table->index_root = rbt_insert(table->index_root, student_number, new_record);
    }

    printf("Record added to table '%s'.\n", table_name);

    // Free the dynamically allocated memory
    free(ordered_values);
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

        // Validate new value for general-course-score and core-course-score
        if (strcmp(column_name, "general-course-score") == 0 || strcmp(column_name, "core-course-score") == 0) {
            int new_score = atoi(new_value);
            if (!is_valid_score(new_score)) {
                printf("Warning: The entered value for %s (%d) is not in the right range (0 to 20).\n", column_name, new_score);
                printf("Use 'HELP' for more information.\n");
                return;
            }
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


// SELECT <table_name> <column_name> <value> [SORTED]
void select_records_cmd(const char* command) {
    char table_name[MAX_TABLE_NAME_LENGTH];
    char column_name[MAX_COLUMN_NAME_LENGTH];
    char value[MAX_STRING_LENGTH];
    char sorted[10] = "";
    char condition[10] = ""; // New: To handle conditions like AND, OR, NOT
    char second_column_name[MAX_COLUMN_NAME_LENGTH];
    char second_value[MAX_STRING_LENGTH];

    // Parse the command
    if (sscanf(command, "SELECT %s %s %s %s %s %s %s", table_name, column_name, value, condition, second_column_name, second_value, sorted) >= 3) {
        Table* table = find_table(table_name);
        if (table == NULL) {
            printf("Error: Table '%s' not found.\n", table_name);
            return;
        }

        int col_index = find_column_index(table, column_name);
        int second_col_index = -1;
        if (strlen(condition) > 0) {
            second_col_index = find_column_index(table, second_column_name);
            if (second_col_index == -1) {
                printf("Error: Column '%s' not found in table '%s'.\n", second_column_name, table_name);
                return;
            }
        }

        Record* matching_records_head = NULL;
        Record* matching_records_tail = NULL;
        Record* current = table->head;

        // Step 1: Find all matching records
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

            // Handle AND, OR, NOT conditions
            if (strlen(condition) > 0) {
                int second_match = 0;
                if (strcmp(table->columns[second_col_index].type, "INTEGER") == 0) {
                    if (atoi(second_value) == *(int*)current->data[second_col_index]) {
                        second_match = 1;
                    }
                } else if (strcmp(table->columns[second_col_index].type, "STRING") == 0) {
                    if (strcmp(second_value, (char*)current->data[second_col_index]) == 0) {
                        second_match = 1;
                    }
                }

                if (strcmp(condition, "AND") == 0) {
                    match = match && second_match;
                } else if (strcmp(condition, "OR") == 0) {
                    match = match || second_match;
                } else if (strcmp(condition, "NOT") == 0) {
                    match = match && !second_match;
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

        // Step 2: Sort the records by student-number if SORTED is specified
        if (strcmp(sorted, "SORTED") == 0) {
            matching_records_head = merge_sort(matching_records_head, 0, table);
        }

        // Step 3: Print the records
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

        // Step 4: Free the temporary linked list of matching records
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

void save_cmd(const char* command) {
    char table_name[MAX_TABLE_NAME_LENGTH];
    char filename[256];
    char format[10];
    if (sscanf(command, "SAVE %s %s %s", table_name, filename, format) == 3) {
        Table* table = find_table(table_name);
        if (!table) {
            printf("Error: Table '%s' not found.\n", table_name);
            return;
        }

        if (strcmp(format, "CSV") == 0) {
            save_to_csv(filename, table);
        } else if (strcmp(format, "JSON") == 0) {
            save_to_json(filename, table);
        } else if (strcmp(format, "EXCEL") == 0) {
            save_to_excel(filename, table);
        } else {
            printf("Error: Unsupported format. Use CSV, JSON, or EXCEL.\n");
        }
    } else {
        printf("Invalid SAVE command. Usage: SAVE <table_name> <filename> <format>\n");
    }
}

void load_cmd(const char* command) {
    char table_name[MAX_TABLE_NAME_LENGTH];
    char filename[256];
    char format[10];
    if (sscanf(command, "LOAD %s %s %s", table_name, filename, format) == 3) {
        Table* table = find_table(table_name);
        if (!table) {
            printf("Error: Table '%s' not found.\n", table_name);
            return;
        }

        if (strcmp(format, "CSV") == 0) {
            load_from_csv(filename, table);
        } else if (strcmp(format, "JSON") == 0) {
            load_from_json(filename, table);
        } else if (strcmp(format, "EXCEL") == 0) {
            load_from_excel(filename, table);
        } else {
            printf("Error: Unsupported format. Use CSV, JSON, or EXCEL.\n");
        }
    } else {
        printf("Invalid LOAD command. Usage: LOAD <table_name> <filename> <format>\n");
    }
}

void help() {
    printf("\nAvailable Commands:\n");
    printf("-------------------\n");

    // CREATE TABLE command
    printf("1. CREATE TABLE <table_name> <num_columns>\n");
    printf("   - Creates a new table with the specified name and number of columns.\n");
    printf("   - After entering this command, you will be prompted to define the column names, types, and constraints.\n");
    printf("   - Example: CREATE TABLE students 3\n\n");

    // DELETE TABLE command
    printf("2. DELETE TABLE <table_name>\n");
    printf("   - Deletes the specified table and all its records.\n");
    printf("   - Example: DELETE TABLE students\n\n");

    // ADD command
    printf("3. ADD <table_name> <column_name_1> <value_1> ... <column_name_n> <value_n>\n");
    printf("   - Adds a new record to the specified table.\n");
    printf("   - You must provide values for all columns in the table.\n");
    printf("   - Example: ADD students student-number 123 name \"John Doe\" score 85\n\n");

    // SELECT command
    printf("4. SELECT <table_name> <column_name> <value> [AND|OR|NOT <second_column_name> <second_value>] [SORTED]\n");
    printf("   - Selects records from the specified table where the column matches the value.\n");
    printf("   - Use AND, OR, or NOT to combine multiple conditions.\n");
    printf("   - Use the optional SORTED keyword to sort the results by student-number.\n");
    printf("   - Examples:\n");
    printf("     - SELECT students score 85\n");
    printf("     - SELECT students score 85 AND name \"John Doe\"\n");
    printf("     - SELECT students score 85 OR score 90 SORTED\n\n");

    // UPDATE command
    printf("5. UPDATE <table_name> <column_name> <old_value> <new_value>\n");
    printf("   - Updates records in the specified table where the column matches the old value.\n");
    printf("   - Example: UPDATE students score 85 90\n\n");

    // DELETE command
    printf("6. DELETE <table_name> <column_name> <value>\n");
    printf("   - Deletes records from the specified table where the column matches the value.\n");
    printf("   - Example: DELETE students student-number 123\n\n");

    // SAVE command
    printf("7. SAVE <table_name> <filename> <format>\n");
    printf("   - Saves the specified table to a file in the given format (CSV, JSON, or EXCEL).\n");
    printf("   - Example: SAVE students data.csv CSV\n\n");

    // LOAD command
    printf("8. LOAD <table_name> <filename> <format>\n");
    printf("   - Loads the specified table from a file in the given format (CSV, JSON, or EXCEL).\n");
    printf("   - Example: LOAD students data.csv CSV\n\n");

    // HELP command
    printf("9. HELP\n");
    printf("   - Displays this help message.\n\n");

    // EXIT command
    printf("10. EXIT\n");
    printf("   - Exits the program.\n\n");
}