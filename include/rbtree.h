#ifndef RBTREE_H
#define RBTREE_H

#include "data_structures.h"

RBTreeNode* rbt_create_node(int student_number, Record* record);
RBTreeNode* rbt_insert(RBTreeNode* root, int student_number, Record* record);
void rbt_insert_fixup(RBTreeNode** root, RBTreeNode* node);
void rbt_left_rotate(RBTreeNode** root, RBTreeNode* x);
void rbt_right_rotate(RBTreeNode** root, RBTreeNode* y);
RBTreeNode* rbt_search(RBTreeNode* root, int student_number);
RBTreeNode* rbt_find_min(RBTreeNode* node);
RBTreeNode* rbt_transplant(RBTreeNode** root, RBTreeNode* u, RBTreeNode* v);
RBTreeNode* rbt_delete(RBTreeNode** root, int student_number);
void rbt_delete_fixup(RBTreeNode** root, RBTreeNode* x);
void rbt_free_tree(RBTreeNode* node);

#endif // RBTREE_H