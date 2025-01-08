#include "utils.h"
#include <stdlib.h>
#include <string.h>

int parse_command(const char* command, char** tokens, int max_tokens) {
    int token_index = 0;
    const char* ptr = command;
    while (token_index < max_tokens && *ptr != '\0') {
        while (*ptr == ' ' && *ptr != '\0') ptr++;
        if (*ptr == '"') {
            ptr++;
            const char* token_start = ptr;
            while (*ptr != '"' && *ptr != '\0') ptr++;
            if (*ptr == '"') {
                tokens[token_index] = malloc(ptr - token_start + 1);
                if (tokens[token_index] == NULL) {
                    return -1;
                }
                strncpy((char*)tokens[token_index], (char*)token_start, ptr - token_start);
                tokens[token_index][ptr - token_start] = '\0';
                token_index++;
                ptr++;
            } else {
                return -1;
            }
        } else if (*ptr != '\0') {
            const char* token_start = ptr;
            while (*ptr != ' ' && *ptr != '\0') ptr++;
            int token_len = ptr - token_start;
            tokens[token_index] = malloc(token_len + 1);
            if (tokens[token_index] == NULL) {
                return -1;
            }
            strncpy((char*)tokens[token_index], (char*)token_start, token_len);
            tokens[token_index][token_len] = '\0';
            token_index++;
        }
    }
    return token_index;
}

Record* merge(Record* left_half, Record* right_half, int col_index) {
    if (!left_half) return right_half;
    if (!right_half) return left_half;

    Record* result = NULL;

    if (strcmp(tables[0]->columns[col_index].type, "INTEGER") == 0) {
        if (*(int*)left_half->data[col_index] <= *(int*)right_half->data[col_index]) {
            result = left_half;
            result->next = merge(left_half->next, right_half, col_index);
            if (result->next) result->next->prev = result;
        } else {
            result = right_half;
            result->next = merge(left_half, right_half->next, col_index);
            if (result->next) result->next->prev = result;
        }
    }
    return result;
}

Record* split(Record* head) {
    Record* fast = head;
    Record* slow = head;
    while (fast->next && fast->next->next) {
        fast = fast->next->next;
        slow = slow->next;
    }
    Record* temp = slow->next;
    slow->next = NULL;
    return temp;
}

Record* merge_sort(Record* head, int col_index) {
    if (!head || !head->next) {
        return head;
    }
    Record* right_half = split(head);
    Record* left_half = merge_sort(head, col_index);
    right_half = merge_sort(right_half, col_index);
    return merge(left_half, right_half, col_index);
}