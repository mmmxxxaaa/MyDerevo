#include <stdio.h>
#include <stdlib.h>
#include "my_derevo.h"
#include "tree_error_types.h"

int main()
{
    Tree tree = {};
    InitTreeLog("my_tree");
    TreeErrorType error = TREE_ERROR_NO;

    error = TreeCtor(&tree);
    if (error != TREE_ERROR_NO)
    {
        printf("Ошибка создания дерева: %d\n", error);
        return 1;
    }
//случайными числами отсортировать дерево переписать в новый массив
    int values[] = {50, 30, 70, 20, 40, 60, 80, 10, 35, 65, 90, 5, 95};
    int count = sizeof(values) / sizeof(values[0]);

    for (int i = 0; i < count; i++)
    {
        error = TreeInsert(&tree, values[i]);
        if (error != TREE_ERROR_NO)
            printf("Ошибка: %d\n", error);
    }

    TreeBaseDump(&tree);
    TreeDump(&tree, "my_tree");

    (tree.root->left)->parent = NULL;

    TreeDump(&tree, "my_tree");

    CloseTreeLog("my_tree");
    TreeDtor(&tree);

    return 0;
}




