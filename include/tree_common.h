#ifndef TREE_COMMON_H_
#define TREE_COMMON_H_

#include <stdlib.h>

const int         kMaxSystemCommandLength             = 512;
const int         kMaxLengthOfFilename                = 256;
const char* const kGeneralFolderNameForLogs           = "tree_logs";
const int         kMaxLengthOfAnswer                  = 256;
const int         kMaxInputCapacity                   = 256;
const int         kMaxPathDepth                       = 100;
const int         kTreeDumpAfterAddingElementCapacity = 512;
const char* const kDefaultDataBaseFilename            = "akinator_tree.txt";

typedef char* TreeElement;

typedef struct Node {
    TreeElement  data;
    struct Node* left;
    struct Node* right;
    struct Node* parent;
    bool is_dynamic;
} Node;

typedef struct {
    Node* root;
    size_t size;
    char* file_buffer;
} Tree;

typedef struct {
    Node*  node;
    size_t depth;
} NodeDepthInfo;

typedef struct {
    NodeDepthInfo* items;
    size_t         size;
    size_t         capacity;
    size_t         current_depth;
} LoadProgress;


#endif //TREE_COMMON_H_
