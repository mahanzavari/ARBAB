// command.c
#include "../include/command.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../include/table.h"
#include "../include/record.h"
#include "../include/bplustree.h"
#include "../include/utils.h"
#include "../include/hashmap.h"
#include "../include/file_io.h"

typedef struct {
    Table* original_tables[HASHMAP_SIZE];
    int in_transaction;
} TransactionState;

TransactionState transaction_state = {0};

void begin_transaction() {
    if (transaction_state.in_transaction) {
        printf("Error: A transaction is already in progress.\n");
        return;
    }

    // Save the current state of all tables
    for (int i = 0; i < HASHMAP_SIZE; i++) {
        HashMapNode* current = hashmap.buckets[i];
        while (current != NULL) {
            Table* original_table = (Table*)malloc(sizeof(Table));
             if (!original_table) {
                perror("Memory allocation failed");
                exit(EXIT_FAILURE);
            }
            memcpy(original_table, current->table, sizeof(Table));
            transaction_state.original_tables[i] = original_table;
            current = current->next;
        }
    }

    transaction_state.in_transaction = 1;
    printf("Transaction started.\n");
}

void commit_transaction() {
    if (!transaction_state.in_transaction) {
        printf("Error: No transaction in progress.\n");
        return;
    }

    // Free the saved original tables
    for (int i = 0; i < HASHMAP_SIZE; i++) {
        if (transaction_state.original_tables[i] != NULL) {
            free(transaction_state.original_tables[i]);
            transaction_state.original_tables[i] = NULL;
        }
    }

    transaction_state.in_transaction = 0;
    printf("Transaction committed.\n");
}

void rollback_transaction() {
    if (!transaction_state.in_transaction) {
        printf("Error: No transaction in progress.\n");
        return;
    }

    // Restore the original state of all tables
    for (int i = 0; i < HASHMAP_SIZE; i++) {
        HashMapNode* current = hashmap.buckets[i];
        while (current != NULL) {
            if(transaction_state.original_tables[i]!=NULL)
            {
                memcpy(current->table, transaction_state.original_tables[i], sizeof(Table));
            }
            current = current->next;
        }
    }
     // Free the saved original tables
    for (int i = 0; i < HASHMAP_SIZE; i++) {
        if (transaction_state.original_tables[i] != NULL) {
            free(transaction_state.original_tables[i]);
            transaction_state.original_tables[i] = NULL;
        }
    }

    transaction_state.in_transaction = 0;
    printf("Transaction rolled back.\n");
}

// CREATE TABLE <table_name> <num_columns>
void create_table_cmd(const char* command) {
    char table_name[MAX_TABLE_NAME_LENGTH];
    int num_columns;

    // Parse the command to get table name and number of columns
    if (sscanf(command, "CREATE TABLE %s %d", table_name, &num_columns) != 2) {
        printf("Invalid CREATE TABLE command. Usage: CREATE TABLE <table_name> <num_columns>\n");
        return;
    }

     if (num_columns <= 0 || num_columns > MAX_COLUMNS) {
        printf("Error: Number of columns must be between 1 and %d.\n", MAX_COLUMNS);
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
    strncpy(new_table->name, table_name, MAX_TABLE_NAME_LENGTH -1);
     new_table->name[MAX_TABLE_NAME_LENGTH-1] = '\0';
    new_table->num_columns = num_columns;

    // Allocate memory for the columns
    new_table->columns = (ColumnDef*)malloc(num_columns * sizeof(ColumnDef));
    if (!new_table->columns) {
        perror("Memory allocation failed");
        free(new_table);
        exit(EXIT_FAILURE);
    }

    // Get column names, types, and constraints from the user
    for (int i = 0; i < num_columns; i++) {
        char column_name[MAX_COLUMN_NAME_LENGTH];
        char column_type[10];
        char constraints[50];

        printf("Enter column %d name: ", i + 1);
        if (scanf("%s", column_name) != 1) {
             printf("Error reading column name.\n");
            free(new_table->columns);
            free(new_table);
             while(getchar() != '\n');
            return;
        }

         printf("Enter column %d type (INTEGER, FLOAT, BOOLEAN, DATE, or STRING): ", i + 1);
        if (scanf("%s", column_type) != 1) {
              printf("Error reading column type.\n");
            free(new_table->columns);
            free(new_table);
             while(getchar() != '\n');
            return;
        }

        printf("Enter column %d constraints (UNIQUE, PRIMARY_KEY, NOT_NULL): ", i + 1);
         if (scanf("%s", constraints) != 1) {
              printf("Error reading column constraints.\n");
            free(new_table->columns);
            free(new_table);
             while(getchar() != '\n');
            return;
         }

        if (strcmp(column_type, "INTEGER") != 0 && strcmp(column_type, "STRING") != 0 &&
            strcmp(column_type, "FLOAT") != 0 && strcmp(column_type, "BOOLEAN") != 0 &&
            strcmp(column_type, "DATE") != 0) {
            printf("Error: Invalid column type. Use 'INTEGER', 'FLOAT', 'BOOLEAN', 'DATE', or 'STRING'.\n");
            free(new_table->columns);
            free(new_table);
            return;
        }

        strncpy(new_table->columns[i].name, column_name, MAX_COLUMN_NAME_LENGTH-1);
         new_table->columns[i].name[MAX_COLUMN_NAME_LENGTH-1] = '\0';
        strncpy(new_table->columns[i].type, column_type, 9);
        new_table->columns[i].type[9] = '\0';
        new_table->columns[i].is_unique = (strstr(constraints, "UNIQUE") != NULL);
        new_table->columns[i].is_primary_key = (strstr(constraints, "PRIMARY_KEY") != NULL);
        new_table->columns[i].is_not_null = (strstr(constraints, "NOT_NULL") != NULL);
    }

    new_table->head = NULL;
    new_table->tail = NULL;
    new_table->index_tree = NULL;

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

        // Free the B+ Tree index
        if(table->index_tree != NULL)
            bptree_free(table->index_tree);

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

        // Create B+ Tree index on the primary key (assuming first column)
        table->index_tree = bptree_create(BPTREE_ORDER);
        Record* current = table->head;
        while (current != NULL) {
            int key = *(int*)current->data[0];
            bptree_insert(table->index_tree, key, current);
            current = current->next;
        }
        printf("B+ Tree index created on table '%s' (primary key).\n", table_name);
    } else {
        printf("Invalid CREATE INDEX command.\n");
    }
}

int parse_command(const char* command, char** tokens, int max_tokens) {
    int token_index = 0;
    char* temp_command = strdup(command);
    if (temp_command == NULL) {
        return -1; // Memory allocation error
    }

    char* saveptr;
    char* token = strtok_r(temp_command, " ", &saveptr);
    while (token_index < max_tokens && token != NULL) {
        size_t token_len = strlen(token);
        if (token[0] == '"' && token[token_len - 1] == '"') {
            // Remove quotes for quoted strings
            tokens[token_index] = malloc(token_len - 1); //Allocate size for string without quotes
            if (tokens[token_index] == NULL) {
                free(temp_command);
                 for(int i = 0; i<token_index; i++)
                   free(tokens[i]);
                 return -1; // Handle memory allocation error
             }
             strncpy(tokens[token_index], token+1, token_len - 2); //Copy string without quotes
             tokens[token_index][token_len - 2] = '\0';
        } else {
          tokens[token_index] = malloc(token_len + 1);
            if (tokens[token_index] == NULL) {
               free(temp_command);
                for (int i = 0; i < token_index; i++)
                {
                 free(tokens[i]);
                }

                return -1; // Memory allocation error
            }
             strncpy(tokens[token_index], token, token_len);
              tokens[token_index][token_len] = '\0';
        }
         token_index++;
         token = strtok_r(NULL, " ", &saveptr);
    }
     free(temp_command);
     return token_index;
}

// ADD <table_name> <column_name_1> <value_1> ... <column_name_n> <value_n>
void add_record_cmd(const char* command) {
    char table_name[MAX_TABLE_NAME_LENGTH];
    char* tokens[100];
    int num_tokens = 0;
    
    //Parse the command into tokens
    num_tokens = parse_command(command, tokens, 100);
     if (num_tokens < 0)
     {
         printf("Error parsing the command.\n");
         return;
     }
    if (num_tokens < 3 || strcmp(tokens[0], "ADD") != 0) {
        printf("Invalid ADD command.\n");
         for(int i=0; i<num_tokens; i++)
             free(tokens[i]);
         return;
    }
    
    strncpy(table_name, tokens[1], MAX_TABLE_NAME_LENGTH-1);
      table_name[MAX_TABLE_NAME_LENGTH-1] = '\0';


    Table* table = find_table(table_name);
    if (table == NULL) {
        printf("Error: Table '%s' not found.\n", table_name);
        for(int i=0; i<num_tokens; i++)
             free(tokens[i]);
        return;
    }

      if ((num_tokens-2) != table->num_columns * 2) {
        printf("Error: Incorrect number of column-value pairs. Expected %d pairs.\n", table->num_columns);
        for(int i=0; i<num_tokens; i++)
             free(tokens[i]);
        return;
    }

    char** ordered_values = (char**)malloc(table->num_columns * sizeof(char*));
    if (!ordered_values) {
        perror("Memory allocation failed");
         for(int i=0; i<num_tokens; i++)
             free(tokens[i]);
        return;
    }
    for (int i = 0; i < table->num_columns; i++) {
         ordered_values[i] = tokens[i * 2 + 3];
    }

     // Validate constraints
    for (int i = 0; i < table->num_columns; i++) {
        if (table->columns[i].is_not_null && (ordered_values[i] == NULL || strlen(ordered_values[i]) == 0)) {
            printf("Error: Column '%s' cannot be NULL.\n", table->columns[i].name);
            free(ordered_values);
             for(int i=0; i<num_tokens; i++)
                free(tokens[i]);
            return;
        }

       if (table->columns[i].is_unique) {
            Record* current = table->head;
             while (current != NULL) {
                if (strcmp(table->columns[i].type, "INTEGER") == 0) {
                     if (atoi(ordered_values[i]) == *(int*)current->data[i]) {
                        printf("Error: Duplicate value for unique column '%s'.\n", table->columns[i].name);
                        free(ordered_values);
                        for(int i=0; i<num_tokens; i++)
                            free(tokens[i]);
                        return;
                    }
                } else if (strcmp(table->columns[i].type, "STRING") == 0) {
                    if (strcmp(ordered_values[i], (char*)current->data[i]) == 0) {
                        printf("Error: Duplicate value for unique column '%s'.\n", table->columns[i].name);
                        free(ordered_values);
                        for(int i=0; i<num_tokens; i++)
                            free(tokens[i]);
                        return;
                    }
                }
                current = current->next;
            }
        }
    }

    // Create a new record
    Record* new_record = create_record(table, ordered_values);
    if (new_record == NULL) {
        printf("Error: Failed to create record.\n");
         free(ordered_values);
         for(int i=0; i<num_tokens; i++)
             free(tokens[i]);
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

    // Update the B+ Tree index (if applicable)
    if (table->index_tree != NULL) {
        int student_number = atoi(ordered_values[0]); // Assuming first column is student-number
        bptree_insert(table->index_tree, student_number, new_record);
    }

    printf("Record added to table '%s'.\n", table_name);

    // Free the dynamically allocated memory
    free(ordered_values);
    for(int i=0; i<num_tokens; i++)
         free(tokens[i]);
}

// DELETE <table_name> <column_name> <value>
void delete_record_cmd(const char* command) {
    char table_name[MAX_TABLE_NAME_LENGTH];
    char column_name[MAX_COLUMN_NAME_LENGTH];
    char value[MAX_STRING_LENGTH];
    
    char* tokens[5];
    int num_tokens = 0;
    
    // Parse the command into tokens
    num_tokens = parse_command(command, tokens, 5);
    if(num_tokens < 0)
    {
     printf("Error parsing the command\n");
     return;
    }

    if (num_tokens != 4 || strcmp(tokens[0], "DELETE") != 0) {
        printf("Invalid DELETE command. Usage: DELETE <table_name> <column_name> <value>\n");
         for(int i=0; i<num_tokens; i++)
             free(tokens[i]);
        return;
    }
     strncpy(table_name, tokens[1], MAX_TABLE_NAME_LENGTH-1);
     table_name[MAX_TABLE_NAME_LENGTH-1] = '\0';
    strncpy(column_name, tokens[2], MAX_COLUMN_NAME_LENGTH-1);
      column_name[MAX_COLUMN_NAME_LENGTH-1] = '\0';
    strncpy(value, tokens[3], MAX_STRING_LENGTH-1);
    value[MAX_STRING_LENGTH-1] = '\0';

    Table* table = find_table(table_name);
    if (table == NULL) {
        printf("Error: Table '%s' not found.\n", table_name);
         for(int i=0; i<num_tokens; i++)
             free(tokens[i]);
        return;
    }
    int col_index = find_column_index(table, column_name);
    if (col_index == -1) {
        printf("Error: Column '%s' not found in table '%s'.\n", column_name, table_name);
        for(int i=0; i<num_tokens; i++)
             free(tokens[i]);
        return;
    }

    Record* current = table->head;
    while (current != NULL) {
         int match = 0;
         if (strcmp(table->columns[col_index].type, "INTEGER") == 0) {
             if (atoi(value) == *(int*)current->data[col_index]) {
                 match = 1;
             }
         } else if (strcmp(table->columns[col_index].type, "FLOAT") == 0) {
             if (atof(value) == *(float*)current->data[col_index]) {
                 match = 1;
             }
         } else if (strcmp(table->columns[col_index].type, "BOOLEAN") == 0) {
             int bool_val = (strcmp(value, "true") == 0 || strcmp(value, "1") == 0) ? 1 : 0;
             if (bool_val == *(int*)current->data[col_index]) {
                 match = 1;
             }
         } else if (strcmp(table->columns[col_index].type, "DATE") == 0) {
             if (strcmp(value, (char*)current->data[col_index]) == 0) {
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
    for(int i=0; i<num_tokens; i++)
       free(tokens[i]);
}

// UPDATE <table_name> <column_name> <old_value> <new_value>
void update_record_cmd(const char* command) {
    char table_name[MAX_TABLE_NAME_LENGTH];
    char column_name[MAX_COLUMN_NAME_LENGTH];
    char old_value[MAX_STRING_LENGTH];
    char new_value[MAX_STRING_LENGTH];
    char* tokens[6];
    int num_tokens = 0;

    // Parse the command into tokens
    num_tokens = parse_command(command, tokens, 6);
    if (num_tokens < 0)
    {
        printf("Error parsing the command\n");
        return;
    }

    if (num_tokens != 5 || strcmp(tokens[0], "UPDATE") != 0) {
        printf("Invalid UPDATE command.\n");
         for(int i=0; i<num_tokens; i++)
             free(tokens[i]);
        return;
    }
    strncpy(table_name, tokens[1], MAX_TABLE_NAME_LENGTH-1);
      table_name[MAX_TABLE_NAME_LENGTH-1] = '\0';
    strncpy(column_name, tokens[2], MAX_COLUMN_NAME_LENGTH - 1);
    column_name[MAX_COLUMN_NAME_LENGTH - 1] = '\0';
    strncpy(old_value, tokens[3], MAX_STRING_LENGTH - 1);
      old_value[MAX_STRING_LENGTH - 1] = '\0';
    strncpy(new_value, tokens[4], MAX_STRING_LENGTH -1);
     new_value[MAX_STRING_LENGTH - 1] = '\0';

    Table* table = find_table(table_name);
    if (table == NULL) {
        printf("Error: Table '%s' not found.\n", table_name);
        for(int i=0; i<num_tokens; i++)
            free(tokens[i]);
        return;
    }
    int col_index = find_column_index(table, column_name);
    if (col_index == -1) {
        printf("Error: Column '%s' not found in table '%s'.\n", column_name, table_name);
        for(int i=0; i<num_tokens; i++)
            free(tokens[i]);
        return;
    }

      // Validate new value for general-course-score and core-course-score
    if (strcmp(column_name, "general-course-score") == 0 || strcmp(column_name, "core-course-score") == 0) {
            int new_score = atoi(new_value);
            if (!is_valid_score(new_score)) {
                printf("Warning: The entered value for %s (%d) is not in the right range (0 to 20).\n", column_name, new_score);
                printf("Use 'HELP' for more information.\n");
                for(int i=0; i<num_tokens; i++)
                   free(tokens[i]);
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
         } else if (strcmp(table->columns[col_index].type, "FLOAT") == 0) {
             if (atof(old_value) == *(float*)current->data[col_index]) {
                 match = 1;
                 *(float*)current->data[col_index] = atof(new_value);
             }
         } else if (strcmp(table->columns[col_index].type, "BOOLEAN") == 0) {
             int old_bool = (strcmp(old_value, "true") == 0 || strcmp(old_value, "1") == 0) ? 1 : 0;
             if (old_bool == *(int*)current->data[col_index]) {
                 match = 1;
                 int new_bool = (strcmp(new_value, "true") == 0 || strcmp(new_value, "1") == 0) ? 1 : 0;
                 *(int*)current->data[col_index] = new_bool;
             }
         } else if (strcmp(table->columns[col_index].type, "DATE") == 0) {
             if (strcmp(old_value, (char*)current->data[col_index]) == 0) {
                 match = 1;
                 strncpy((char*)current->data[col_index], new_value, 10);
                 ((char*)current->data[col_index])[10] = '\0';
             }
         } else if (strcmp(table->columns[col_index].type, "STRING") == 0) {
             if (strcmp(old_value, (char*)current->data[col_index]) == 0) {
                 match = 1;
                 strncpy((char*)current->data[col_index], new_value, MAX_STRING_LENGTH - 1);
                 ((char*)current->data[col_index])[MAX_STRING_LENGTH - 1] = '\0';
            }
         }
       current = current->next;
    }
    printf("Records updated in table '%s'.\n", table_name);
     for(int i=0; i<num_tokens; i++)
        free(tokens[i]);
}

// SELECT <table_name> <column_name> <value> [SORTED]
void select_records_cmd(const char* command) {
    char table_name[MAX_TABLE_NAME_LENGTH];
    char column_name[MAX_COLUMN_NAME_LENGTH];
    char value[MAX_STRING_LENGTH];
    char condition[10] = "";
    char second_column_name[MAX_COLUMN_NAME_LENGTH];
    char second_value[MAX_STRING_LENGTH];
    char sorted[10] = "";
     char* tokens[20];
    int num_tokens = 0;

    // Parse the command into tokens
    num_tokens = parse_command(command, tokens, 20);
    if (num_tokens < 0)
    {
        printf("Error parsing the command.\n");
        return;
    }
     if (num_tokens < 4 || strcmp(tokens[0], "SELECT") != 0)
    {
      printf("Invalid SELECT command. Usage: SELECT <table_name> <column_name> <value> [AND/OR/NOT <column_name> <value>] [SORTED]\n");
       for (int i = 0; i < num_tokens; i++)
            free(tokens[i]);
      return;
    }
    strncpy(table_name, tokens[1], MAX_TABLE_NAME_LENGTH-1);
     table_name[MAX_TABLE_NAME_LENGTH-1] = '\0';
    strncpy(column_name, tokens[2], MAX_COLUMN_NAME_LENGTH -1);
     column_name[MAX_COLUMN_NAME_LENGTH-1] = '\0';
    strncpy(value, tokens[3], MAX_STRING_LENGTH - 1);
    value[MAX_STRING_LENGTH - 1] = '\0';

    int condition_index = -1;
    int sorted_index = -1;
      for(int i=4; i < num_tokens; i++)
      {
        if(strcmp(tokens[i], "AND")==0 || strcmp(tokens[i], "OR")==0 || strcmp(tokens[i], "NOT")==0)
        {
          condition_index = i;
          if (i + 2 < num_tokens) {
                strncpy(second_column_name, tokens[i + 1], MAX_COLUMN_NAME_LENGTH-1);
                 second_column_name[MAX_COLUMN_NAME_LENGTH-1] = '\0';
                strncpy(second_value, tokens[i + 2], MAX_STRING_LENGTH - 1);
                  second_value[MAX_STRING_LENGTH - 1] = '\0';
              strncpy(condition, tokens[i], 9);
              condition[9] = '\0';
            } else {
               printf("Invalid SELECT command. Missing second column or value for condition.\n");
               for (int j = 0; j < num_tokens; j++)
                   free(tokens[j]);
               return;
            }

            break;
        }
        if(strcmp(tokens[i], "SORTED")==0)
        {
           sorted_index= i;
            strncpy(sorted, tokens[i], 9);
             sorted[9] = '\0';
             break;
        }
      }
    
    Table* table = find_table(table_name);
     if (table == NULL) {
         printf("Error: Table '%s' not found.\n", table_name);
          for (int i = 0; i < num_tokens; i++)
              free(tokens[i]);
         return;
     }
    
    int col_index = find_column_index(table, column_name);
     if (col_index == -1) {
         printf("Error: Column '%s' not found in table '%s'.\n", column_name, table_name);
           for (int i = 0; i < num_tokens; i++)
               free(tokens[i]);
         return;
    }
    int second_col_index = -1;
        if (strlen(condition) > 0) {
            second_col_index = find_column_index(table, second_column_name);
            if (second_col_index == -1) {
                printf("Error: Column '%s' not found in table '%s'.\n", second_column_name, table_name);
                 for (int i = 0; i < num_tokens; i++)
                    free(tokens[i]);
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
            } else if (strcmp(table->columns[col_index].type, "FLOAT") == 0) {
                if (atof(value) == *(float*)current->data[col_index]) {
                    match = 1;
                }
            } else if (strcmp(table->columns[col_index].type, "BOOLEAN") == 0) {
                int bool_val = (strcmp(value, "true") == 0 || strcmp(value, "1") == 0) ? 1 : 0;
                if (bool_val == *(int*)current->data[col_index]) {
                    match = 1;
                }
            } else if (strcmp(table->columns[col_index].type, "DATE") == 0) {
                if (strcmp(value, (char*)current->data[col_index]) == 0) {
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
                 } else if (strcmp(table->columns[second_col_index].type, "FLOAT") == 0) {
                     if (atof(second_value) == *(float*)current->data[second_col_index]) {
                         second_match = 1;
                    }
                 } else if (strcmp(table->columns[second_col_index].type, "BOOLEAN") == 0) {
                     int bool_val = (strcmp(second_value, "true") == 0 || strcmp(second_value, "1") == 0) ? 1 : 0;
                     if (bool_val == *(int*)current->data[second_col_index]) {
                         second_match = 1;
                    }
                 } else if (strcmp(table->columns[second_col_index].type, "DATE") == 0) {
                    if (strcmp(second_value, (char*)current->data[second_col_index]) == 0) {
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
            // Sort by student-number (column index 0)
             matching_records_head = merge_sort(matching_records_head, 0, table);
        }

       // Step 3: Print the records in a table format
        if (matching_records_head != NULL) {
            // Print table headers
            for (int i = 0; i < table->num_columns; i++) {
                printf("%-20s", table->columns[i].name);
            }
            printf("\n");

           // Print separator line
            for (int i = 0; i < table->num_columns; i++) {
                 printf("--------------------");
            }
            printf("\n");

            // Print records
             Record* print_current = matching_records_head;
             while (print_current != NULL) {
                for (int i = 0; i < table->num_columns; i++) {
                     if (strcmp(table->columns[i].type, "INTEGER") == 0) {
                         printf("%-20d", *(int*)print_current->data[i]);
                     } else if (strcmp(table->columns[i].type, "FLOAT") == 0) {
                         printf("%-20.2f", *(float*)print_current->data[i]);
                     } else if (strcmp(table->columns[i].type, "BOOLEAN") == 0) {
                         printf("%-20s", *(int*)print_current->data[i] ? "true" : "false");
                     } else if (strcmp(table->columns[i].type, "DATE") == 0) {
                         printf("%-20s", (char*)print_current->data[i]);
                     } else {
                         printf("%-20s", (char*)print_current->data[i]);
                    }
                }
                 printf("\n");
                 print_current = print_current->next;
            }
        } else {
             printf("No matching records found.\n");
        }
     // Step 4: Free the temporary linked list of matching records
        Record* temp = matching_records_head;
        while (temp != NULL) {
             Record* next = temp->next;
            free(temp);
            temp = next;
        }
        for (int i = 0; i < num_tokens; i++)
           free(tokens[i]);
}
// SAVE <table_name> <filename>
void save_cmd(const char* command) {
    char table_name[MAX_TABLE_NAME_LENGTH];
    char filename[256];
    
    char* tokens[5];
    int num_tokens = 0;
    
     // Parse the command into tokens
    num_tokens = parse_command(command, tokens, 5);
     if (num_tokens < 0)
    {
         printf("Error parsing the command.\n");
         return;
     }
    if (num_tokens != 4 || strcmp(tokens[0], "SAVE") != 0)
     {
      printf("Invalid SAVE command. Usage: SAVE <table_name> <filename> CSV\n");
       for (int i = 0; i < num_tokens; i++)
          free(tokens[i]);
        return;
     }

    strncpy(table_name, tokens[1], MAX_TABLE_NAME_LENGTH - 1);
      table_name[MAX_TABLE_NAME_LENGTH - 1] = '\0';
    strncpy(filename, tokens[2], 255);
    filename[255] = '\0';

    if (strcmp(tokens[3], "CSV")!=0)
    {
         printf("Invalid SAVE command. Usage: SAVE <table_name> <filename> CSV\n");
        for (int i = 0; i < num_tokens; i++)
            free(tokens[i]);
        return;
    }
     Table* table = find_table(table_name);
        if (table == NULL) {
           printf("Error: Table '%s' not found.\n", table_name);
            for (int i = 0; i < num_tokens; i++)
                free(tokens[i]);
            return;
        }
        
      save_to_csv(filename, table);
      for (int i = 0; i < num_tokens; i++)
            free(tokens[i]);
}

// LOAD <table_name> <filename>
void load_cmd(const char* command) {
     char table_name[MAX_TABLE_NAME_LENGTH];
    char filename[256];
    char* tokens[5];
    int num_tokens = 0;

     // Parse the command into tokens
    num_tokens = parse_command(command, tokens, 5);
     if (num_tokens < 0)
    {
        printf("Error parsing the command.\n");
        return;
    }
    if (num_tokens != 4 || strcmp(tokens[0], "LOAD") != 0) {
        printf("Invalid LOAD command. Usage: LOAD <table_name> <filename> CSV\n");
        for (int i = 0; i < num_tokens; i++)
            free(tokens[i]);
        return;
    }
    strncpy(table_name, tokens[1], MAX_TABLE_NAME_LENGTH-1);
      table_name[MAX_TABLE_NAME_LENGTH-1] = '\0';
    strncpy(filename, tokens[2], 255);
    filename[255] = '\0';
    
    if (strcmp(tokens[3], "CSV")!=0)
    {
         printf("Invalid LOAD command. Usage: LOAD <table_name> <filename> CSV\n");
         for (int i = 0; i < num_tokens; i++)
            free(tokens[i]);
        return;
    }

     Table* table = find_table(table_name);
        if (table == NULL) {
           printf("Error: Table '%s' not found.\n", table_name);
            for (int i = 0; i < num_tokens; i++)
                free(tokens[i]);
            return;
        }
        load_from_csv(filename, table);
      for (int i = 0; i < num_tokens; i++)
            free(tokens[i]);
}

void select_records_where_cmd(const char* command) {
     char table_name[MAX_TABLE_NAME_LENGTH];
    char condition[256] = "";
    char sorted[10] = "";
    char* tokens[10];
    int num_tokens = 0;

    // Parse the command into tokens
    num_tokens = parse_command(command, tokens, 10);
    if (num_tokens < 0)
    {
         printf("Error parsing the command.\n");
         return;
    }
    if (num_tokens < 4 || strcmp(tokens[0], "SELECT") != 0 || strcmp(tokens[2], "WHERE")!=0 ) {
        printf("Invalid SELECT WHERE command.\n");
        for (int i = 0; i < num_tokens; i++)
            free(tokens[i]);
        return;
    }
    strncpy(table_name, tokens[1], MAX_TABLE_NAME_LENGTH-1);
      table_name[MAX_TABLE_NAME_LENGTH-1] = '\0';

     // Reconstruct the condition string with remaining tokens
        int condition_start_index = 3;
        int sorted_index = -1;
        for (int i = 3; i < num_tokens; i++)
        {
          if(strcmp(tokens[i], "SORTED")==0)
          {
            sorted_index = i;
            break;
          }
        }
         
        int len = 0;
          for (int i = condition_start_index; i < num_tokens; i++)
        {
            if (sorted_index !=-1 && i == sorted_index)
               break;
            len+=strlen(tokens[i]);
             if(i!=condition_start_index)
                len++;
        }
         char* condition_ptr = condition;
        for (int i = condition_start_index; i < num_tokens; i++)
        {
           if (sorted_index != -1 && i == sorted_index)
            {
                 break;
            }
            strncpy(condition_ptr,tokens[i],255 - (condition_ptr-condition));
            condition_ptr+= strlen(tokens[i]);
            if(i!=condition_start_index)
            {
                 strncpy(condition_ptr," ", 1);
                condition_ptr+=1;
            }
        }
         if(sorted_index !=-1)
        {
           strncpy(sorted, tokens[sorted_index],9);
             sorted[9] = '\0';
        }
    Table* table = find_table(table_name);
    if (table == NULL) {
        printf("Error: Table '%s' not found.\n", table_name);
         for (int i = 0; i < num_tokens; i++)
            free(tokens[i]);
        return;
    }
        
      char column_name[MAX_COLUMN_NAME_LENGTH];
       char operator[3];
      char value[MAX_STRING_LENGTH];

      if (sscanf(condition, "%s %s %s", column_name, operator, value) != 3) {
            printf("Error: Invalid WHERE condition.\n");
            for (int i = 0; i < num_tokens; i++)
                free(tokens[i]);
            return;
        }

        int col_index = find_column_index(table, column_name);
        if (col_index == -1) {
            printf("Error: Column '%s' not found in table '%s'.\n", column_name, table_name);
            for (int i = 0; i < num_tokens; i++)
                free(tokens[i]);
            return;
        }
        
     // Find matching records
        Record* matching_records_head = NULL;
        Record* matching_records_tail = NULL;
        Record* current = table->head;

        while (current != NULL) {
            int match = 0;
             if (strcmp(table->columns[col_index].type, "INTEGER") == 0) {
                int record_value = *(int*)current->data[col_index];
                int condition_value = atoi(value);

                if (strcmp(operator, "=") == 0 && record_value == condition_value) match = 1;
                else if (strcmp(operator, "!=") == 0 && record_value != condition_value) match = 1;
                else if (strcmp(operator, ">") == 0 && record_value > condition_value) match = 1;
                else if (strcmp(operator, "<") == 0 && record_value < condition_value) match = 1;
                else if (strcmp(operator, ">=") == 0 && record_value >= condition_value) match = 1;
                else if (strcmp(operator, "<=") == 0 && record_value <= condition_value) match = 1;
            } else if (strcmp(table->columns[col_index].type, "STRING") == 0) {
                 if (strcmp(operator, "=") == 0 && strcmp((char*)current->data[col_index], value) == 0) match = 1;
                else if (strcmp(operator, "!=") == 0 && strcmp((char*)current->data[col_index], value) != 0) match = 1;
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
    
        // Sort records if SORTED is specified
        if (strcmp(sorted, "SORTED") == 0) {
             matching_records_head = merge_sort(matching_records_head, 0, table);
        }
       
        // Print matching records in a table format
       printf("Selected records from table '%s':\n", table_name);
       
         if (matching_records_head != NULL) {
            // Print table headers
            for (int i = 0; i < table->num_columns; i++) {
                printf("%-20s", table->columns[i].name);
            }
            printf("\n");

            // Print separator line
            for (int i = 0; i < table->num_columns; i++) {
                printf("--------------------");
            }
            printf("\n");

            // Print records
            Record* print_current = matching_records_head;
            while (print_current != NULL) {
                for (int i = 0; i < table->num_columns; i++) {
                    if (strcmp(table->columns[i].type, "INTEGER") == 0) {
                        printf("%-20d", *(int*)print_current->data[i]);
                    } else {
                        printf("%-20s", (char*)print_current->data[i]);
                    }
                }
                printf("\n");
                print_current = print_current->next;
            }
        } else {
            printf("No matching records found.\n");
        }

        // Free temporary linked list
        Record* temp = matching_records_head;
        while (temp != NULL) {
             Record* next = temp->next;
            free(temp);
             temp = next;
        }
    for (int i = 0; i < num_tokens; i++)
      free(tokens[i]);
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

    // CREATE INDEX command
    printf("3. CREATE INDEX <table_name>\n");
    printf("   - Creates an index on the primary key (student-number) of the specified table.\n");
    printf("   - Example: CREATE INDEX students\n\n");

    // ADD command
    printf("4. ADD <table_name> <column_name_1> <value_1> ... <column_name_n> <value_n>\n");
    printf("   - Adds a new record to the specified table.\n");
    printf("   - Example: ADD students student-number 123 name \"John Doe\" score 85\n\n");

    // DELETE command
    printf("5. DELETE <table_name> <column_name> <value>\n");
    printf("   - Deletes records from the specified table where the column matches the value.\n");
    printf("   - Example: DELETE students student-number 123\n\n");

    // UPDATE command
    printf("6. UPDATE <table_name> <column_name> <old_value> <new_value>\n");
    printf("   - Updates records in the specified table where the column matches the old value.\n");
    printf("   - Example: UPDATE students score 85 90\n\n");

    // SELECT command
     printf("7. SELECT <table_name> <column_name> <value> [AND/OR/NOT <column_name> <value>] [SORTED]\n");
    printf("   - Selects records from the specified table where the column matches the value.\n");
      printf("   - AND, OR, NOT condtions are optional after the value\n");
    printf("   - Use the optional SORTED keyword to sort the results by student-number.\n");
    printf("   - Example: SELECT students score 85 SORTED\n\n");

    // SELECT WHERE command
    printf("8. SELECT <table_name> WHERE <condition> [SORTED]\n");
    printf("   - Selects records from the specified table based on a condition.\n");
    printf("   - Conditions can include comparisons like '=', '!=', '>', '<', '>=', '<='.\n");
    printf("   - Use the optional SORTED keyword to sort the results by student-number.\n");
    printf("   - Example: SELECT students WHERE score > 85 SORTED\n\n");

    // SAVE CSV command
    printf("9. SAVE <table_name> <filename> CSV\n");
    printf("   - Saves the specified table to a CSV file.\n");
    printf("   - Example: SAVE students data.csv\n\n");

    // LOAD CSV command
    printf("10. LOAD <table_name> <filename> CSV\n");
    printf("    - Loads the specified table from a CSV file.\n");
    printf("    - Example: LOAD students data.csv\n\n");

     // SAVE BINARY command
    printf("11. SAVE BINARY <table_name> <filename>\n");
    printf("    - Saves the specified table to a binary file.\n");
    printf("    - Example: SAVE BINARY students data.bin\n\n");

    // LOAD BINARY command
    printf("12. LOAD BINARY <table_name> <filename>\n");
    printf("    - Loads the specified table from a binary file.\n");
    printf("    - Example: LOAD BINARY students data.bin\n\n");

    // BEGIN TRANSACTION command
    printf("13. BEGIN\n");
    printf("    - Starts a new transaction. All changes made after this command can be rolled back.\n");
    printf("    - Example: BEGIN\n\n");

    // COMMIT TRANSACTION command
    printf("14. COMMIT\n");
    printf("    - Commits the current transaction, saving all changes made since the last BEGIN.\n");
    printf("    - Example: COMMIT\n\n");

    // ROLLBACK TRANSACTION command
    printf("15. ROLLBACK\n");
    printf("    - Rolls back the current transaction, undoing all changes made since the last BEGIN.\n");
    printf("    - Example: ROLLBACK\n\n");

    // HELP command
    printf("16. HELP\n");
    printf("    - Displays this help message.\n\n");

    // EXIT command
    printf("17. EXIT\n");
    printf("    - Exits the program.\n\n");

    // Additional information
    printf("Note:\n");
    printf("- Column types can be either 'INTEGER' or 'STRING'.\n");
    printf("- Constraints can include 'UNIQUE', 'PRIMARY KEY', and 'NOT NULL'.\n");
    printf("- The 'general-course-score' and 'core-course-score' columns must be between 0 and 20.\n");
}