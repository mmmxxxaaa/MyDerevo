#ifndef MY_DEREVO_H_
#define MY_DEREVO_H_

#include "tree_error_types.h"
#include <stdlib.h>
#include <stdio.h>

typedef int TreeElement;

typedef struct Node Node;

struct Node {
    TreeElement data;
    Node*       left;
    Node*       right;
    Node*       parent;
};

typedef struct {
    Node*  root;
    size_t size;
} Tree;

TreeErrorType TreeCtor        (Tree* tree);
TreeErrorType TreeInsert      (Tree* tree, TreeElement value);
TreeErrorType TreeBaseDump    (Tree* tree);
TreeErrorType TreeDtor        (Tree* tree);


TreeErrorType GenerateDotFile(Tree* tree, const char* filename);
void CreateDotNodes(Tree* tree, FILE* dot_file);
void CreateTreeConnections(Node* node, FILE* dot_file);
const char* GetNodeColor(Node* node, Tree* tree);
void WriteTreeInfo(FILE* htm_file, Tree* tree);
void WriteDumpHeader(FILE* htm_file, time_t now);
void WriteDumpFooter(FILE* htm_file);

TreeErrorType TreeDumpToHtm(Tree* tree, FILE* htm_file, const char* folder_path, const char* folder_name);

TreeErrorType InitTreeLog(const char* filename);
TreeErrorType CloseTreeLog(const char* filename);

TreeErrorType    TreeDump  (Tree* tree, const char* filename);
TreeVerifyResult VerifyTree(Tree* tree);

const char* TreeVerifyResultToString(TreeVerifyResult result);

const int         kMaxSystemCommandLength   = 512;
const int         kMaxLengthOfFilename      = 256;
const char* const kGeneralFolderNameForLogs = "tree_logs";

#endif // MY_DEREVO_H_
