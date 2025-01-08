#ifndef UTILS_H
#define UTILS_H

int parse_command(const char* command, char** tokens, int max_tokens);
Record* merge(Record* left_half, Record* right_half, int col_index);
Record* split(Record* head);
Record* merge_sort(Record* head, int col_index);

#endif // UTILS_H