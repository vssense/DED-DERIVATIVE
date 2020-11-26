#include "derivative.h"


const size_t MAX_INPUT_SIZE = 512;
const char*  STANDARD_INPUT = "derivative.txt";

static const char* TECH_FILE = "tech.txt";

const size_t DOT_CMD_SIZE = 64; 
const size_t JPG_CMD_SIZE = 32;
const char*  STANDARD_DOT_TXT_FILE_NAME = "log\\DerTree.txt";
const char*  DUMP_FILE_NAME = "log\\numsjpg.txt";
const size_t NUM_STR_LEN = 5;
const size_t NOTATION = 10;


DerTree* NewTree                   ();
DerNode* NewNode                   (DerTree* tree);
void     Destruct                  (DerTree* tree);
void     DestructNodes             (DerTree* tree, DerNode* node);
void     DestructNode              (DerTree* tree, DerNode* node);
void     KillChildren              (DerTree* tree, DerNode* node);
void     KillYourselfAndChildren   (DerTree* tree, DerNode* node, ElemT new_value);
void     KillFatherAndBrother      (DerTree* tree, DerNode* node);
void     Delete                    (DerTree* tree);
DerTree* GetTree                   (const int argc, char* argv[]);
void     GetData                   (DerTree* tree, char* buffer);
void     GetConstant               (DerNode* node, char* buffer, size_t* ofs);
void     GetVariative              (DerNode* node, char* buffer, size_t* ofs);
void     GetOperator               (DerNode* node, char* buffer, size_t* ofs);
void     GetBinaryOperator         (DerNode* node, char* buffer, size_t* ofs);
void     GetUnaryOperator          (DerNode* node, char* buffer, size_t* ofs);
DerNode* ConstructNode             (NodeType type, ElemT value, DerNode* left, DerNode* right);
void     TreeDump                  (DerTree* tree);
void     PrintNodes                (DerTree* tree, DerNode* node, FILE* dump_file);
void     GetNames                  (char* dot_cmd, char* jpg_cmd);
size_t   GetJPGNumber              ();
void     PrintExpression           (DerTree* tree);
void     PrintExpressionRecursively(DerTree* tree, DerNode* node, FILE* tech_file);



DerTree* NewTree()
{
    DerTree* tree = (DerTree*)calloc(1, sizeof(DerTree));
    assert(tree);

    tree->nil = (DerNode*)calloc(1, sizeof(DerNode));
    assert(tree->nil);

    tree->nil->parent = tree->nil;
    tree->nil->left   = tree->nil;
    tree->nil->right  = tree->nil;
    tree->nil->type   = TYPE_CONST;

    tree->root = tree->nil;

    return tree;
}

DerNode* NewNode(DerTree* tree)
{
    assert(tree);

    DerNode* node = (DerNode*)calloc(1, sizeof(DerNode));
    assert(node);

    node->left   = tree->nil;
    node->right  = tree->nil;
    node->parent = tree->nil;

    return node; 
}

void Destruct(DerTree* tree)
{
    assert(tree);

    DestructNodes(tree, tree->root);

    free(tree->nil);
    tree->nil = nullptr;
}

void DestructNodes(DerTree* tree, DerNode* node)
{
    assert(tree);
    assert(node);

    if (node == tree->nil) return;

    DestructNodes(tree, node->left);
    DestructNodes(tree, node->right);

    DestructNode(tree, node);
}

void DestructNode(DerTree* tree, DerNode* node)
{
    assert(tree);
    assert(node);

    if (node == tree->nil) return;

    node->right  = nullptr;
    node->left   = nullptr;
    node->parent = nullptr;

    node->value = 0;

    free(node);
}

void KillChildren(DerTree* tree, DerNode* node)
{
    assert(tree);
    assert(node);
    
    DestructNodes(tree, node->right);
    DestructNodes(tree, node->left);
    node->left  = tree->nil;
    node->right = tree->nil;
}

void KillYourselfAndChildren(DerTree* tree, DerNode* node, ElemT new_value)
{
    assert(tree);
    assert(node);
    KillChildren(tree, node);

    node->value = new_value;
}

void KillFatherAndBrother(DerTree* tree, DerNode* node)
{
    assert(tree);
    assert(node);

}

void Delete(DerTree* tree)
{
    assert(tree);

    free(tree);
}

DerTree* GetTree(const int argc, char* argv[])
{
    FILE* input = nullptr;

    if (argc - 1 > 0)
    {
        input = fopen(argv[1], "r");
        assert(input);
    }
    else
    {
        input = fopen(STANDARD_INPUT, "r");
        assert(input);
    }

    DerTree* tree = NewTree();

    char* buffer = (char*)calloc(MAX_INPUT_SIZE, sizeof(char));

    fgets(buffer, MAX_INPUT_SIZE, input);
    GetData(tree, buffer);

    free(buffer);
    fclose(input);

    return tree;
}

void GetData(DerTree* tree, char* buffer)
{
    assert(tree);
    assert(buffer);

    size_t ofs = 0;

    if (buffer[ofs++] == '(')
    {
        tree->root = NewNode(tree);
    }
    else
    {
        assert(!"Syntax error : first symbol must be '('");
    }

    DerNode* node = tree->root;

    while (buffer[ofs] != '\0' && buffer[ofs] != '\n')
    {
        while (buffer[ofs] == ' ') ofs++;

        if (buffer[ofs] == '(')
        {
            node->left = NewNode(tree);
            node->left->parent = node;
            node = node->left;
            ofs++;
        }
        else if (buffer[ofs] == ')')
        {
            while (buffer[ofs] == ')' || buffer[ofs] == ' ')
            {
                if (buffer[ofs] == ')')
                {
                    node = node->parent;
                }
                ofs++;
            }   
        }
        else if (isdigit(buffer[ofs]))
        {
            GetConstant(node, buffer, &ofs);
        }
        else if (strspn(buffer + ofs, VARIABLES))
        {
            GetVariative(node, buffer, &ofs);
        }
        else 
        {
            GetOperator(node, buffer, &ofs);
            if (node->type == TYPE_CONST)
            {
                printf("Syntax error : unknown operator\n ofs = %u", ofs);
            }

            node->right = NewNode(tree);
            node->right->parent = node;
            node = node->right;
            while (buffer[ofs++] != '(');
        }
    }
}

void GetConstant(DerNode* node, char* buffer, size_t* ofs)
{
    assert(node);
    assert(buffer);

    size_t start = 0;
    size_t end = 0;
    sscanf(buffer + *ofs, "%n%d%n", &start, &node->value, &end);  //<<<------ change to %lf if double

    *ofs += end - start;
    node->type = TYPE_CONST;
}

void GetVariative(DerNode* node, char* buffer, size_t* ofs)
{
    assert(node);
    assert(buffer);

    node->value = buffer[*ofs];
    node->type  = TYPE_VAR;
    (*ofs)++;
}

void GetOperator(DerNode* node, char* buffer, size_t* ofs)
{
    assert(node);
    assert(buffer);

    GetBinaryOperator(node, buffer, ofs);
    if (node->type == TYPE_CONST)
    {
        GetUnaryOperator(node, buffer, ofs);
    }
}

void GetBinaryOperator(DerNode* node, char* buffer, size_t* ofs)
{
    assert(node);
    assert(buffer);

    size_t num_of_operator = NO_MATCHES;
    for (size_t i = 0; i < NUM_UNARY_OP; ++i)
    {
        if (strncmp(buffer + *ofs, BINARY_OP[i], strlen(BINARY_OP[i])) == 0)
        {
            num_of_operator = i;
            node->value = num_of_operator;
            node->type  = TYPE_BIN_OP;
            (*ofs)++;
            break;
        }
    }
}

void GetUnaryOperator(DerNode* node, char* buffer, size_t* ofs)
{
    assert(node);
    assert(buffer);
    
    size_t num_of_operator = NO_MATCHES;
    for (size_t i = 0; i < NUM_UNARY_OP; ++i)
    {
        if (strncmp(buffer + *ofs, UNARY_OP[i], strlen(UNARY_OP[i])) == 0)
        {
            num_of_operator = i;
            node->value = num_of_operator;
            node->type = TYPE_UN_OP;
            (*ofs) += strlen(UNARY_OP[num_of_operator]);
            break;
        }
    }
}

DerNode* ConstructNode(NodeType type, ElemT value, DerNode* left, DerNode* right)
{
    DerNode* node = (DerNode*)calloc(1, sizeof(DerNode));

    node->type  = type;
    node->value = value;

    node->left  = left;
    node->right = right;

    return node;
}

void TreeDump(DerTree* tree)
{
    assert(tree);

    FILE* dump_file = fopen(STANDARD_DOT_TXT_FILE_NAME, "w");

    fprintf(dump_file, "digraph G{\n");
    fprintf(dump_file, "node [shape=\"circle\"]\n");

    PrintNodes(tree, tree->root, dump_file);

    fprintf(dump_file, "}");

    fclose(dump_file);

    char dot_cmd[DOT_CMD_SIZE] = "";
    snprintf(dot_cmd, DOT_CMD_SIZE, "dot -Tjpg %s -o log\\Dump", STANDARD_DOT_TXT_FILE_NAME);
    char jpg_cmd[JPG_CMD_SIZE] = "start log\\Dump";
    
    GetNames(dot_cmd, jpg_cmd);

    system(dot_cmd);
    system(jpg_cmd);
}

void PrintNodes(DerTree* tree, DerNode* node, FILE* dump_file)
{
    assert(tree);
    assert(dump_file);

    if (node->type == NODE_ERROR)
    {
        fprintf(dump_file, "\"%p\"[style=\"filled\", fillcolor=\"#EF4C3A\", fontcolor=\"black\", label=\"%d\"]", node, node->value);
    }
    else if (node->type == TYPE_CONST)
    {
        fprintf(dump_file, "\"%p\"[style=\"filled\", fillcolor=\"#9BC3F7\", fontcolor=\"black\", label=\"%d\"]", node, node->value);
    }
    else if (node->type == TYPE_VAR)
    { 
        fprintf(dump_file, "\"%p\"[style=\"filled\", fillcolor=\"#C6F7DD\", fontcolor=\"black\", label=\"%c\"]", node, node->value);
    }
    else if (node->type == TYPE_BIN_OP)
    {
        fprintf(dump_file, "\"%p\"[style=\"filled\", fillcolor=\"#F6F7C6\", fontcolor=\"black\", label=\"%s\"]", node, BINARY_OP[node->value]);
    }
    else
    {
        fprintf(dump_file, "\"%p\"[style=\"filled\", fillcolor=\"#F6F7C6\", fontcolor=\"black\", label=\"%s\"]", node, UNARY_OP[node->value]);
    }

    if (node->left != tree->nil)
    {
        fprintf(dump_file, "\"%p\":sw->\"%p\";\n", node, node->left);
        PrintNodes(tree, node->left, dump_file);
    }
    
    if (node->right != tree->nil)
    {
        fprintf(dump_file, "\"%p\":se->\"%p\";\n", node, node->right);
        PrintNodes(tree, node->right, dump_file);
    }
}

void GetNames(char* dot_cmd, char* jpg_cmd)
{
    assert(dot_cmd);
    assert(jpg_cmd);

    size_t num = GetJPGNumber();

    char num_str[NUM_STR_LEN] = "";
    itoa(num, num_str, NOTATION);
    char extension[] = ".jpg";


    strcat(dot_cmd, num_str);
    strcat(dot_cmd, extension);
    
    strcat(jpg_cmd, num_str);
    strcat(jpg_cmd, extension);
}

size_t GetJPGNumber()
{
    FILE* numjpgs = fopen(DUMP_FILE_NAME, "r");

    size_t num = 0;
    
    if (numjpgs != nullptr)
    {
        fscanf(numjpgs, "%u", &num);
    }

    fclose(numjpgs);

    numjpgs = fopen(DUMP_FILE_NAME, "w");
    fprintf(numjpgs, "%u", num + 1);
    fclose(numjpgs);
    
    return num;
}

void PrintExpression(DerTree* tree)
{
    FILE* tech_file = fopen(TECH_FILE, "w");
    assert(tech_file);

    PrintExpressionRecursively(tree, tree->root, tech_file);

    fclose(tech_file);
    system(TECH_FILE);
}

void PrintExpressionRecursively(DerTree* tree, DerNode* node, FILE* tech_file)
{
    if (node == tree->nil) return;

    fprintf(tech_file, "(");
    PrintExpressionRecursively(tree, node->left, tech_file);
    
    if (node->type == TYPE_CONST)
    {
        fprintf(tech_file, "%d", node->value);
    }
    else if (node->type == TYPE_VAR)
    { 
        fprintf(tech_file, "%c", node->value);
    }
    else if (node->type == TYPE_BIN_OP)
    {
        fprintf(tech_file, " %s ", BINARY_OP[node->value]);
    }
    else
    {
        fprintf(tech_file, "%s", UNARY_OP[node->value]);
    }
    PrintExpressionRecursively(tree, node->right, tech_file);
    fprintf(tech_file, ")");
}

