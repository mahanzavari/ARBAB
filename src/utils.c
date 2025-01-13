#include "../include/utils.h"
#include <stdlib.h>
#define _POSIX_C_SOURCE 200809L  // Add this line before including string.h
#include <string.h>              // Ensure this is included in command.c
#include <stdio.h>
#include "../include/record.h"


int is_valid_score(int score) {
    return score >= 0 && score <= 20;
}

// Function to merge two sorted subarrays
Record* merge(Record* left_half, Record* right_half, int col_index, struct Table* table) {
    if (!left_half) return right_half;
    if (!right_half) return left_half;

    Record* result = NULL;

    // Check the column type (INTEGER or STRING)
    if (strcmp(table->columns[col_index].type, "INTEGER") == 0) {
        // Compare integer values
        if (*(int*)left_half->data[col_index] <= *(int*)right_half->data[col_index]) {
            result = left_half;
            result->next = merge(left_half->next, right_half, col_index, table);
            if (result->next) result->next->prev = result;
        } else {
            result = right_half;
            result->next = merge(left_half, right_half->next, col_index, table);
             if (result->next) result->next->prev = result;
        }
    } else if (strcmp(table->columns[col_index].type, "STRING") == 0) {
        // Compare string values
        if (strcmp((char*)left_half->data[col_index], (char*)right_half->data[col_index]) <= 0) {
            result = left_half;
            result->next = merge(left_half->next, right_half, col_index, table);
            if (result->next) result->next->prev = result;
        } else {
            result = right_half;
            result->next = merge(left_half, right_half->next, col_index, table);
            if (result->next) result->next->prev = result;
        }
    }

    return result;
}

// Function to split the linked list for merge sort
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

// Main merge sort function for linked list
Record* merge_sort(Record* head, int col_index, struct Table* table) {
    if (!head || !head->next) {
        return head;
    }
    Record* right_half = split(head);
    Record* left_half = merge_sort(head, col_index, table);
    right_half = merge_sort(right_half, col_index, table);
    return merge(left_half, right_half, col_index, table);
}