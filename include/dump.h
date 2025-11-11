#ifndef DUMP_FOR_TREE_H_
#define DUMP_FOR_TREE_H_

#include <stdio.h>
#include <time.h>
#include "tree_error_types.h"
#include "tree_common.h"

TreeErrorType TreeBaseDump(Tree* tree);

TreeErrorType GenerateDotFile(Tree* tree, const char* filename);
void CreateDotNodes(Tree* tree, FILE* dot_file);
void CreateTreeConnections(Node* node, FILE* dot_file);
const char* GetNodeColor(Node* node, Tree* tree);
void WriteTreeInfo(FILE* htm_file, Tree* tree);
void WriteDumpHeader(FILE* htm_file, time_t now);
void WriteDumpFooter(FILE* htm_file);
TreeErrorType TreeDumpToHtm(Tree* tree, FILE* htm_file, const char* folder_path, const char* folder_name);
TreeErrorType TreeDump(Tree* tree, const char* filename);

TreeErrorType InitTreeLog(const char* filename);
TreeErrorType CloseTreeLog(const char* filename);

TreeVerifyResult VerifyTree(Tree* tree);
const char* TreeVerifyResultToString(TreeVerifyResult result);


#endif // DUMP_FOR_TREE_H_
