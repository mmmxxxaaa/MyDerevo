#include "akinator_game.h"
#include "io.h"
#include "dump.h"
#include "operations.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    Tree tree;
    TreeCtor(&tree);

    InitTreeLog("akinator");
    InitTreeLog("akinator_parse");

    printf("=== АКИНАТОР ===\n");

    TreeErrorType load_result = TreeLoad(&tree, "akinator_tree.txt");
    if (load_result != TREE_ERROR_NO)
    {
        printf("He удалось загрузить дерево из файла. Создаем новое дерево.\n");
        Node* current = tree.root;
        TreeAddQuestion(&tree, current, "Это животное?", "кошка", "неизвестно что");
    }
    else
    {
        printf("Дерево успешно загружено из файла.\n");
    }

    int choice = 0;
    do {
        printf("\n Выберите действие:\n");
        printf("1. Играть в Акинатор\n");
        printf("2. Дамп дерева (базовый)\n");
        printf("3. Визуализировать дерево (графически)\n");
        printf("4. Найти определение объекта\n");
        printf("5. Сохранить дерево в файл\n");
        printf("6. Загрузить дерево из файла\n");
        printf("0. Выход\n");
        printf("Выберите действие: ");

        if (scanf("%d", &choice) != 1)
        {
            while (getchar() != '\n')
                continue;
            choice = -1;
        }
        while (getchar() != '\n');

        switch (choice)
        {
            case 1:
                PlayAkinator(&tree);
                break;
            case 2:
                TreeBaseDump(&tree);
                break;
            case 3:
                TreeDump(&tree, "akinator", NULL, 0);
                printf("Дерево визуализировано.\n");
                break;
            case 4:
                FindObjectDefinition(&tree);
                break;
            case 5:
                if (TreeSave(&tree, "akinator_tree.txt") == TREE_ERROR_NO)
                    printf("Дерево сохранено в файл 'akinator_tree.txt'\n");
                else
                    printf("Ошибка сохранения дерева!\n");
                break;
            case 6:
                if (TreeLoad(&tree, "akinator_tree.txt") == TREE_ERROR_NO)
                {
                    printf("Дерево загружено из файла 'akinator_tree.txt'\n");
                }
                else
                {
                    printf("Ошибка загрузки дерева!\n");
                }
                break;
            case 0:
                printf("Выход...\n");
                TreeSave(&tree, "akinator_tree.txt");
                break;
            default:
                printf("Неверный выбор!\n");
        }
    } while (choice != 0);

    CloseTreeLog("akinator");
    CloseTreeLog("akinator_parse");

    TreeDtor(&tree);
    return 0;
}
