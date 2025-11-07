#include "my_derevo.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

static TreeErrorType TreeDestroyRecursive(Node* node)
{
    if (node == NULL)
        return TREE_ERROR_NO; //для рекурсии это норм

    TreeDestroyRecursive(node->left);
    TreeDestroyRecursive(node->right);

    free(node);

    return TREE_ERROR_NO;
}

static TreeErrorType PrintTreeNode(const Node* node)
{
    if (node == NULL)
    {
        printf("()");
        return TREE_ERROR_NO;
    }

    printf("(");
    if (node->left)
        PrintTreeNode(node->left);

    printf("%d", node->data);

    if (node->right)
        PrintTreeNode(node->right);
    printf(")");

    return TREE_ERROR_NO;
}

static TreeErrorType TreeInsertRecursive(Node** node_ptr, Node* parent, TreeElement value) //иначе будем изменять копию node_ptr
{
    if (*node_ptr == NULL)
    {
        *node_ptr = (Node*)calloc(1, sizeof(Node));
        if (*node_ptr == NULL)
            return TREE_ERROR_ALLOCATION;

        (*node_ptr)->data   = value;
        (*node_ptr)->left   = NULL;
        (*node_ptr)->right  = NULL;
        (*node_ptr)->parent = parent;
        return TREE_ERROR_NO;
    }

    if (value <= (*node_ptr)->data)
        return TreeInsertRecursive(&(*node_ptr)->left,  *node_ptr, value);

    else if (value > (*node_ptr)->data)
        return TreeInsertRecursive(&(*node_ptr)->right, *node_ptr, value);

    return TREE_ERROR_NO;
}

TreeErrorType TreeCtor(Tree* tree)
{
    tree->root = NULL;
    tree->size = 0;

    return TREE_ERROR_NO;
}

TreeErrorType TreeInsert(Tree* tree, TreeElement value)
{
    if (tree == NULL)
        return TREE_ERROR_NULL_PTR;

    TreeErrorType result = TreeInsertRecursive(&tree->root, NULL, value);
    if (result == TREE_ERROR_NO)
        tree->size++;

    return result;
}

TreeErrorType TreeBaseDump(Tree* tree)
{
    if (tree == NULL)
    {
        printf("Tree: NULL\n");
        return TREE_ERROR_NULL_PTR;
    }

    printf("===TREE DUMP===\n");
    printf("Tree size: %lu\n", tree->size);
    printf("Tree structure:\n");

    if (tree->root == NULL)
    {
        printf("EMPTY TREE");
    }
    else
    {
        PrintTreeNode(tree->root);
    }

    putchar('\n');
    return TREE_ERROR_NO;
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

TreeErrorType TreeDump(Tree* tree, const char* filename)
{
    if (tree == NULL)
        return TREE_ERROR_NULL_PTR;

    char command[kMaxSystemCommandLength] = {};
    snprintf(command, sizeof(command), "mkdir -p %s", kGeneralFolderNameForLogs);
    system(command);

    char folder_name[kMaxLengthOfFilename] = {};
    snprintf(folder_name, sizeof(folder_name), "%s_dump", filename);

    char folder_path[kMaxLengthOfFilename] = {};
    snprintf(folder_path, sizeof(folder_path), "%s/%s_dump", kGeneralFolderNameForLogs, filename);

    snprintf(command, sizeof(command), "mkdir -p %s", folder_path);
    system(command);

    char htm_filename[kMaxLengthOfFilename] = {};
    snprintf(htm_filename, sizeof(htm_filename), "%s/%s.htm", kGeneralFolderNameForLogs, filename);

    FILE* htm_file = fopen(htm_filename, "a");
    if (!htm_file)
        return TREE_ERROR_OPENING_FILE;

    TreeErrorType result = TreeDumpToHtm(tree, htm_file, folder_path, folder_name);

    fclose(htm_file);
    return result;
}

TreeErrorType TreeDumpToHtm(Tree* tree, FILE* htm_file, const char* folder_path, const char* folder_name)
{
    assert(tree);
    assert(htm_file);
    assert(folder_path);

    time_t now = time(NULL);

    WriteDumpHeader(htm_file, now);
    WriteTreeInfo(htm_file, tree);

    static int n_of_pictures = 0;

    char temp_dot_global_path[kMaxLengthOfFilename] = {};
    char temp_svg_global_path[kMaxLengthOfFilename] = {};
    snprintf(temp_dot_global_path, sizeof(temp_dot_global_path), "%s/tree_%d%ld.dot",
             folder_path, n_of_pictures, now);
    snprintf(temp_svg_global_path, sizeof(temp_svg_global_path), "%s/tree_%d%ld.svg",
             folder_path, n_of_pictures, now);

    char temp_svg_local_path[kMaxLengthOfFilename] = {};
    snprintf(temp_svg_local_path, sizeof(temp_svg_local_path), "%s/tree_%d%ld.svg",
             folder_name, n_of_pictures, now);

    n_of_pictures++;

    TreeErrorType dot_result = GenerateDotFile(tree, temp_dot_global_path);
    if (dot_result != TREE_ERROR_NO)
        return dot_result;

    char command[kMaxSystemCommandLength] = {};
    snprintf(command, sizeof(command), "dot -Tsvg %s -o %s", temp_dot_global_path, temp_svg_global_path);
    int result = system(command);

    if (result == 0)
    {
        fprintf(htm_file, "<div style='text-align:center;'>\n");
        fprintf(htm_file, "<img src='%s' style='max-width:100%%; border:1px solid #ddd;'>\n", temp_svg_local_path);
        fprintf(htm_file, "</div>\n");
    }
    else
    {
        fprintf(htm_file, "<p style='color:red;'>Error generating SVG graph</p>\n");
    }

    remove(temp_dot_global_path);

    WriteDumpFooter(htm_file);
    return TREE_ERROR_NO;
}

TreeErrorType GenerateDotFile(Tree* tree, const char* filename)
{
    assert(tree);
    assert(filename);

    FILE* dot_file = fopen(filename, "w");
    if (!dot_file)
        return TREE_ERROR_OPENING_FILE;

    fprintf(dot_file, "digraph BinaryTree {\n");
    fprintf(dot_file, "    rankdir=TB;\n");
    fprintf(dot_file, "    node [shape=record, style=filled, color=black];\n\n");

    CreateDotNodes(tree, dot_file);

    if (tree->root != NULL)
    {
        CreateTreeConnections(tree->root, dot_file);
    }
    else
    {
        fprintf(dot_file, "    empty [label=\"Empty Tree\", shape=plaintext];\n");
    }

    fprintf(dot_file, "}\n");
    fclose(dot_file);

    return TREE_ERROR_NO;
}

static void CreateNodeRecursive(Node* node, Tree* tree, FILE* dot_file)
{
    if (node == NULL)
        return;

    const char* color = GetNodeColor(node, tree);

    fprintf(dot_file, "    node_%p [label=\"{ {data: %d} | {address: %p} | {left %p| right %p| parent %p} }\", fillcolor=%s];\n",
            (void*)node, node->data, (void*)node, (void*)node->left, (void*)node->right, (void*)node->parent, color);

    CreateNodeRecursive(node->left,  tree, dot_file);
    CreateNodeRecursive(node->right, tree, dot_file);
}

void CreateDotNodes(Tree* tree, FILE* dot_file)
{
    assert(tree);
    assert(dot_file);

    CreateNodeRecursive(tree->root, tree, dot_file);
}

void CreateTreeConnections(Node* node, FILE* dot_file)
{
    if (node == NULL)
        return;

    // чекаем связь с левым
    if (node->left != NULL)
    {
        // проверяем, что связь взаимная
        if (node->left->parent == node)
        {
            fprintf(dot_file, "    node_%p -> node_%p [color=blue, dir=both, arrowtail=normal, arrowhead=normal, label=\"L\"];\n",
                    (void*)node, (void*)node->left);
        }
        else
        {
            fprintf(dot_file, "    node_%p -> node_%p [color=blue, label=\"L\"];\n",
                    (void*)node, (void*)node->left);

            fprintf(dot_file, "    error_parent_%p [shape=ellipse, style=filled, fillcolor=orange, label=\"Parent address Error\"];\n",
                    (void*)node->left);
            fprintf(dot_file, "    node_%p -> error_parent_%p [color=red];\n", (void*)node->left, (void*)node->left);
        }
        CreateTreeConnections(node->left, dot_file);
    }

    // чекаем связь с правым
    if (node->right != NULL)
    {
        // проверяем на взаимность
        if (node->right->parent == node)
        {
            fprintf(dot_file, "    node_%p -> node_%p [color=green, dir=both, arrowtail=normal, arrowhead=normal, label=\"R\"];\n",
                    (void*)node, (void*)node->right);
        }
        else
        {
            fprintf(dot_file, "    node_%p -> node_%p [color=green, label=\"R\"];\n",
                    (void*)node, (void*)node->right);

            fprintf(dot_file, "    error_parent_%p [shape=ellipse, style=filled, fillcolor=orange, label=\"Parent address Error\"];\n",
                    (void*)node->right);
            fprintf(dot_file, "    node_%p -> error_parent_%p [color=red, style=dashed];\n", (void*)node->right, (void*)node->right);
        }
        CreateTreeConnections(node->right, dot_file);
    }

    // if (node->parent != NULL && node->parent->left != node && node->parent->right != node)
    // {
    //     fprintf(dot_file, "    node_%p -> node_%p [color=red, style=dashed, label=\"P\"];\n",
    //             (void*)node, (void*)node->parent);
    // }
}

const char* GetNodeColor(Node* node, Tree* tree)
{
    if (node == tree->root)
        return "lightcoral"; // красный корень
    else if (node->left == NULL && node->right == NULL)
        return "lightgreen"; // листья
    else
        return "lightblue";
}

void WriteTreeInfo(FILE* htm_file, Tree* tree)
{
    assert(htm_file);
    assert(tree);

    fprintf(htm_file, "<div style='margin-bottom:15px;'>\n");
    fprintf(htm_file, "<p><b>Tree Size:</b> %lu</p>\n", tree->size);

    if (tree->root != NULL)
    {
        fprintf(htm_file, "<p><b>Root Address:</b> %p</p>\n", (void*)tree->root);
        fprintf(htm_file, "<p><b>Root Data:</b> %d</p>\n", tree->root->data);
    }
    else
    {
        fprintf(htm_file, "<p><b>Root:</b> NULL</p>\n");
    }

    TreeVerifyResult verify_result = VerifyTree(tree); //FIXME
    const char* verify_result_in_string = TreeVerifyResultToString(verify_result);
    const char* verify_color = (verify_result == TREE_VERIFY_SUCCESS) ? "green" : "red";

    fprintf(htm_file, "<p><b>Verify result:</b> <span style='color:%s; font-weight: bold;'>%s</span></p>\n",
            verify_color, verify_result_in_string);

    fprintf(htm_file, "</div>\n");
}

void WriteDumpHeader(FILE* htm_file, time_t now)
{
    assert(htm_file);

    fprintf(htm_file, "<div style='border:2px solid #ccc; margin:10px; padding:15px; background:#f9f9f9;'>\n");
    fprintf(htm_file, "<h2 style='color:#333;'>Tree Dump at %s</h2>\n", ctime(&now));
}

void WriteDumpFooter(FILE* htm_file)
{
    assert(htm_file);
    fprintf(htm_file, "</div>\n\n");
}

TreeVerifyResult VerifyTree(Tree* tree) //FIXME дописать
{
    if (tree == NULL)
        return TREE_VERIFY_NULL_PTR;

    if (tree->root != NULL && tree->root->parent != NULL)
        return TREE_VERIFY_INVALID_PARENT;

    return TREE_VERIFY_SUCCESS;
}

const char* TreeVerifyResultToString(TreeVerifyResult result)
{
    switch (result)
    {
        case TREE_VERIFY_SUCCESS:          return "Success";
        case TREE_VERIFY_NULL_PTR:         return "Null pointer";
        case TREE_VERIFY_INVALID_PARENT:   return "Invalid parent pointer";
        case TREE_VERIFY_CYCLE_DETECTED:   return "Cycle detected";           //FIXME проверить на циклы
        default:                           return "Unknown error";
    }
}

TreeErrorType InitTreeLog(const char* filename)
{
    assert(filename);

    char htm_filename[kMaxLengthOfFilename] = {};
    snprintf(htm_filename, sizeof(htm_filename), "%s/%s.htm", kGeneralFolderNameForLogs, filename);

    FILE* htm_file = fopen(htm_filename, "w");
    if (!htm_file)
        return TREE_ERROR_OPENING_FILE;

    fprintf(htm_file, "<!DOCTYPE html>\n"
                      "<html>\n"
                      "<head>\n"
                      "<title>Tree Dump Log</title>\n"
                      "<style>\n"
                      "body { font-family: Arial, sans-serif; margin: 20px; }\n"
                      "table { border-collapse: collapse; width: 100%%; }\n"
                      "th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }\n"
                      "th { background-color: #f2f2f2; }\n"
                      "</style>\n"
                      "</head>\n"
                      "<body>\n"
                      "<h1>Tree Dump Log</h1>\n");
    fclose(htm_file);

    return TREE_ERROR_NO;
}

TreeErrorType CloseTreeLog(const char* filename)
{
    assert(filename);

    char htm_filename[kMaxLengthOfFilename] = {};
    snprintf(htm_filename, sizeof(htm_filename), "%s/%s.htm", kGeneralFolderNameForLogs, filename);

    FILE* htm_file = fopen(htm_filename, "a");
    if (!htm_file)
        return TREE_ERROR_OPENING_FILE;

    fprintf(htm_file, "</body>\n");
    fprintf(htm_file, "</html>\n");
    fclose(htm_file);

    return TREE_ERROR_NO;
}
