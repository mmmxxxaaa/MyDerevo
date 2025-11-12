#include "tree_base.h"
#include <stdlib.h>
#include <string.h>

TreeErrorType TreeCtor(Tree* tree)
{
    if (tree == NULL)
        return TREE_ERROR_NULL_PTR;

    tree->root = NULL;
    tree->size = 0;
    tree->file_buffer = NULL;

    return TREE_ERROR_NO;
}

TreeErrorType TreeDtor(Tree* tree)
{
    if (tree == NULL)
        return TREE_ERROR_NULL_PTR;

    TreeDestroyWithDataRecursive(tree->root);

    if (tree->file_buffer != NULL)
    {
        free(tree->file_buffer);
        tree->file_buffer = NULL;
    }

    tree->root = NULL;
    tree->size = 0;

    return TREE_ERROR_NO;
}

bool IsLeaf(Node* node)
{
    if (node == NULL)
        return false;

    return (node->left == NULL && node->right == NULL);
}

Node* CreateNode(TreeElement value, Node* parent, bool is_dynamic)
{
    Node* node = (Node*)calloc(1, sizeof(Node));
    if (node == NULL)
        return NULL;

    node->data = value;
    node->left = NULL;
    node->right = NULL;
    node->parent = parent;
    node->is_dynamic = is_dynamic;

    return node;
}

TreeErrorType TreeDestroyWithDataRecursive(Node* node)
{
    if (node == NULL)
        return TREE_ERROR_NO;

    TreeDestroyWithDataRecursive(node->left);
    TreeDestroyWithDataRecursive(node->right);

    if (node->is_dynamic && node->data != NULL)
        free((void*)node->data);  // FIXME какая-то хуйня но пока так

    free(node);
    return TREE_ERROR_NO;
}
