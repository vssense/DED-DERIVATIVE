#include "derivative.h"
#include "expression_loader.h"


const size_t MAX_INPUT_SIZE = 512;
const char*  STANDARD_INPUT = "src\\derivative.txt";

static const char* TECH_FILE = "tech\\tech.tex";

const size_t DOT_CMD_SIZE = 64;
const size_t JPG_CMD_SIZE = 32;
const char*  STANDARD_DOT_TXT_FILE_NAME = "log\\DerTree.txt";
const char*  DUMP_FILE_NAME = "log\\numsjpg.txt";
const size_t NUM_STR_LEN = 5;
const size_t NOTATION = 10;


DerTree* NewTree                   ();
DerTree* CopyTree                  (DerTree* tree);
DerNode* NewNode                   (DerTree* tree);
void     Destruct                  (DerTree* tree);
void     DestructNodes             (DerTree* tree, DerNode* node);
void     DestructNode              (DerTree* tree, DerNode* node);
void     KillChildren              (DerTree* tree, DerNode* node);
void     KillYourselfAndChildren   (DerTree* tree, DerNode* node, ElemT new_value);
void     KillFatherAndBrother      (DerTree* tree, DerNode* node);
void     Delete                    (DerTree* tree);
DerTree* GetTree                   (const int argc, char* argv[]);
void     SetNils                   (DerTree* tree, DerNode* node);
DerNode* ConstructNode             (NodeType type,  ElemT value, DerNode* left, DerNode* right);
void     TreeDump                  (DerTree* tree);
void     PrintNodes                (DerTree* tree, DerNode* node, FILE* dump_file);
void     PrintNodesHard            (DerTree* tree, DerNode* node, FILE* dump_file);
void     GetNames                  (char* dot_cmd, char* jpg_cmd);
size_t   GetJPGNumber              ();
void     PrintExpression           (DerTree* tree);
void     PrintExpressionRecursively(DerTree* tree, DerNode* node, FILE* tech_file);
void     PrintTexBinOP             (FILE* file, DerNode* node);



DerTree* NewTree()
{
    DerTree* tree = (DerTree*)calloc(1, sizeof(DerTree));
    assert(tree);

    tree->nil = (DerNode*)calloc(1, sizeof(DerNode));
    assert(tree->nil);

    tree->nil->parent = tree->nil;
    tree->nil->left   = tree->nil;
    tree->nil->right  = tree->nil;
    tree->nil->type   = TYPE_NIL;

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
    node->type   = TYPE_NIL;

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

    node->value.number = 0;

    free(node);
}

DerTree* CopyTree(DerTree* tree)
{
    DerTree* new_tree = (DerTree*)calloc(1, sizeof(DerTree));
    assert(new_tree);

    new_tree->root = CopySubTree(tree, tree->root);
    new_tree->nil  = tree->nil;

    return new_tree;
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

    node->value.number = new_value;
    node->type         = TYPE_CONST;
}

void KillFatherAndBrother(DerTree* tree, DerNode* node)
{
    assert(tree);
    assert(node);

    if (node->parent->right == node)
    {
        node->parent->right = tree->nil;
    }
    else
    {
        node->parent->left = tree->nil;
    }

    DerNode* father = node->parent;
    node->parent    = node->parent->parent;

    if (father->parent != tree->nil)
    {
        if (father->parent->right == father)
        {
            father->parent->right = node;
        }
        else
        {
            father->parent->left = node;
        }
    }
    else
    {
        tree->root = node;
    }

    DestructNodes(tree, father);

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
    assert(buffer);

    fgets(buffer, MAX_INPUT_SIZE, input);

    tree->root = GetAnswer(buffer);

    SetNils(tree, tree->root);

    free(buffer);
    fclose(input);

    return tree;
}

void SetNils(DerTree* tree, DerNode* node)
{
    if (node == tree->nil) return;

    if (node->parent == nullptr) node->parent = tree->nil;
    if (node->right  == nullptr) node->right  = tree->nil;
    if (node->left   == nullptr) node->left   = tree->nil;

    SetNils(tree, node->right);
    SetNils(tree, node->left);
}

DerNode* ConstructNode(NodeType type, Value value, DerNode* left, DerNode* right)
{
    DerNode* node = (DerNode*)calloc(1, sizeof(DerNode));
    assert(node);

    node->type  = type;
    node->value = value;

    node->left  = left;
    node->right = right;

    // if (left != nullptr)
    // {
    //     left->parent = node;
    // }

    // if (right != nullptr)
    // {
    //     right->parent = node;
    // }

    return node;
}

void TreeDump(DerTree* tree)
{
    assert(tree);

    FILE* dump_file = fopen(STANDARD_DOT_TXT_FILE_NAME, "w");

    fprintf(dump_file, "digraph G{\n");
    fprintf(dump_file, "node [shape=\"circle\"]\n");

    PrintNodesHard(tree, tree->root, dump_file);

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

    if (node->type == NODE_ERROR || node->type == TYPE_NIL)
    {
        fprintf(dump_file, "\"%p\"[style=\"filled\", fillcolor=\"#EF4C3A\", label=\"%lg\"]",
                node, node->value.number);
    }
    else if (node->type == TYPE_CONST)
    {
        fprintf(dump_file, "\"%p\"[style=\"filled\", fillcolor=\"#9BC3F7\", label=\"%lg\"]",
                node, node->value.number);
    }
    else if (node->type == TYPE_VAR)
    { 
        fprintf(dump_file, "\"%p\"[style=\"filled\", fillcolor=\"#C6F7DD\", label=\"%c\"]",
                node, node->value.var);
    }
    else if (node->type == TYPE_BIN_OP)
    {
        fprintf(dump_file, "\"%p\"[style=\"filled\", fillcolor=\"#F6F7C6\", label=\"%s\"]",
                node, BINARY_OP[node->value.op]);
    }
    else if (node->type == TYPE_UN_OP)
    {
        fprintf(dump_file, "\"%p\"[style=\"filled\", fillcolor=\"#F6F796\", label=\"%s\"]",
                node, UNARY_OP[node->value.op]);
    }

    if (node->left != tree->nil && node->left)
    {
        fprintf(dump_file, "\"%p\":sw->\"%p\";\n", node, node->left);
        PrintNodes(tree, node->left, dump_file);
    }
    
    if (node->right != tree->nil && node->right)
    {
        fprintf(dump_file, "\"%p\":se->\"%p\";\n", node, node->right);
        PrintNodes(tree, node->right, dump_file);
    }
}

void PrintNodesHard(DerTree* tree, DerNode* node, FILE* dump_file)
{
    assert(tree);
    assert(dump_file);

    if (node->type == NODE_ERROR || node->type == TYPE_NIL)
    {
        fprintf(dump_file, "\"%p\"[shape=\"record\", style=\"filled\", fillcolor=\"#EF4C3A\", label=\"%lg|p: %p|%p|{l: %p|r: %p}\"]",
                node, node->value.number, node->parent, node, node->left, node->right);
    }
    else if (node->type == TYPE_CONST)
    {
        fprintf(dump_file, "\"%p\"[shape=\"record\", style=\"filled\", fillcolor=\"#9BC3F7\", label=\"%lg|p: %p|%p|{l: %p|r: %p}\"]",
                node, node->value.number, node->parent, node, node->left, node->right);
    }
    else if (node->type == TYPE_VAR)
    { 
        fprintf(dump_file, "\"%p\"[shape=\"record\", style=\"filled\", fillcolor=\"#C6F7DD\", label=\"%c|p: %p|%p|{l: %p|r: %p}\"]",
                node, node->value.var, node->parent, node, node->left, node->right);
    }
    else if (node->type == TYPE_BIN_OP)
    {
        fprintf(dump_file, "\"%p\"[shape=\"record\", style=\"filled\", fillcolor=\"#F6F7C6\", label=\"%s|p: %p|%p|{l: %p|r: %p}\"]",
                node, BINARY_OP[node->value.op], node->parent, node, node->left, node->right);
    }
    else
    {
        fprintf(dump_file, "\"%p\"[shape=\"record\", style=\"filled\", fillcolor=\"#F6F796\", label=\"%s|p: %p|%p|{l: %p|r: %p}\"]",
                node, UNARY_OP[node->value.op], node->parent, node, node->left, node->right);
    }

    if (node->left != tree->nil && node->left)
    {
        fprintf(dump_file, "\"%p\":sw->\"%p\";\n", node, node->left);
        PrintNodesHard(tree, node->left, dump_file);
    }
    
    if (node->right != tree->nil && node->right)
    {
        fprintf(dump_file, "\"%p\":se->\"%p\";\n", node, node->right);
        PrintNodesHard(tree, node->right, dump_file);
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

    fprintf(tech_file, "\\documentclass[32pt]{article}\n"
                       "\\begin{document}             \n$");

    PrintExpressionRecursively(tree, tree->root, tech_file);

    fprintf(tech_file,"$\n\\end{document}\n");

    fclose(tech_file);
    system("tech");
}

bool IsMul(DerNode* node)
{
    return node->type == TYPE_BIN_OP && node->value.op == OP_MUL;
}

bool IsSum(DerNode* node)
{
    return node->type == TYPE_BIN_OP && 
    (node->value.op == OP_ADD || node->value.op == OP_SUB);
}

bool IsDiv(DerNode* node)
{
    return node->type == TYPE_BIN_OP && node->value.op == OP_DIV;
}

bool IsPow(DerNode* node)
{
    return node->type == TYPE_BIN_OP && node->value.op == OP_POW;
}

bool IsList(DerTree* tree, DerNode* node)
{
    return node->right == tree->nil && node->left == tree->nil;
}

bool IsUnOP(DerNode* node)
{
    return node->type == TYPE_UN_OP;
}

bool BracketNeeded(DerTree* tree, DerNode* node)
{
    return (!IsList(tree, node) && (IsMul(node->parent) && IsSum(node))) ||
           (IsPow(node->parent) && node->parent->left == node && !IsList(tree, node)) ;
}

void PrintExpressionRecursively(DerTree* tree, DerNode* node, FILE* tech_file)
{
    if (node == tree->nil) return;

    if (BracketNeeded(tree, node))
    {
        fprintf(tech_file, "{(");    
    }
    else
    {
        fprintf(tech_file, "{");
    }

    PrintExpressionRecursively(tree, node->left, tech_file);
    
    if (node->type == TYPE_CONST)
    {
        fprintf(tech_file, "%lg", node->value.number);
    }
    else if (node->type == TYPE_VAR)
    { 
        fprintf(tech_file, "%c", node->value.var);
    }
    else if (node->type == TYPE_BIN_OP)
    {
        PrintTexBinOP(tech_file, node);
    }
    else
    {
        fprintf(tech_file, "\\%s ", UNARY_OP[node->value.op]);
    }
    PrintExpressionRecursively(tree, node->right, tech_file);

    if (BracketNeeded(tree, node))
    {
        fprintf(tech_file, "})");    
    }
    else
    {
        fprintf(tech_file, "}");
    }
}

void PrintTexBinOP(FILE* file, DerNode* node)
{
    assert(file);
    assert(node);

    switch (node->value.op)
    {
        case OP_ADD :
        {
            fprintf(file, " + ");
            break;
        }
        case OP_SUB :
        {
            fprintf(file, " - ");
            break;
        }
        case OP_MUL :
        {
            if (node->left->type != TYPE_CONST)
            {
                fprintf(file, " \\cdot ");
            }
            break;
        }
        case OP_DIV :
        {
            fprintf(file, " \\over ");
            break;
        }
        case OP_POW :
        {
            fprintf(file, " ^ ");
            break;
        }
        default :
        {
            printf("Tex error : unknown binary command\nline = %d", __LINE__);
            break;
        }
    }
}