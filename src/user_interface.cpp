#include "user_interface.h"
#include "akinator_game.h"
#include "io.h"
#include "dump.h"
#include "operations.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

const char* GetDataBaseFilename(int argc, const char** argv)
{
    assert(argv);

    return (argc < 1) ? kDefaultDataBaseFilename : argv[1];
}

void UserInterface(UserActionChoices* choice)
{
    assert(choice);

    printf("\n Выберите действие:\n");
    printf("1. Играть в Акинатор\n");
    printf("2. Дамп дерева (базовый)\n");
    printf("3. Визуализировать дерево (графически)\n");
    printf("4. Найти определение объекта\n");
    printf("5. Сохранить дерево в файл\n");
    printf("6. Загрузить дерево из файла\n");
    printf("0. Выход\n");
    printf("Выберите действие: ");

    int int_choice = 0;

    if (scanf("%d", &int_choice) != 1)
        *choice = CHOICE_FAILED;
    ClearInputBuffer();

    switch (int_choice)
    {
        case 0: *choice = CHOICE_LEAVE;                 break;
        case 1: *choice = CHOICE_PLAY_AKINATOR;         break;
        case 2: *choice = CHOICE_BASE_DUMP;             break;
        case 3: *choice = CHOICE_HTM_DUMP;              break;
        case 4: *choice = CHOICE_FIND_OBJ_DEFINITION;   break;
        case 5: *choice = CHOICE_SAVE_TREE_TO_FILE;     break;
        case 6: *choice = CHOICE_LOAD_TREE_FROM_FILE;   break;
    }
}

void ChooseAction(Tree* tree, UserActionChoices choice, const char* filename)
{
    assert(tree);
    assert(filename);

    switch (choice)
    {
        case CHOICE_PLAY_AKINATOR:
            PlayAkinator(tree);
            break;
        case CHOICE_BASE_DUMP:
            TreeBaseDump(tree);
            break;
        case CHOICE_HTM_DUMP:
            TreeDump(tree, "akinator");
            printf("Дерево визуализировано.\n");
            break;
        case CHOICE_FIND_OBJ_DEFINITION:
            FindObjectDefinition(tree);
            break;
        case CHOICE_SAVE_TREE_TO_FILE:
            if (TreeSave(tree, filename) == TREE_ERROR_NO)
                printf("Дерево сохранено в файл '%s'\n", filename);
            else
                printf("Ошибка сохранения дерева!\n");
            break;
        case CHOICE_LOAD_TREE_FROM_FILE:
            if (TreeLoad(tree, filename) == TREE_ERROR_NO)
                printf("Дерево загружено из файла '%s'\n", filename);
            else
                printf("Ошибка загрузки дерева!\n");
            break;
        case CHOICE_LEAVE:
            printf("Выход...\n");
            TreeSave(tree, filename);
            break;
        default:
            printf("Неверный выбор!\n");
    }
}
