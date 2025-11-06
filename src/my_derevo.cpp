#include "my_derevo.h"
#include <stdio.h>

static TreeErrorType TreeDestroyRecursive(Node* node)
{
    if (node == NULL)
        return TREE_ERROR_NO; //для рекурсии это норм

    TreeDestroyRecursive(node->left);
    TreeDestroyRecursive(node->right);

    free(node);

    return TREE_ERROR_NO;
}

static TreeErrorType PrintTreeNode(const Node* node)
{
    if (node == NULL)
    {
        printf("()");
        return TREE_ERROR_NO;
    }

    printf("(");
    if (node->left)
        PrintTreeNode(node->left);

    printf("%d", node->data);

    if (node->right)
        PrintTreeNode(node->right);
    printf(")");

    return TREE_ERROR_NO;
}

static TreeErrorType TreeInsertRecursive(Node** node_ptr, TreeElement value) //иначе будем изменять копию node_ptr
{
    if (*node_ptr == NULL)
    {
        *node_ptr = (Node*)calloc(1, sizeof(Node));
        if (*node_ptr == NULL)
            return TREE_ERROR_ALLOCATION;

        (*node_ptr)->data  = value;
        (*node_ptr)->left  = NULL;
        (*node_ptr)->right = NULL;
        return TREE_ERROR_NO;
    }

    if (value <= (*node_ptr)->data)
        return TreeInsertRecursive(&(*node_ptr)->left, value);

    else if (value > (*node_ptr)->data)
        return TreeInsertRecursive(&(*node_ptr)->right, value);

    return TREE_ERROR_NO;
}

TreeErrorType TreeCtor(Tree* tree)
{
    tree->root = NULL;
    tree->size = 0;

    return TREE_ERROR_NO;
}

TreeErrorType TreeInsert(Tree* tree, TreeElement value)
{
    if (tree == NULL)
        return TREE_ERROR_NULL_PTR;

    TreeErrorType result = TreeInsertRecursive(&tree->root, value);
    if (result == TREE_ERROR_NO)
        tree->size++;

    return result;
}

TreeErrorType TreeBaseDump(Tree* tree)
{
    if (tree == NULL)
    {
        printf("Tree: NULL\n");
        return TREE_ERROR_NULL_PTR;
    }

    printf("===TRUE DUMP===\n");
    printf("Tree size: %lu\n", tree->size);
    printf("Tree structure:\n");

    if (tree->root == NULL)
    {
        printf("EMPTY TREE");
    }
    else
    {
        PrintTreeNode(tree->root);
    }

    putchar('\n');
    return TREE_ERROR_NO;
}

TreeErrorType TreeDtor(Tree* tree)
{
    if (tree == NULL)
        return TREE_ERROR_NULL_PTR;

    TreeDestroyRecursive(tree->root);

    tree->root = NULL;
    tree->size = 0;

    return TREE_ERROR_NO;
}
