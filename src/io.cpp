#include "io.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "dump.h"
#include "tree_base.h"

static void SkipSpaces(const char* buffer, size_t* pos)
{
    while (buffer[*pos] == ' ' || buffer[*pos] == '\t' || buffer[*pos] == '\n' || buffer[*pos] == '\r')
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

static Node* ReadNodeFromBuffer(Tree* tree, const char* buffer, size_t* pos, Node* parent, char* string_pool, size_t* pool_pos, int depth)
{
    char depth_info[64]; //FIXME в конст потом
    snprintf(depth_info, sizeof(depth_info), "Глубина: %d", depth);
    TreeDump(tree, "akinator_parse", buffer, *pos);

    SkipSpaces(buffer, pos);

    if (buffer[*pos] == '\0')
        return NULL;

    if (strncmp(buffer + *pos, "nil", 3) == 0)
    {
        *pos += 3;
        TreeDump(tree, "akinator_parse", buffer, *pos);
        return NULL;
    }

    if (buffer[*pos] != '(')
        return NULL;

    (*pos)++;
    TreeDump(tree, "akinator_parse", buffer, *pos);

    SkipSpaces(buffer, pos);

    size_t str_len = 0;
    const char* str_ptr = ReadQuotedString(buffer, pos, &str_len);
    if (str_ptr == NULL)
        return NULL;

    TreeDump(tree, "akinator_parse", buffer, *pos);

    char* pooled_string = string_pool + *pool_pos;
    strncpy(pooled_string, str_ptr, str_len);
    pooled_string[str_len] = '\0';
    *pool_pos += str_len + 1;

    Node* node = CreateNode(pooled_string, parent, false);
    if (node == NULL)
        return NULL;

    if (tree != NULL)
        tree->size++;

    TreeDump(tree, "akinator_parse", buffer, *pos);

    SkipSpaces(buffer, pos);

    TreeDump(tree, "akinator_parse", buffer, *pos);
    node->left = ReadNodeFromBuffer(tree, buffer, pos, node, string_pool, pool_pos, depth + 1);

    SkipSpaces(buffer, pos);

    TreeDump(tree, "akinator_parse", buffer, *pos);
    node->right = ReadNodeFromBuffer(tree, buffer, pos, node, string_pool, pool_pos, depth + 1);

    SkipSpaces(buffer, pos);

    if (buffer[*pos] != ')')
    {
        TreeDestroyWithDataRecursive(node);
        return NULL;
    }
    (*pos)++;

    TreeDump(tree, "akinator_parse", buffer, *pos);

    return node;
}

TreeErrorType TreeLoad(Tree* tree, const char* filename)
{
    if (tree == NULL || filename == NULL)
        return TREE_ERROR_NULL_PTR;

    FILE* file = fopen(filename, "r");
    if (file == NULL)
        return TREE_ERROR_IO;

    fseek(file, 0, SEEK_END);
    long file_size_long = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (file_size_long <= 0)
    {
        fclose(file);
        return TREE_ERROR_IO;
    }

    size_t file_size = (size_t)file_size_long;
    size_t total_pool_size = file_size * 2 + 1;

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

    TreeDtor(tree);

    size_t pos = 0;

    TreeDump(tree, "akinator_parse", file_buffer, pos);

    tree->root = ReadNodeFromBuffer(tree, file_buffer, &pos, NULL, string_pool, &pool_pos, 0);

    TreeDump(tree, "akinator", file_buffer, pos);

    if (tree->root == NULL)
    {
        free(buffer_and_pool);
        return TREE_ERROR_FORMAT;
    }

    tree->file_buffer = buffer_and_pool;

    return TREE_ERROR_NO;
}

static void WriteNodeToFile(FILE* file, const Node* node)
{
    if (node == NULL)
    {
        fprintf(file, "nil");
        return;
    }

    fprintf(file, "( \"%s\" ", node->data);
    WriteNodeToFile(file, node->left);
    fprintf(file, " ");
    WriteNodeToFile(file, node->right);
    fprintf(file, ")");
}

TreeErrorType TreeSave(const Tree* tree, const char* filename)
{
    if (tree == NULL || filename == NULL)
        return TREE_ERROR_NULL_PTR;

    FILE* file = fopen(filename, "w");
    if (file == NULL)
        return TREE_ERROR_IO;

    WriteNodeToFile(file, tree->root);
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

        if (strcmp(answer, "да") == 0 || strcmp(answer, "д") == 0 || strcmp(answer, "yes") == 0 || strcmp(answer, "y") == 0)
            return true;
        else if (strcmp(answer, "нет") == 0 || strcmp(answer, "н") == 0 || strcmp(answer, "no") == 0 || strcmp(answer, "n") == 0)
            return false;
        else
            printf("Пожалуйста, ответьте 'да' или 'нет'.\n");
    }
}
