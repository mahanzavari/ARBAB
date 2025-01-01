#include "../../header/utils/merge_sort.h"
#include "../header/record.h" // For accessing `Record` structure
#include <stdlib.h>


// Function to merge two sorted subarrays
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
Record* merge_sort(Record* head, int col_index) {
    if (!head || !head->next) {
        return head;
    }
    Record* right_half = split(head);
    Record* left_half = merge_sort(head, col_index);
    right_half = merge_sort(right_half, col_index);
    return merge(left_half, right_half, col_index);
}
