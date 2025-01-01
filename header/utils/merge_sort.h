#ifndef MERGE_H


#include "../../header/record.h"





Record* merge(Record* left_half, Record* right_half, int col_index);
Record* split(Record* head);
Record* merge_sort(Record* head, int col_index);

#endif 