#ifndef MY_DEREVO_H_
#define MY_DEREVO_H_

#include "tree_error_types.h"
#include <stdlib.h>
typedef int TreeElement;

typedef struct Node Node;

struct Node {
    TreeElement data;
    Node*       left;
    Node*       right;
};

typedef struct {
    Node*  root;
    size_t size;
} Tree;

TreeErrorType TreeCtor    (Tree* tree);
TreeErrorType TreeInsert  (Tree* tree, TreeElement value);
TreeErrorType TreeBaseDump(Tree* tree);
TreeErrorType TreeDtor    (Tree* tree);


#endif // MY_DEREVO_H_
