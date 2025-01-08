#ifndef COMMANDS_H
#define COMMANDS_H

void create_table_cmd(const char* command);
void delete_table_cmd(const char* command);
void create_index_cmd(const char* command);
void add_record_cmd(const char* command);
void delete_record_cmd(const char* command);
void update_record_cmd(const char* command);
void select_records_cmd(const char* command);

#endif // COMMANDS_H