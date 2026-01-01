// for turning off the safety warnings about the safety scanf 
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#define _POSIX_C_SOURCE 200809L  // Add this line before including string.h
#include <string.h>              
#include <stdlib.h>
#include <ctype.h>
#include "../include/command.h"
#include "../include/hashmap.h"
#include "../include/table.h"
#include "../include/record.h"  
#include "../include/file_io.h"
#include "../include/bplustree.h"

// Helper function to convert string to uppercase
static void to_uppercase(char* str) {
    for (int i = 0; str[i]; i++) {
        str[i] = toupper((unsigned char)str[i]);
    }
}

int main() {
    char command[256];
    char command_upper[256];
    extern HashMap hashmap;

    // Initialize the hashmap
    for (int i = 0; i < HASHMAP_SIZE; i++) {
        hashmap.buckets[i] = NULL;
    }

    // Main command loop
    while (1) {
        printf("Enter command: ");
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = 0; // Remove newline character
        
        // Create uppercase version for keyword matching
        strncpy(command_upper, command, sizeof(command_upper) - 1);
        command_upper[sizeof(command_upper) - 1] = '\0';
        to_uppercase(command_upper);        // Create uppercase version for keyword matching
        strncpy(command_upper, command, sizeof(command_upper) - 1);
        command_upper[sizeof(command_upper) - 1] = '\0';
        to_uppercase(command_upper);

        // Handle commands (use uppercase for matching)
        if (strncmp(command_upper, "CREATE TABLE", 12) == 0) {
            create_table_cmd(command);
        } else if (strncmp(command_upper, "DELETE TABLE", 12) == 0) {
            delete_table_cmd(command);
        } else if (strncmp(command_upper, "CREATE INDEX", 12) == 0) {
            create_index_cmd(command);
        } else if (strncmp(command_upper, "ADD", 3) == 0) {
            add_record_cmd(command);
        } else if (strncmp(command_upper, "DELETE", 6) == 0) {
            delete_record_cmd(command);
        } else if (strncmp(command_upper, "UPDATE", 6) == 0) {
            update_record_cmd(command);
        } else if (strncmp(command_upper, "SELECT", 6) == 0) {
            if (strstr(command_upper, "WHERE") != NULL) {
                select_records_where_cmd(command); // Handle SELECT with WHERE
            } else {
                select_records_cmd(command); // Handle regular SELECT
            }
        } else if (strncmp(command_upper, "JOIN", 4) == 0) {
            join_tables_cmd(command);
        } else if (strcmp(command_upper, "HELP") == 0) {
            help();
        } else if (strcmp(command_upper, "BEGIN") == 0) {
            begin_transaction(); // Start a transaction
        } else if (strcmp(command_upper, "COMMIT") == 0) {
            commit_transaction(); // Commit the transaction
        } else if (strcmp(command_upper, "ROLLBACK") == 0) {
            rollback_transaction(); // Rollback the transaction
        } else if (strncmp(command_upper, "SAVE BINARY", 11) == 0) {
            char table_name[MAX_TABLE_NAME_LENGTH];
            char filename[256];
            if (sscanf(command, "SAVE BINARY %s %s", table_name, filename) == 2) {
                Table* table = find_table(table_name);
                if (table == NULL) {
                    printf("Error: Table '%s' not found.\n", table_name);
                } else {
                    save_to_binary(filename, table); // Save table to binary file
                }
            } else {
                printf("Invalid SAVE BINARY command.\n");
            }
        } else if (strncmp(command_upper, "LOAD BINARY", 11) == 0) {
            char table_name[MAX_TABLE_NAME_LENGTH];
            char filename[256];
            if (sscanf(command, "LOAD BINARY %s %s", table_name, filename) == 2 ||
                sscanf(command, "load binary %s %s", table_name, filename) == 2) {
                Table* table = find_table(table_name);
                if (table == NULL) {
                    printf("Error: Table '%s' not found.\n", table_name);
                } else {
                    load_from_binary(filename, table); // Load table from binary file
                }
            } else {
                printf("Invalid LOAD BINARY command.\n");
            }
        } else if (strncmp(command_upper, "LOAD SCHEMA", 11) == 0) {
            load_schema_cmd(command);
        } else if (strcmp(command_upper, "EXIT") == 0 || strcmp(command_upper, "QUIT") == 0) {
            break; // Exit the program
        } else {
            printf("Invalid command. Type 'HELP' for a list of commands.\n");
        }
    }

    // Free allocated memory using the hashmap
    for (int i = 0; i < HASHMAP_SIZE; i++) {
        HashMapNode* current = hashmap.buckets[i];
        while (current != NULL) {
            HashMapNode* next = current->next;
            Table* table = current->table;

            // Free all records in the table
            Record* record = table->head;
            while (record != NULL) {
                Record* next_record = record->next;
                free_record_data(table, record);
                record = next_record;
            }

            // Free the B+ Tree index
            if (table->index_tree != NULL) {
                bptree_free(table->index_tree);
            }

            // Free the table structure
            free(table);

            // Free the hashmap node
            free(current);
            current = next;
        }
    }

    return 0;
}