#include "io.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "dump.h"
#include "tree_base.h"

void InitLoadProgress(LoadProgress* progress)
{
    progress->capacity      = 10;
    progress->size          = 0;
    progress->current_depth = 0;
    progress->nodes         = (Node**) calloc(progress->capacity, sizeof(Node*));
    progress->depths        = (size_t*)calloc(progress->capacity, sizeof(size_t));
}

void AddNodeToLoadProgress(LoadProgress* progress, Node* node, size_t depth)
{
    if (progress->size >= progress->capacity)
    {
        progress->capacity *= 2;
        progress->nodes     = (Node**) realloc(progress->nodes,  progress->capacity * sizeof(Node*));
        progress->depths    = (size_t*)realloc(progress->depths, progress->capacity * sizeof(size_t));
    }
    progress->nodes[progress->size]  = node;
    progress->depths[progress->size] = depth;
    progress->size++;
}

void FreeLoadProgress(LoadProgress* progress)
{
    free(progress->nodes);
    free(progress->depths);

    progress->nodes         = NULL;
    progress->depths        = NULL;
    progress->capacity      = 0;
    progress->size          = 0;
    progress->current_depth = 0;
}

static void SkipSpaces(const char* buffer, size_t* pos) //FIXME возвращать pos, а не через указатель изменять
{
    while (isspace(buffer[*pos]))
        (*pos)++;
}

static const char* ReadQuotedString(const char* buffer, size_t* pos, size_t* len_out)
{
    SkipSpaces(buffer, pos);

    if (buffer[*pos] != '"')
        return NULL;
    (*pos)++;

    size_t start = *pos;
    while (buffer[*pos] != '\0' && buffer[*pos] != '"')
        (*pos)++;

    if (buffer[*pos] != '"')
        return NULL;

    *len_out = *pos - start;
    (*pos)++;

    return buffer + start;
}

static Node* ReadNodeFromBuffer(Tree* tree, const char* buffer, size_t* pos, Node* parent,
                                char* string_pool, size_t* pool_pos, int depth, LoadProgress* progress)
{
    SkipSpaces(buffer, pos);

    if (buffer[*pos] == '\0')
        return NULL;

    if (strncmp(buffer + *pos, "nil", sizeof("nil") - 1) == 0)
    {
        *pos += sizeof("nil") - 1;
        return NULL;
    }

    if (buffer[*pos] != '(')
        return NULL;

    (*pos)++;
    SkipSpaces(buffer, pos);

    size_t str_len = 0;
    const char* str_ptr = ReadQuotedString(buffer, pos, &str_len);
    if (str_ptr == NULL)
        return NULL;

    char* pooled_string = string_pool + *pool_pos;
    strncpy(pooled_string, str_ptr, str_len);
    pooled_string[str_len] = '\0';
    *pool_pos += str_len + 1;

    Node* node = CreateNode(pooled_string, parent, false);
    if (node == NULL)
        return NULL;

    if (tree != NULL)
        tree->size++;

    if (progress != NULL)
    {
        AddNodeToLoadProgress(progress, node, depth);
        TreeLoadDump(tree, "akinator_parse", buffer, *pos, progress, "Node created");
    }

    SkipSpaces(buffer, pos);
    node->left = ReadNodeFromBuffer(tree, buffer, pos, node, string_pool, pool_pos, depth + 1, progress);

    SkipSpaces(buffer, pos);
    node->right = ReadNodeFromBuffer(tree, buffer, pos, node, string_pool, pool_pos, depth + 1, progress);

    SkipSpaces(buffer, pos);
    if (buffer[*pos] != ')')
    {
        TreeDestroyWithDataRecursive(node);
        return NULL;
    }
    (*pos)++;

    if (progress != NULL)
        TreeLoadDump(tree, "akinator_parse", buffer, *pos, progress, "Subtree complete");

    return node;
}

TreeErrorType TreeLoad(Tree* tree, const char* filename)
{
    if (tree == NULL || filename == NULL)
        return TREE_ERROR_NULL_PTR;

    FILE* file = fopen(filename, "r");
    if (file == NULL)
        return TREE_ERROR_IO;

    fseek(file, 0, SEEK_END); // FIXME в функциб
    long file_size_long = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (file_size_long <= 0)
    {
        fclose(file);
        return TREE_ERROR_IO;
    }

    size_t file_size = (size_t)file_size_long;
    size_t total_pool_size = file_size * 2 + 1; //ХУЙНЯ ДЕРЬМО
//СИГМА СКИБИДИ вместо printfа использовать цикл по размеру и печатать через fputc
    char* buffer_and_pool = (char*)calloc(total_pool_size, sizeof(char));
    if (buffer_and_pool == NULL)
    {
        fclose(file);
        return TREE_ERROR_ALLOCATION;
    }

    char* file_buffer = buffer_and_pool;
    size_t bytes_read = fread(file_buffer, 1, file_size, file);
    file_buffer[bytes_read] = '\0';
    fclose(file);

    char* string_pool = buffer_and_pool + file_size + 1;
    size_t pool_pos = 0;

    //с началом процесса загрузки начинаем отслеживать ступени создания дерева
    LoadProgress progress = {};
    InitLoadProgress(&progress);

    size_t pos = 0;
    tree->root = ReadNodeFromBuffer(tree, file_buffer, &pos, NULL, string_pool, &pool_pos, 0, &progress);

    if (tree->root == NULL)
    {
        free(buffer_and_pool);
        FreeLoadProgress(&progress);
        return TREE_ERROR_FORMAT;
    }

    tree->file_buffer = buffer_and_pool;

    TreeDump(tree, "akinator"); //финальный дамп после загрузки базы данных

    FreeLoadProgress(&progress);

    return TREE_ERROR_NO;
}

static void WriteNodeToFile(FILE* file, const Node* node, int depth, bool should_print_tabs)
{
    if (should_print_tabs)
    {
        for (int i = 0; i < depth; i++)
            fprintf(file, "    ");
    }

    if (node == NULL)
    {
        fprintf(file, "nil ");
        if (!should_print_tabs)
            fprintf(file, "\n");
        return;
    }

    fprintf(file, "( \"%s\" \n", node->data);
    WriteNodeToFile(file, node->left, depth + 1, should_print_tabs);
    if (node->left == NULL)
        should_print_tabs = false;
    WriteNodeToFile(file, node->right, depth + 1, should_print_tabs);
    for (int i = 0; i < depth; i++)
        fprintf(file, "    ");
    fprintf(file, ")\n");
}

TreeErrorType TreeSave(const Tree* tree, const char* filename)
{
    if (tree == NULL || filename == NULL)
        return TREE_ERROR_NULL_PTR;

    FILE* file = fopen(filename, "w");
    if (file == NULL)
        return TREE_ERROR_IO;

    int starting_depth = 0;
    WriteNodeToFile(file, tree->root, starting_depth, true);
    fprintf(file, "\n");
    fclose(file);

    return TREE_ERROR_NO;
}

void SafeInputString(char* buffer, size_t buffer_size, const char* prompt)
{
    while (true)
    {
        if (prompt)
            printf("%s", prompt);

        buffer[0] = '\0';

        if (fgets(buffer, (int)buffer_size, stdin) == NULL)
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

void ToLowerCase(char* string)
{
    for (int i = 0; string[i] != '\0'; i++)
        string[i] = (char)tolower(string[i]);
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
        printf("%s? (да/нет): ", question);
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

        if (
            strcmp(answer, "да") == 0
            || strcmp(answer, "д") == 0
            || strcmp(answer, "yes") == 0
            || strcmp(answer, "y") == 0
        )
            return true;
        else if (
            strcmp(answer, "нет") == 0
            || strcmp(answer, "н") == 0
            || strcmp(answer, "no") == 0
            || strcmp(answer, "n") == 0
        )
            return false;
        else
            printf("Пожалуйста, ответьте 'да' или 'нет'.\n");
    }
}
