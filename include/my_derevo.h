#ifndef MY_DEREVO_H_
#define MY_DEREVO_H_

#include <stdlib.h>
#include <stdio.h>
#include "tree_error_types.h"
#include "tree_common.h"

typedef struct {
    Node* question_node;
    bool answer; // true да, false нет
} PathStep;

TreeErrorType TreeCtor    (Tree* tree);
TreeErrorType TreeInsert  (Tree* tree, const char* value);
TreeErrorType TreeDtor    (Tree* tree);

void ToLowerCase(char* string);
bool GetUserAnswer(const char* question);
TreeErrorType TreeAddQuestion(Tree* tree, Node* leaf, const char* question,
                              const char* yes_answer, const char* no_answer);
void SafeInputString(char* buffer, size_t buffer_size, const char* prompt);

void PlayAkinator(Tree* tree);

void ClearInputBuffer();

bool IsLeaf(Node* node);

TreeErrorType PrintObjectPath(Tree* tree, const char* object);
void FindObjectDefinition(Tree* tree);


#endif // MY_DEREVO_H_
