#ifndef COMMAND_H
#define COMMAND_H

#include "table.h"
#include "hashmap.h"
#include "rbtree.h"
#include "record.h"
#include "utils.h"

// Function prototypes
void create_table_cmd(const char* command);
void delete_table_cmd(const char* command);
void create_index_cmd(const char* command);
void add_record_cmd(const char* command);
void delete_record_cmd(const char* command);
void update_record_cmd(const char* command);
void select_records_cmd(const char* command);
void help();
int parse_command(const char* command, char** tokens, int max_tokens);

#endif