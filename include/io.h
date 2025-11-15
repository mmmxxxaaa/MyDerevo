#ifndef IO_H
#define IO_H

#include "tree_base.h"
#include <stdbool.h>
#include <stdio.h>

void InitLoadProgress(LoadProgress* progress);
void AddNodeToLoadProgress(LoadProgress* progress, Node* node, size_t depth);
void FreeLoadProgress(LoadProgress* progress);

size_t GetFileSize(FILE* file);
TreeErrorType TreeLoad(Tree* tree, const char* filename);
TreeErrorType TreeSave(const Tree* tree, const char* filename);

void SafeInputString(char* buffer, size_t buffer_size, const char* prompt);
void ToLowerCase(char* string);
void ClearInputBuffer();
bool GetUserAnswer(const char* question);

#endif
