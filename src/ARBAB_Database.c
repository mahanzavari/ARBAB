#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/table.h"
#include "../include/rbtree.h"
#include "../include/commands.h"
#include "../include/hashmap.h"

// Define global variables
HashMap hashmap; // Global hashmap
Table* tables[10]; // Global array of tables
int num_tables = 0; // Global counter for tables

HashMap hashmap; // Global hashmap to store tables

int main() {
    char command[256];

    while (1) {
        printf("Enter command: ");
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = 0; // Remove trailing newline

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
        } else if (strcmp(command, "exit") == 0) {
            break;
        } else {
            printf("Invalid command.\n");
        }
    }

    // Free allocated memory (tables and their records/indices) - Not fully implemented for brevity
    for (int i = 0; i < HASHMAP_SIZE; i++) {
        HashMapNode* current = hashmap.buckets[i];
        while (current != NULL) {
            HashMapNode* next = current->next;
            Table* table = current->table;
            Record* record = table->head;
            while (record != NULL) {
                Record* next_record = record->next;
                free_record_data(table, record);
                record = next_record;
            }
            rbt_free_tree(table->index_root);
            free(table);
            free(current);
            current = next;
        }
    }

    return 0;
}