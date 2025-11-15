#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "user_interface.h"
#include "io.h"
#include "dump.h"
#include "operations.h"

int main(int argc, const char** argv)
{
    const char* filename = GetDataBaseFilename(argc, argv);

    Tree tree = {};
    TreeCtor(&tree);

    InitTreeLog("akinator");
    InitTreeLog("akinator_parse");

    printf("=== АКИНАТОР ===\n");

    // TreeErrorType load_result = TreeLoad(&tree, filename);
    TreeErrorType load_result = TREE_ERROR_NULL_PTR;

    if (load_result != TREE_ERROR_NO)
    {
        printf("He удалось загрузить дерево из файла. Создаем новое дерево.\n");
        TreeInitWithFirstQuestion(&tree, "Это животное?", "кошка", "неизвестно что");
    }
    else
    {
        printf("Дерево успешно загружено из файла.\n");
    }

    UserActionChoices choice = CHOICE_LEAVE;
    do {
        UserInterface(&choice);
        ChooseAction(&tree, choice, filename);
    } while (choice != CHOICE_LEAVE);

    CloseTreeLog("akinator");
    CloseTreeLog("akinator_parse");

    TreeDtor(&tree);
    return 0;
}
