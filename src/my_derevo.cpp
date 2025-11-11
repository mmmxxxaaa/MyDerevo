#include "my_derevo.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

void ToLowerCase(char* string)
{
    for (int i = 0; string[i] != '\0'; i++)
    {
        string[i] = (char)tolower(string[i]);
    }
}

void ClearInputBuffer()
{
    int c = '\0';
    while ((c = getchar()) != '\n' && c != EOF) { }
}

bool GetUserAnswer(const char* question)
{
    char answer[kMaxInputCapacity] = {};

    while (true)
    {
        printf("%s? (да/нет): ", question); //FIXME вопросы дюпаются
        if (fgets(answer, sizeof(answer), stdin) == NULL)
        {
            ClearInputBuffer();
            continue;
        }

        size_t len = strlen(answer);
        if (len > 0 && answer[len-1] != '\n')
        {
            ClearInputBuffer();
            printf("Слишком длинный ввод. Пожалуйста, ответьте короче.\n");
            continue;
        }

        if (len > 0 && answer[len-1] == '\n')
            answer[len-1] = '\0';

        ToLowerCase(answer);

        if (strcmp(answer, "да") == 0 || strcmp(answer, "д") == 0 || strcmp(answer, "yes") == 0 || strcmp(answer, "y") == 0)
            return true;
        else if (strcmp(answer, "нет") == 0 || strcmp(answer, "н") == 0 || strcmp(answer, "no") == 0 || strcmp(answer, "n") == 0)
            return false;
        else
            printf("Пожалуйста, ответьте 'да' или 'нет'.\n");
    }
}

bool IsLeaf(Node* node)
{
    return node != NULL && node->left == NULL && node->right == NULL;
}

static TreeErrorType TreeDestroyRecursive(Node* node)
{
    if (node == NULL)
        return TREE_ERROR_NO; //для рекурсии это норм

    TreeDestroyRecursive(node->left);
    TreeDestroyRecursive(node->right);

    if (node->data)
        free(node->data);
    free(node);

    return TREE_ERROR_NO;
}


//это для работы с числами
// static TreeErrorType TreeInsertRecursive(Node** node_ptr, Node* parent, TreeElement value) //иначе будем изменять копию node_ptr
// {
//     if (*node_ptr == NULL)
//     {
//         *node_ptr = (Node*)calloc(1, sizeof(Node));
//         if (*node_ptr == NULL)
//             return TREE_ERROR_ALLOCATION;
//
//         (*node_ptr)->data = value ? strdup(value) : NULL;
//         if (value != NULL && (*node_ptr)->data == NULL)
//         {
//             // free(*node_ptr); //это делает деструктор
//             return TREE_ERROR_ALLOCATION;
//         }
//
//         (*node_ptr)->left   = NULL;
//         (*node_ptr)->right  = NULL;
//         (*node_ptr)->parent = parent;
//         return TREE_ERROR_NO;
//     }
//
//     return TreeInsertRecursive(&(*node_ptr)->left,  *node_ptr, value);
// }

TreeErrorType TreeCtor(Tree* tree)
{
    tree->root = NULL;
    tree->size = 0;

    return TreeInsert(tree, "неизвестно что");
}

static Node* CreateNode(const char* value, Node* parent)
{
    Node* node = (Node*)calloc(1, sizeof(Node));
    if (node == NULL)
        return NULL;

    node->data = value ? strdup(value) : NULL;
    if (value != NULL && node->data == NULL)
    {
        free(node);
        return NULL;
    }

    node->left = NULL;
    node->right = NULL;
    node->parent = parent;

    return node;
}

TreeErrorType TreeInsert(Tree* tree, const char* value)
{
    if (tree == NULL)
        return TREE_ERROR_NULL_PTR;

    if (tree->root == NULL)
    {
        tree->root = CreateNode(value, NULL);
        if (tree->root == NULL)
            return TREE_ERROR_ALLOCATION;

        tree->size++;
        return TREE_ERROR_NO;
    }

    Node* current = tree->root;
    while (current->left != NULL)
        current = current->left;

    current->left = CreateNode(value, current);
    if (current->left == NULL)
        return TREE_ERROR_ALLOCATION;

    tree->size++;
    return TREE_ERROR_NO;
}

TreeErrorType TreeAddQuestion(Tree* tree, Node* leaf, const char* question,
                              const char* yes_answer, const char* no_answer)
{
    if (tree == NULL || leaf == NULL || question == NULL || yes_answer == NULL || no_answer == NULL)
        return TREE_ERROR_NULL_PTR;

    char* old_answer = leaf->data;
    char* question_copy = strdup(question);
    if (question_copy == NULL)
        return TREE_ERROR_ALLOCATION;

    // создаем левый узел (да) [сюда новый объект]
    leaf->left = CreateNode(yes_answer, leaf);
    if (leaf->left == NULL)
    {
        free(question_copy);
        return TREE_ERROR_ALLOCATION;
    }

    // создаем правый узел (да) [сюда старый объект]
    leaf->right = CreateNode(no_answer, leaf);
    if (leaf->right == NULL)
    {
        free(question_copy);
        free(leaf->left->data);
        free(leaf->left);
        leaf->left = NULL;
        return TREE_ERROR_ALLOCATION;
    }
    //теперь наш бывший лист это признак, копируем в него вопрос
    leaf->data = question_copy;

    free(old_answer);

    tree->size += 2;

    return TREE_ERROR_NO;
}

void SafeInputString(char* buffer, size_t buffer_size, const char* prompt)
{
    while (true)
    {
        if (prompt)
            printf("%s", prompt);

        buffer[0] = '\0';

        if (fgets(buffer, buffer_size, stdin) == NULL)
        {
            ClearInputBuffer();
            printf("Ошибка ввода. Попробуйте снова.\n");
            continue;
        }

        size_t len = strlen(buffer);
        if (len > 0 && buffer[len-1] != '\n')
        {
            ClearInputBuffer();
            printf("Ввод слишком длинный. Пожалуйста, введите снова (макс. %zu символов).\n", buffer_size - 1);
            continue;
        }

        if (len > 0 && buffer[len-1] == '\n')
            buffer[len-1] = '\0';

        if (buffer[0] == '\0')
        {
            printf("Пустой ввод не допускается. Пожалуйста, введите текст.\n");
            continue;
        }

        break;
    }
}

void PlayAkinator(Tree* tree)
{
    if (tree == NULL || tree->root == NULL)
    {
        printf("Дерево не инициализировано!\n");
        return;
    }

    printf("\n=== Это игра АКИНАТОР! ===\n");
    printf("Загадайте животное, растение или предмет, и я попробую угадать!\n\n");

    Node* current = tree->root; //начинаем с корня

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

        char correct_answer[kMaxInputCapacity]          = {};
        char distinguishing_question[kMaxInputCapacity] = {};

        SafeInputString(correct_answer, sizeof(correct_answer), "Что ты загадал? ");

        printf("Какой вопрос отличает \"%s\" от \"%s\"? \n", correct_answer, current->data);
        printf("(Вопрос должен быть таким, чтобы для \"%s\" ответ был ДА): ", correct_answer);

        SafeInputString(distinguishing_question, sizeof(distinguishing_question), "");
        // новый объект на ДА, старый на НЕТ
        TreeErrorType result = TreeAddQuestion(tree, current, distinguishing_question,
                                               correct_answer, current->data);

        if (result == TREE_ERROR_NO)
            printf("Спасибо! Теперь я знаю больше!\n");
        else
            printf("Ошибка при добавлении вопроса!\n");
    }
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

//FIXME переделать с поиск листа с рекурсии на цикл
static Node* FindLeafByData(Node* node, const char* data, bool case_sensitive)
{
    if (node == NULL)
        return NULL;

    if (IsLeaf(node))
    {
        int compare_result = 0;
        if (case_sensitive)
        {
            compare_result = strcmp(node->data, data);
        }
        else
        {
            char* node_data_lower = strdup(node->data);
            char* data_lower = strdup(data);
            if (node_data_lower == NULL || data_lower == NULL)
            {
                free(node_data_lower);
                free(data_lower);
                return NULL;
            }

            ToLowerCase(node_data_lower);
            ToLowerCase(data_lower);
            compare_result = strcmp(node_data_lower, data_lower);

            free(node_data_lower);
            free(data_lower);
        }

        if (compare_result == 0)
            return node;
    }

    // рекурсивно ищем в левом и правом поддеревьях
    Node* left_result = FindLeafByData(node->left, data, case_sensitive);
    if (left_result != NULL)
        return left_result;

    Node* right_result = FindLeafByData(node->right, data, case_sensitive);
    return right_result;
}

// Функция для вывода пути к объекту
TreeErrorType PrintObjectPath(Tree* tree, const char* object)
{
    if (tree == NULL || object == NULL)
    {
        printf("Ошибка: дерево или объект не задан.\n");
        return TREE_ERROR_NULL_PTR;
    }

    Node* found = FindLeafByData(tree->root, object, false);
    if (found == NULL)
    {
        printf("Объект \"%s\" не найден в базе.\n", object);
        return TREE_ERROR_NO;
    }

    PathStep path[kMaxPathDepth] = {};
    int step_count = 0;
    Node* current = found;

    //поднимаемся от листа к корню
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

    // выводим путь в правильном порядке
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

// поиск и вывод пути элемента (интерфейс для пользователя)
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
