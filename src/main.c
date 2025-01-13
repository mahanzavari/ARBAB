#include <stdio.h>
#define _POSIX_C_SOURCE 200809L  // Add this line before including string.h
#include <string.h>              // Ensure this is included in command.c
#include <stdlib.h>
#include "../include/command.h"
#include "../include/hashmap.h"
#include "../include/table.h"
#include "../include/record.h"  // Include record.h for Record type
int main() {
    char command[256];
    extern HashMap hashmap;
    for(int i = 0; i < HASHMAP_SIZE; i++) {
        hashmap.buckets[i] = NULL;
    }
    while (1) {
        printf("Enter command: ");
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = 0;

        if (strncmp(command, "CREATE TABLE", 12) == 0) {
            create_table_cmd(command);
        } else if (strncmp(command, "DELETE TABLE", 12) == 0) {
            delete_table_cmd(command);
        } else if (strncmp(command, "CREATE INDEX", 12) == 0) {
            create_index_cmd(command);
        } else if (strncmp(command, "ADD", 3) == 0) {
            add_record_cmd(command);
        } else if (strncmp(command, "DELETE", 6) == 0) {
            delete_record_cmd(command);
        } else if (strncmp(command, "UPDATE", 6) == 0) {
            update_record_cmd(command);
        } else if (strncmp(command, "SELECT", 6) == 0) {
            select_records_cmd(command);
        } else if (strcmp(command, "HELP") == 0) {
            help();
        } else if (strcmp(command, "exit") == 0) {
            break;
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

            // Free the Red-Black Tree index
            rbt_free_tree(table->index_root);

            // Free the table structure
            free(table);

            // Free the hashmap node
            free(current);
            current = next;
        }
    }

    return 0;
}