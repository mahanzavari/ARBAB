#include "../header/rbtree.h"
#include "../header/record.h"
#include <stdlib.h>
#include <string.h>




RBTreeNode* rbt_create_node(int student_number, Record* record) {
    RBTreeNode* node = (RBTreeNode*)malloc(sizeof(RBTreeNode));
    if (!node) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    node->student_number = student_number;
    node->record = record;
    node->parent = NULL;
    node->left = NULL;
    node->right = NULL;
    node->color = 'R'; // New nodes are always red
    return node;
}



RBTreeNode* rbt_insert(RBTreeNode* root, int student_number, Record* record) {
    RBTreeNode* y = NULL;
    RBTreeNode* x = root;
    while (x != NULL) {
        y = x;
        if (student_number < x->student_number) {
            x = x->left;
        } else {
            x = x->right;
        }
    }
    RBTreeNode* z = rbt_create_node(student_number, record);
    z->parent = y;
    if (y == NULL) {
        root = z;
    } else if (z->student_number < y->student_number) {
        y->left = z;
    } else {
        y->right = z;
    }
    rbt_insert_fixup(&root, z);
    return root;
}

void rbt_insert_fixup(RBTreeNode** root, RBTreeNode* z) {
    while (z->parent != NULL && z->parent->color == 'R') {
        if (z->parent == z->parent->parent->left) {
            RBTreeNode* y = z->parent->parent->right;
            if (y != NULL && y->color == 'R') {
                z->parent->color = 'B';
                y->color = 'B';
                z->parent->parent->color = 'R';
                z = z->parent->parent;
            } else {
                if (z == z->parent->right) {
                    z = z->parent;
                    rbt_left_rotate(root, z);
                }
                z->parent->color = 'B';
                z->parent->parent->color = 'R';
                rbt_right_rotate(root, z->parent->parent);
            }
        } else {
            RBTreeNode* y = z->parent->parent->left;
            if (y != NULL && y->color == 'R') {
                z->parent->color = 'B';
                y->color = 'B';
                z->parent->parent->color = 'R';
                z = z->parent->parent;
            } else {
                if (z == z->parent->left) {
                    z = z->parent;
                    rbt_right_rotate(root, z);
                }
                z->parent->color = 'B';
                z->parent->parent->color = 'R';
                rbt_left_rotate(root, z->parent->parent);
            }
        }
    }
    (*root)->color = 'B'; // Root is always black
}

void rbt_left_rotate(RBTreeNode** root, RBTreeNode* x) {
    RBTreeNode* y = x->right;
    x->right = y->left;
    if (y->left != NULL) {
        y->left->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == NULL) {
        *root = y;
    } else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
}

void rbt_right_rotate(RBTreeNode** root, RBTreeNode* y) {
    RBTreeNode* x = y->left;
    y->left = x->right;
    if (x->right != NULL) {
        x->right->parent = y;
    }
    x->parent = y->parent;
    if (y->parent == NULL) {
        *root = x;
    } else if (y == y->parent->left) {
        y->parent->left = x;
    } else {
        y->parent->right = x;
    }
    x->right = y;
    y->parent = x;
}

RBTreeNode* rbt_search(RBTreeNode* root, int student_number) {
    while (root != NULL && student_number != root->student_number) {
        if (student_number < root->student_number) {
            root = root->left;
        } else {
            root = root->right;
        }
    }
    return root;
}

RBTreeNode* rbt_find_min(RBTreeNode* node) {
    while (node->left != NULL) {
        node = node->left;
    }
    return node;
}

RBTreeNode* rbt_transplant(RBTreeNode** root, RBTreeNode* u, RBTreeNode* v) {
    if (u->parent == NULL) {
        *root = v;
    } else if (u == u->parent->left) {
        u->parent->left = v;
    } else {
        u->parent->right = v;
    }
    if (v != NULL) {
        v->parent = u->parent;
    }
    return *root;
}

RBTreeNode* rbt_delete(RBTreeNode** root, int student_number) {
    RBTreeNode* z = rbt_search(*root, student_number);
    if (z == NULL) {
        return *root; // Node not found
    }

    RBTreeNode* y = z;
    char y_original_color = y->color;
    RBTreeNode* x;

    if (z->left == NULL) {
        x = z->right;
        *root = rbt_transplant(root, z, z->right);
    } else if (z->right == NULL) {
        x = z->left;
        *root = rbt_transplant(root, z, z->left);
    } else {
        y = rbt_find_min(z->right);
        y_original_color = y->color;
        x = y->right;
        if (y->parent == z) {
            if (x != NULL) {
                x->parent = y;
            }
        } else {
            *root = rbt_transplant(root, y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }
        *root = rbt_transplant(root, z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }
    free(z);
    if (y_original_color == 'B' && x != NULL) {
        rbt_delete_fixup(root, x);
    }
    return *root;
}

void rbt_delete_fixup(RBTreeNode** root, RBTreeNode* x) {
    while (x != *root && (x == NULL || x->color == 'B')) {
        if (x == x->parent->left) {
            RBTreeNode* w = x->parent->right;
            if (w != NULL && w->color == 'R') {
                w->color = 'B';
                x->parent->color = 'R';
                rbt_left_rotate(root, x->parent);
                w = x->parent->right;
            }
            if (w != NULL && (w->left == NULL || w->left->color == 'B') && (w->right == NULL || w->right->color == 'B')) {
                w->color = 'R';
                x = x->parent;
            } else {
                if (w != NULL && (w->right == NULL || w->right->color == 'B')) {
                    w->left->color = 'B';
                    w->color = 'R';
                    rbt_right_rotate(root, w);
                    w = x->parent->right;
                }
                if (w != NULL) {
                    w->color = x->parent->color;
                    x->parent->color = 'B';
                    w->right->color = 'B';
                    rbt_left_rotate(root, x->parent);
                    x = *root;
                } else {
                    break; // Handle the case where w is NULL
                }
            }
        } else {
            RBTreeNode* w = x->parent->left;
            if (w != NULL && w->color == 'R') {
                w->color = 'B';
                x->parent->color = 'R';
                rbt_right_rotate(root, x->parent);
                w = x->parent->left;
            }
            if (w != NULL && (w->right == NULL || w->right->color == 'B') && (w->left == NULL || w->left->color == 'B')) {
                w->color = 'R';
                x = x->parent;
            } else {
                if (w != NULL && (w->left == NULL || w->left->color == 'B')) {
                    w->right->color = 'B';
                    w->color = 'R';
                    rbt_left_rotate(root, w);
                    w = x->parent->left;
                }
                if (w != NULL) {
                    w->color = x->parent->color;
                    x->parent->color = 'B';
                    w->left->color = 'B';
                    rbt_right_rotate(root, x->parent);
                    x = *root;
                } else {
                    break; // Handle the case where w is NULL
                }
            }
        }
    }
    if (x != NULL) {
        x->color = 'B';
    }
}

void rbt_free_tree(RBTreeNode* node) {
    if (node != NULL) {
        rbt_free_tree(node->left);
        rbt_free_tree(node->right);
        free(node);
    }
}
