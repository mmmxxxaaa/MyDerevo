#ifndef TREE_COMMON_H_
#define TREE_COMMON_H_

const int         kMaxSystemCommandLength   = 512;
const int         kMaxLengthOfFilename      = 256;
const char* const kGeneralFolderNameForLogs = "tree_logs";
const int         kMaxLengthOfAnswer        = 256;
const int         kMaxInputCapacity = 256;
const int         kMaxPathDepth     = 100;

typedef char* TreeElement;

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


#endif //TREE_COMMON_H_
