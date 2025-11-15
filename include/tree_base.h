#ifndef TREE_BASE_H_
#define TREE_BASE_H_

#include "tree_common.h"
#include "tree_error_types.h"

TreeErrorType TreeCtor(Tree* tree);
TreeErrorType TreeDtor(Tree* tree);
Node* CreateNode(TreeElement value, Node* parent, bool is_dynamic);
bool IsLeaf(Node* node);
TreeErrorType TreeDestroyWithDataRecursive(Node* node);

#endif // TREE_BASE_H_
