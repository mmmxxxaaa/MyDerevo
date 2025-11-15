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
    progress->items         = (NodeDepthInfo*)calloc(progress->capacity, sizeof(NodeDepthInfo));
}

void AddNodeToLoadProgress(LoadProgress* progress, Node* node, size_t depth)
{
    if (progress->size >= progress->capacity)
    {
        progress->capacity *= 2;
        progress->items = (NodeDepthInfo*)realloc(progress->items, progress->capacity * sizeof(NodeDepthInfo));
    }
    progress->items[progress->size].node  = node;
    progress->items[progress->size].depth = depth;
    progress->size++;
}

void FreeLoadProgress(LoadProgress* progress)
{
    free(progress->items);

    progress->items         = NULL;
    progress->capacity      = 0;
    progress->size          = 0;
    progress->current_depth = 0;
}

static void SkipSpaces(const char* buffer, size_t* pos)
{
    while (isspace(buffer[*pos]))
        (*pos)++;
}

static char* ReadQuotedString(char* buffer, size_t* pos)
{
    SkipSpaces(buffer, pos);

    if (buffer[*pos] != '"')
        return NULL;
    (*pos)++;

    char* start = buffer + *pos;

    while (buffer[*pos] != '\0' && buffer[*pos] != '"')
        (*pos)++;

    if (buffer[*pos] != '"')
        return NULL;

    buffer[*pos] = '\0';
    (*pos)++;

    return start;
}

static Node* ReadNodeFromBuffer(Tree* tree, char* buffer, size_t buffer_length, size_t* pos, Node* parent,
                                int depth, LoadProgress* progress)
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

    char* str_ptr = ReadQuotedString(buffer, pos);
    if (str_ptr == NULL)
        return NULL;

    Node* node = CreateNode(str_ptr, parent, false);
    if (node == NULL)
        return NULL;

    if (tree != NULL)
        tree->size++;

    if (progress != NULL)
    {
        AddNodeToLoadProgress(progress, node, depth);
        TreeLoadDump(tree, "akinator_parse", buffer, buffer_length, *pos, progress, "Node created");
    }

    SkipSpaces(buffer, pos);
    node->left = ReadNodeFromBuffer(tree, buffer, buffer_length, pos, node, depth + 1, progress);

    SkipSpaces(buffer, pos);
    node->right = ReadNodeFromBuffer(tree, buffer, buffer_length, pos, node, depth + 1, progress);
//FIXME побольше дампов, чтобы прям как в тетради
    SkipSpaces(buffer, pos);
    if (buffer[*pos] != ')')
    {
        TreeDestroyWithDataRecursive(node);
        return NULL;
    }
    (*pos)++;

    if (progress != NULL)
        TreeLoadDump(tree, "akinator_parse", buffer, buffer_length, *pos, progress, "Subtree complete");

    return node;
}

size_t GetFileSize(FILE* file)
{
    fseek(file, 0, SEEK_END);
    long file_size_long = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (file_size_long <= 0)
    {
        fclose(file);
        return 0;
    }

    return (size_t)file_size_long;
}

TreeErrorType TreeLoad(Tree* tree, const char* filename)
{
    if (tree == NULL || filename == NULL)
        return TREE_ERROR_NULL_PTR;

    FILE* file = fopen(filename, "r");
    if (file == NULL)
        return TREE_ERROR_IO;

    size_t file_size = GetFileSize(file);

    char* file_buffer = (char*)calloc(file_size + 1, sizeof(char));
    if (file_buffer == NULL)
    {
        fclose(file);
        return TREE_ERROR_ALLOCATION;
    }

    size_t bytes_read = fread(file_buffer, 1, file_size, file);
    file_buffer[bytes_read] = '\0';
    fclose(file);

    LoadProgress progress = {};
    InitLoadProgress(&progress);

    size_t pos = 0;
    size_t buffer_length = strlen(file_buffer);
    tree->root = ReadNodeFromBuffer(tree, file_buffer, buffer_length, &pos, NULL, 0, &progress);

    if (tree->root == NULL)
    {
        free(file_buffer);
        FreeLoadProgress(&progress);
        return TREE_ERROR_FORMAT;
    }

    tree->file_buffer = file_buffer;

    TreeDump(tree, "akinator"); //финальный дамп после загрузки дерева

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

    fprintf(file, "( \"");
    const char* str = node->data;
    while (*str != '\0')
    {
        fputc(*str, file);
        str++;
    }
    fprintf(file, "\" \n");

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
