#include "akinator_game.h"
#include "operations.h"
#include "io.h"
#include <stdio.h>
#include <string.h>
#include "tree_base.h"

void PlayAkinator(Tree* tree)
{
    if (tree == NULL || tree->root == NULL)
    {
        printf("Дерево не инициализировано!\n");
        return;
    }

    printf("\n=== Это игра АКИНАТОР! ===\n");
    printf("Загадайте животное, растение или предмет, и я попробую угадать!\n\n");

    Node* current = tree->root;

    while (!IsLeaf(current))
    {
        bool answer = GetUserAnswer(current->data);

        if (answer)
        {
            if (current->left != NULL)
            {
                current = current->left;
            }
            else
            {
                printf("Ошибка в структуре дерева\n");
                return;
            }
        }
        else
        {
            if (current->right != NULL)
            {
                current = current->right;
            }
            else
            {
                printf("Ошибка в структуре дерева\n");
                return;
            }
        }
    }

    // дошли до листа -> это ответ
    printf("Я думаю, это: %s\n", current->data);

    bool is_correct = GetUserAnswer("Я угадал?");

    if (is_correct)
    {
        printf("Я угадал! Спасибо за игру!\n");
    }
    else
    {
        printf("Ой! Я не угадал.\n");

        char correct_answer         [kMaxInputCapacity] = {};
        char distinguishing_question[kMaxInputCapacity] = {};

        SafeInputString(correct_answer, sizeof(correct_answer), "Что ты загадал? ");

        printf("Какой вопрос отличает \"%s\" от \"%s\"? \n", correct_answer, current->data);
        printf("(Вопрос должен быть таким, чтобы для \"%s\" ответ был ДА): ", correct_answer);

        SafeInputString(distinguishing_question, sizeof(distinguishing_question), "");

        TreeErrorType result = TreeAddQuestion(tree, current, distinguishing_question,correct_answer);

        if (result == TREE_ERROR_NO)
            printf("Спасибо! Теперь я знаю больше!\n");
        else
            printf("Ошибка при добавлении вопроса!\n");
    }
}

void FindObjectDefinition(Tree* tree)
{
    if (tree == NULL || tree->root == NULL)
    {
        printf("Дерево не инициализировано!\n");
        return;
    }

    char object_name[kMaxInputCapacity] = {};
    SafeInputString(object_name, sizeof(object_name), "Введите название объекта для поиска: ");

    PrintObjectPath(tree, object_name);
}
