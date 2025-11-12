#ifndef DUMP_FOR_TREE_H_
#define DUMP_FOR_TREE_H_

#include "dump.h"
#include "tree_base.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <assert.h>

const int kMaxEscapedDataCapacity = 512;

TreeErrorType TreeBaseDump(Tree* tree);

TreeErrorType GenerateDotFile(Tree* tree, const char* filename);
void CreateDotNodes(Tree* tree, FILE* dot_file);
void CreateTreeConnections(Node* node, FILE* dot_file);
const char* GetNodeColor(Node* node, Tree* tree);
void WriteTreeInfo(FILE* htm_file, Tree* tree, const char* buffer, size_t buffer_pos);
void WriteDumpHeader(FILE* htm_file, time_t now);
void WriteDumpFooter(FILE* htm_file);
TreeErrorType TreeDumpToHtm(Tree* tree, FILE* htm_file, const char* folder_path, const char* folder_name, const char* buffer, size_t buffer_pos);
TreeErrorType TreeDump(Tree* tree, const char* filename, const char* buffer, size_t buffer_pos);

TreeErrorType InitTreeLog(const char* filename);
TreeErrorType CloseTreeLog(const char* filename);

TreeVerifyResult VerifyTree(Tree* tree);
const char* TreeVerifyResultToString(TreeVerifyResult result);


#endif // DUMP_FOR_TREE_H_
