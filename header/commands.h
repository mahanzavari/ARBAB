#ifndef COMMANDS_H
#define COMMANDS_H

#include "table.h"
#include "record.h"
#include "rbtree.h"
#include "utils/merge_sort.h"
#include "utils/hashmap.h"




void create_table_cmd(const char* command);
void delete_table_cmd(const char* command);
void create_index_cmd(const char* command);
void select_records_cmd(const char* command);
void update_record_cmd(const char* command);
void add_record_cmd(const char* command);
void delete_record_cmd(const char* command);


#endif