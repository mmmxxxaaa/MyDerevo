#ifndef TREE_COMMON_H_
#define TREE_COMMON_H_

#include <stdlib.h>

const int         kMaxSystemCommandLength   = 512;
const int         kMaxLengthOfFilename      = 256;
const char* const kGeneralFolderNameForLogs = "tree_logs";
const int         kMaxLengthOfAnswer        = 256;
const int         kMaxInputCapacity         = 256;
const int         kMaxPathDepth             = 100;

typedef const char* TreeElement;

typedef struct Node {
    TreeElement data;
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


#endif //TREE_COMMON_H_
