#include "derivative.h"


/////////////////////////////////
//Simplification
/////////////////////////////////
void Simplify           (DerTree* tree);
void CalculateConsts    (DerTree* tree, DerNode* node, bool* sth_has_changed);
void CalculateBinOP     (DerTree* tree, DerNode* node);
void CalculateUnOP      (DerTree* tree, DerNode* node);
void CalculateNeutralOP (DerTree* tree, DerNode* node, bool* sth_has_changed);
void CalculateNeutralAdd(DerTree* tree, DerNode* node);
void CalculateNeutralSub(DerTree* tree, DerNode* node);
void CalculateNeutralMul(DerTree* tree, DerNode* node);
void CalculateNeutralDiv(DerTree* tree, DerNode* node);
void CalculateNeutralPow(DerTree* tree, DerNode* node);

/////////////////////////////////
//Direvative
/////////////////////////////////


int main(const int argc, char* argv[])
{
    DerTree* tree = GetTree(argc, argv);

    TreeDump(tree);
    // CalculateConsts(tree, tree->root);
    // PrintExpression(tree);
    // CalculateNeutralOP(tree, tree->root);

    Simplify(tree);

    TreeDump(tree);

    Destruct(tree);
    Delete(tree);

    return 0;
}

void Simplify(DerTree* tree)
{
    bool sth_has_changed = true;

    while (sth_has_changed)
    {
        sth_has_changed = false;
        CalculateConsts(tree, tree->root, &sth_has_changed);
        CalculateNeutralOP(tree, tree->root, &sth_has_changed);
    }
}

#define Rval  node->right->value
#define Lval  node->left->value
#define Rtype node->right->type
#define Ltype node->left->type

void CalculateConsts(DerTree* tree, DerNode* node, bool* sth_has_changed)
{
    assert(tree);
    assert(node);

    if (node == tree->nil) return;

    CalculateConsts(tree, node->right, sth_has_changed);
    CalculateConsts(tree, node->left,  sth_has_changed);

    if (node->type == TYPE_BIN_OP)
    {
        if (Rtype == TYPE_CONST && Ltype == TYPE_CONST)
        {
            node->type = TYPE_CONST;
            CalculateBinOP(tree, node);
            *sth_has_changed = true;
        }
    }

    if (node->type == TYPE_UN_OP)
    {
        if (Rtype == TYPE_CONST)
        {
            node->type = TYPE_CONST;
            CalculateUnOP(tree, node);
            *sth_has_changed = true;
        }
    }
}

void CalculateBinOP(DerTree* tree, DerNode* node)
{
    switch (node->value)
    {
        case OP_ADD :
        {
            node->value = Lval + Rval;
            break;
        }
        case OP_SUB :
        {
            node->value = Lval - Rval;
            break;
        }
        case OP_MUL :
        {
            node->value = Lval * Rval;
            break;
        }
        case OP_DIV :
        {
            if (Rval != 0)
            {
                node->value = Lval / Rval;
            } 
            else
            {
                node->type = NODE_ERROR;
            }
            break;
        }
        case OP_POW :
        {
            node->value = (ElemT)pow(Lval, Rval);
            break;
        }
        default :
        {
            printf("Error in calculating constants : unknown type of node\n");
            break;
        }
    }
    KillChildren(tree, node);
}

void CalculateUnOP(DerTree* tree, DerNode* node)
{
    switch (node->value)
    {
        case OP_SIN :
        {
            node->value = (ElemT)sin(Rval);
            break;
        }
        case OP_COS :
        {
            node->value = (ElemT)cos(Rval);
            break;
        }
        case OP_TG :
        {
            node->value = (ElemT)tan(Rval);
            break;
        }
        case OP_CTG :
        {
            if (Rval != 0)
            {
                node->value = (ElemT)(1 / tan(Rval));
            }
            break;
        }
        case OP_SQRT :
        {
            if (Rval >= 0)
            {
                node->value = (ElemT)sqrt(Rval);
            }
            else
            {
                node->type = NODE_ERROR;
            }
            break;
        }
        default :
        {
            printf("Error in calculating constants : unknown type of node\n");
            break;
        }
    }
    DestructNode(tree, node->right);
    node->right = tree->nil;
}

void CalculateNeutralOP(DerTree* tree, DerNode* node, bool* sth_has_changed)
{
    assert(tree);
    assert(node);

    if (node == tree->nil) return;

    CalculateNeutralOP(tree, node->right, sth_has_changed);
    CalculateNeutralOP(tree, node->left,  sth_has_changed);

    if (node->type == TYPE_BIN_OP)
    {
        switch (node->value)
        {
            case OP_ADD :
            {
                CalculateNeutralAdd(tree, node);
                break;  
            }
            case OP_SUB :
            {
                CalculateNeutralSub(tree, node);
                break;  
            }
            case OP_MUL :
            {
                CalculateNeutralMul(tree, node);
                break;
            }
            case OP_DIV :
            {
                CalculateNeutralDiv(tree, node);
                break;  
            }
            case OP_POW :
            {
                CalculateNeutralPow(tree, node);
                break;  
            }
        }
        if (node->type != TYPE_BIN_OP) 
        {
            *sth_has_changed = true;
        }
    }
}

void CalculateNeutralAdd(DerTree* tree, DerNode* node)
{
    if (Rval == ADD_NEUT && Rtype == TYPE_CONST)
    {
        KillFatherAndBrother(tree, node->left);
    }
    else if (Lval == ADD_NEUT && Ltype == TYPE_CONST)
    {
        KillFatherAndBrother(tree, node->right);
    }
}
void CalculateNeutralSub(DerTree* tree, DerNode* node)
{
    if (Rval == SUB_NEUT && Rtype == TYPE_CONST)
    {
        KillFatherAndBrother(tree, node->left);
    }
}
void CalculateNeutralMul(DerTree* tree, DerNode* node)
{
    if ((Rval == MUL_NULL && Rtype == TYPE_CONST) || 
        (Lval == MUL_NULL && Ltype == TYPE_CONST))
    {
        KillYourselfAndChildren(tree, node, MUL_NULL);
    }
    else if (Rval == MUL_NEUT && Rtype == TYPE_CONST) 
    {
        KillFatherAndBrother(tree, node->left);
    }
    else if (Lval == MUL_NEUT && Ltype == TYPE_CONST)
    {
        KillFatherAndBrother(tree, node->right);
    }
}
void CalculateNeutralDiv(DerTree* tree, DerNode* node)
{
    if (Lval == DIV_NULL && Ltype == TYPE_CONST)
    {
        KillYourselfAndChildren(tree, node, DIV_NULL);
    }
    else if (Rval == DIV_NEUT && Rtype == TYPE_CONST)
    {
        KillFatherAndBrother(tree, node->left);
    }
}
void CalculateNeutralPow(DerTree* tree, DerNode* node)
{
    if (Rval == POW_NULL && Rtype == TYPE_CONST)
    {
        KillYourselfAndChildren(tree, node, POW_NEUT);
    }
    else if (Rval == POW_NEUT && Rtype == TYPE_CONST)
    {
        KillFatherAndBrother(tree, node->left);
    }
    else if (Lval == POW_NULL && Ltype == TYPE_CONST)
    {
        KillYourselfAndChildren(tree, node, POW_NULL);
    }
    else if (Lval == POW_NEUT && Ltype == TYPE_CONST)
    {
        KillYourselfAndChildren(tree, node, POW_NEUT);
    }
}

#undef Rval
#undef Lval
#undef Rtype
#undef Ltype
