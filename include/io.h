#ifndef TREE_IO_H_
#define TREE_IO_H_

#include "tree_common.h"
#include "tree_error_types.h"

TreeErrorType TreeLoad(Tree* tree, const char* filename);
TreeErrorType TreeSave(const Tree* tree, const char* filename);

void SafeInputString(char* buffer, size_t buffer_size, const char* prompt);
void ToLowerCase(char* string);
void ClearInputBuffer();
bool GetUserAnswer(const char* question);

#endif //TREE_IO_H_

