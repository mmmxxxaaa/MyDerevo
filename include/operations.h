#ifndef TREE_OPERATIONS_H_
#define TREE_OPERATIONS_H_

#include "tree_common.h"
#include "tree_error_types.h"

typedef struct {
    Node* question_node;
    bool answer; // true да, false нет
} PathStep;

TreeErrorType TreeInitWithFirstQuestion(Tree* tree, const char* question, const char* first_object, const char* default_object);
TreeErrorType TreeAddQuestion(Tree* tree, Node* current_node, const char* question, const char* new_object);
TreeErrorType PrintObjectPath(Tree* tree, const char* object);
Node*         FindLeafByData (Node* node, const char* data);


#endif // TREE_OPERATIONS_H_
