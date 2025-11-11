#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "my_derevo.h"
#include "tree_error_types.h"
#include "dump.h"

int main()
{
    Tree tree;
    TreeCtor(&tree);
    InitTreeLog("akinator");

    printf("=== АКИНАТОР ===\n");

    Node* current = tree.root;
    TreeAddQuestion(&tree, current, "Это животное?", "кошка", "неизвестно что");
    current = current->right;
    TreeAddQuestion(&tree, current, "Оно съедобное?", "торт", "неизвестно что");
    current = current->right;
    TreeAddQuestion(&tree, current, "Это посуда?", "ложка", "неизвестно что");
    TreeDump(&tree, "akinator");
    int choice = 0;
    do {
        printf("\n Выберите действие:\n");
        printf("1. Играть в Акинатор\n");
        printf("2. Дамп дерева (базовый)\n");
        printf("3. Визуализировать дерево (графически)\n");
        printf("4. Найти определение объекта\n");
        printf("0. Выход\n");
        printf("Выберите действие: ");

        if (scanf("%d", &choice) != 1)
        {
            while (getchar() != '\n')
                continue;
        }
        while (getchar() != '\n');

        switch (choice)
        {
            case 1:
                while (true)
                {
                    PlayAkinator(&tree);
                    TreeDump(&tree, "akinator");
                    printf("\n Играем еще раз? (да/нет): ");

                    char play_again[kMaxInputCapacity] = {};
                    if (fgets(play_again, sizeof(play_again), stdin) == NULL)
                        break;

                    ToLowerCase(play_again);
                    if (strcmp(play_again, "да") != 0 && strcmp(play_again, "д") != 0 && strcmp(play_again, "yes") != 0 && strcmp(play_again, "y") != 0)
                        break;

                    printf("\n");
                }
                break;
            case 2:
                TreeBaseDump(&tree);
                break;
            case 3:
                TreeDump(&tree, "akinator");
                printf("Дерево визуализировано.\n");
                break;
            case 4:
                FindObjectDefinition(&tree);
                break;
            case 0:
                printf("Выход...\n");
                break;
            default:
                printf("Неверный выбор!\n");
        }
    } while (choice != 0);

    CloseTreeLog("akinator");
    TreeDtor(&tree);
    return 0;
}





