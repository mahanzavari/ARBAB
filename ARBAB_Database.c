#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<arbab.h>



// Table* tables[10]; // Array to store tables (fixed size for simplicity)
#include "hashmap.h"

HashMap* table_map;

int num_tables = 0;




// fnuction headers
void help(void);
// use void when you get bugs 
int main(int args , char *argv[]){
     char command[256];
     // the input logic
          printf("--------ARBAB Database--------\nby Mahan Zavari\nUse the command 'help' for more information about the allowed commands");
     while(1){
          printf(">> ");
          fgets(command , sizeof(command) , stdin);
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
        } else if (strcmp(command, "help") == 0) {
            help();
        } else if (strcmp(command, "exit") == 0) {
            break;
        } else {
            printf("Invalid command.\n");
        }

     }
}
void help(void){
     printf("Available commands:\n");
     printf("  CREATE TABLE <table_name>\n");
     printf("  DELETE TABLE <table_name>\n");
     printf("  CREATE INDEX <table_name>\n");
     printf("  ADD <table_name> <col1_name> <col1_value> <col2_name> <col2_value> ...\n");
     printf("  DELETE <table_name> <column_name> <value>\n");
     printf("  UPDATE <table_name> <column_name> <old_value> <new_value>\n");
     printf("  SELECT <table_name> <column_name> <value> [SORTED]\n");
     printf("  help\n");
     printf("  exit\n");
}