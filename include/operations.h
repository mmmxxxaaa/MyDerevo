#ifndef TREE_OPERATIONS_H_
#define TREE_OPERATIONS_H_

#include "tree_common.h"
#include "tree_error_types.h"

typedef struct {
    Node* question_node;
    bool answer; // true да, false нет
} PathStep;

TreeErrorType TreeInsert     (Tree* tree, const char* value);
TreeErrorType TreeAddQuestion(Tree* tree, Node* leaf, const char* question, const char* yes_answer, const char* no_answer);
TreeErrorType PrintObjectPath(Tree* tree, const char* object);
Node*         FindLeafByData (Node* node, const char* data, bool case_sensitive);


#endif // TREE_OPERATIONS_H_
