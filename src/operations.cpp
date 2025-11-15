#include "operations.h"
#include "tree_base.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include "io.h"
#include "dump.h"

TreeErrorType TreeAddQuestion(Tree* tree, Node* current_node, const char* question, const char* new_object)
{
    if (tree == NULL || current_node == NULL || question == NULL || new_object == NULL)
        return TREE_ERROR_NULL_PTR;

    TreeElement old_data = current_node->data;
    bool old_is_dynamic = current_node->is_dynamic;

    current_node->data = strdup(question);
    if (current_node->data == NULL)
        return TREE_ERROR_ALLOCATION;
    current_node->is_dynamic = true;

    current_node->left  = CreateNode(strdup(new_object), current_node, true);
    current_node->right = CreateNode(strdup(old_data),   current_node, true);

    if (current_node->left == NULL || current_node->right == NULL)
    {
        free(current_node->data);
        current_node->data = old_data;
        current_node->is_dynamic = old_is_dynamic;

        if (current_node->left)
        {
            free(current_node->left->data);
            free(current_node->left);
        }
        if (current_node->right)
        {
            free(current_node->right->data);
            free(current_node->right);
        }
        current_node->left = current_node->right = NULL;
        return TREE_ERROR_ALLOCATION;
    }

    if (old_is_dynamic && old_data != NULL)
        free(old_data);

    tree->size += 2;

    TreeDump(tree, "akinator");

    return TREE_ERROR_NO;
}

Node* FindLeafByData(Node* node, const char* data)
{
    assert(data);

    if (node == NULL)
        return NULL;

    if (IsLeaf(node))
    {
        char* node_data_lower = (char*)calloc(strlen(node->data) + 1, sizeof(char));
        char* data_lower =      (char*)calloc(strlen(data) + 1,       sizeof(char));
        if (node_data_lower == NULL || data_lower == NULL)
        {
            free(node_data_lower);
            free(data_lower);
            return NULL;
        }

        strcpy(node_data_lower, node->data);
        strcpy(data_lower, data);
        ToLowerCase(node_data_lower);
        ToLowerCase(data_lower);

        int compare_result = strcmp(node_data_lower, data_lower);

        free(node_data_lower);
        free(data_lower);

        if (compare_result == 0)
            return node;
    }

    Node* left_result = FindLeafByData(node->left, data);
    if (left_result != NULL)
        return left_result;

    Node* right_result = FindLeafByData(node->right, data);
    return right_result;
}

TreeErrorType PrintObjectPath(Tree* tree, const char* object)
{
    if (tree == NULL || object == NULL)
    {
        printf("Ошибка: дерево или объект не задан.\n");
        return TREE_ERROR_NULL_PTR;
    }

    Node* found = FindLeafByData(tree->root, object);
    if (found == NULL)
    {
        printf("Объект \"%s\" не найден в базе.\n", object);
        return TREE_ERROR_NO;
    }

    PathStep path[kMaxPathDepth] = {};
    int step_count = 0;
    Node* current = found;

    while (current->parent != NULL && step_count < kMaxPathDepth)
    {
        Node* parent = current->parent;

        if (parent->left == current)
        {
            path[step_count].question_node = parent;
            path[step_count].answer = true;
        }
        else if (parent->right == current)
        {
            path[step_count].question_node = parent;
            path[step_count].answer = false;
        }
        else
        {
            printf("Ошибка: нарушена структура дерева.\n");
            return TREE_ERROR_STRUCTURE;
        }

        step_count++;
        current = parent;
    }

    printf("\n=== ОПРЕДЕЛЕНИЕ ОБЪЕКТА \"%s\" ===\n", object);

    if (step_count == 0)
    {
        printf("Это корневой объект: %s\n", found->data);
    }
    else
    {
        printf("Определение:\n");
        for (int i = step_count - 1; i >= 0; i--)
        {
            if (path[i].answer)
                printf("%s, ", path[i].question_node->data);
            else
                printf("HE %s, ", path[i].question_node->data);
        }
        printf("\n");
        printf("Результат: %s\n", found->data);
    }
    printf("==================================\n\n");

    return TREE_ERROR_NO;
}
