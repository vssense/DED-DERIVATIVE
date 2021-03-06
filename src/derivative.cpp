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
void CalculateNeutralLn (DerTree* tree, DerNode* node);
void CalculateNeutralExp(DerTree* tree, DerNode* node);

/////////////////////////////////
//Derivative
/////////////////////////////////
void     TakeDerivative       (DerTree* tree);
DerNode* Derivative           (DerTree* tree, DerNode* node);
DerNode* CopySubTree          (DerTree* tree, DerNode* node);
DerNode* SwitchBinOP          (DerTree* tree, DerNode* node);
DerNode* SwitchUnOP           (DerTree* tree, DerNode* node);
void     TakeDerivative       (DerTree* tree);
void     Taylor               (DerTree* tree, size_t order);
void     SetParents           (DerTree* tree);
void     SetParentsRecursively(DerTree* tree, DerNode* node);
bool     IsThereVariable      (DerTree* tree, DerNode* node);


int main(const int argc, char* argv[])
{
    DerTree* tree = GetTree(argc, argv);

    Taylor(tree, 8);
    // TakeDerivative(tree);
    // PrintExpression(tree);

    // TreeDump(tree);

    Destruct(tree);
    Delete(tree);

    return 0;
}

DerNode* TakeDerivativeWithNewTree(DerTree* tree)
{
    assert(tree);

    DerNode* tmp1 = tree->root;
    DerNode* tmp2 = Derivative(tree, tree->root);
    tree->root    = tmp2;

    SetParents(tree);

    tree->root = tmp1;
    return tmp2;
}

void TakeDerivative(DerTree* tree)
{
    assert(tree);

    DerNode* tmp = Derivative(tree, tree->root);

    DestructNodes(tree, tree->root);

    tree->root = tmp;
    SetParents(tree);

    Simplify(tree);
}

void SetParents(DerTree* tree)
{
    tree->root->parent = tree->nil;
    SetParentsRecursively(tree, tree->root);
}

void SetParentsRecursively(DerTree* tree, DerNode* node)
{
    if (node->right != tree->nil)
    {
        node->right->parent = node;
        SetParentsRecursively(tree, node->right);
    }

    if (node->left  != tree->nil)
    {
        node->left->parent = node;
        SetParentsRecursively(tree, node->left);
    }
}

#define dR    Derivative(tree, node->right)
#define dL    Derivative(tree, node->left)
#define cR    CopySubTree(tree, node->right)
#define cL    CopySubTree(tree, node->left)
#define COPY  CopySubTree(tree, node)

#define ADD(left, right) ConstructNode(TYPE_BIN_OP, { .op = OP_ADD  }, left, right)
#define SUB(left, right) ConstructNode(TYPE_BIN_OP, { .op = OP_SUB  }, left, right)
#define MUL(left, right) ConstructNode(TYPE_BIN_OP, { .op = OP_MUL  }, left, right)
#define DIV(left, right) ConstructNode(TYPE_BIN_OP, { .op = OP_DIV  }, left, right)
#define POW(left, right) ConstructNode(TYPE_BIN_OP, { .op = OP_POW  }, left, right)
#define EXP(right)       ConstructNode(TYPE_UN_OP,  { .op = OP_EXP  }, tree->nil, right)
#define LN(right)        ConstructNode(TYPE_UN_OP,  { .op = OP_LN   }, tree->nil, right)
#define CONST(NUM)       ConstructNode(TYPE_CONST,  { .number = NUM }, tree->nil, tree->nil)
#define VAR(x)           ConstructNode(TYPE_VAR,    { .var = x      }, tree->nil, tree->nil)

DerNode* Derivative(DerTree* tree, DerNode* node)
{
    assert(tree);
    assert(node);

    if (node == tree->nil) return tree->nil;

    switch (node->type)
    {
        case TYPE_CONST :
        {
            return CONST(0);
        }
        case TYPE_VAR :
        {
            return CONST(1);
        }
        case TYPE_BIN_OP :
        {
            return SwitchBinOP(tree, node);
        }
        case TYPE_UN_OP :
        {
            return SwitchUnOP(tree, node);
        }
        default :
        {
            printf("Error type was discovored while taking taking a derivative\nline = %d\n", __LINE__);
            return tree->nil;
            break;
        }
    }
}

DerNode* CopySubTree(DerTree* tree, DerNode* node)
{
    assert(tree);
    assert(node);

    if (node == tree->nil)
    {
        return tree->nil;
    }

    return ConstructNode(node->type, node->value, cL, cR);
}

DerNode* SwitchBinOP(DerTree* tree, DerNode* node)
{
    assert(tree);
    assert(node);

    switch (node->value.op)
    {
        case OP_ADD :
        {
            return ADD(dL, dR);
        }
        case OP_SUB :
        {
            return SUB(dL, dR);
        }
        case OP_MUL :
        {
            return ADD(MUL(dL, cR), MUL(cL, dR));
        }
        case OP_DIV :
        {
            return DIV(SUB(MUL(dL, cR), MUL(cL, dR)), MUL(cR, cR));
        }
        case OP_POW :
        {
            bool IsVarInRight = IsThereVariable(tree, node->right);
            bool IsVarInLeft  = IsThereVariable(tree, node->left);
            
            if (IsVarInLeft && IsVarInRight)
            {
                return Derivative(tree, EXP(MUL(cR, LN(cL))));
            }
            else if (IsVarInLeft)
            {
                return MUL(MUL(cR, POW(cL, SUB(cR, CONST(1)))), dL);
            } 
            else if (IsVarInRight)
            {
                return MUL(MUL(COPY, LN(cL)), dR);
            }
            else
            {
                return CONST(0); 
            }
        }
        default :
        {
            printf("Error: unknown binary operation, line %d\n", __LINE__);
            return tree->nil;
        }
    }
}

#define COS(right)  ConstructNode(TYPE_UN_OP, { .op = OP_COS  }, tree->nil, right)
#define SIN(right)  ConstructNode(TYPE_UN_OP, { .op = OP_SIN  }, tree->nil, right)
#define SQRT(right) ConstructNode(TYPE_UN_OP, { .op = OP_SQRT }, tree->nil, right)

DerNode* SwitchUnOP(DerTree* tree, DerNode* node)
{
    assert(tree);
    assert(node);

    switch (node->value.op)
    {
        case OP_SIN :
        {
            return MUL(COS(cR), dR);
        }
        case OP_COS :
        {
            return MUL(MUL(SIN(cR), dR), CONST(-1));
        }
        case OP_TAN :
        {
            return MUL(DIV(CONST(1), POW(COS(cR), CONST(2))), dR);
        }
        case OP_CTG :
        {
            return MUL(DIV(CONST(-1), POW(SIN(cR), CONST(2))), dR);
        }
        case OP_SQRT :
        {
           return MUL(DIV(CONST(1), MUL(CONST(2), SQRT(cR))), dR); 
        }
        case OP_LN :
        {
            return MUL(DIV(CONST(1), cR), dR);
        }
        case OP_EXP :
        {
            return MUL(COPY, dR);
        }
        default :
        {
            printf("Error: unknown unary operation, line %d\n", __LINE__);
            return tree->nil;
        }
    }
}

void SetX(DerTree* tree, DerNode* node, double value)
{
    if (node == tree->nil)
    {
        return;
    }

    if (node->type == TYPE_VAR && node->value.var == 'x')
    {
        node->type = TYPE_CONST;
        node->value.number = value;
    }

    SetX(tree, node->right, value);
    SetX(tree, node->left,  value);
}

void Taylor(DerTree* tree, size_t order)
{
    size_t factorial = 1;

    DerTree* taylor_tree = CopyTree(tree);
    SetX(taylor_tree, taylor_tree->root, 0);
    Simplify(taylor_tree);

    for (size_t i = 1; i <= order; ++i)
    {
        if (i != 0)
        {
            factorial *= i;
        }

        TakeDerivative(tree);
        DerTree* tmp = CopyTree(tree);
        SetX(tmp, tmp->root, 0);
        Simplify(tmp);

        taylor_tree->root = ADD(taylor_tree->root, MUL(DIV(POW(VAR('x'), CONST(i)), CONST(factorial)), CONST(tmp->root->value.number)));
        SetParents(taylor_tree);
        Simplify(taylor_tree);
    }

    PrintExpression(taylor_tree);
}

#undef dR
#undef dL
#undef cR
#undef cL
#undef COPY 
#undef ADD
#undef SUB
#undef MUL
#undef DIV
#undef SIN
#undef COS
#undef SQRT
#undef CONST

void Simplify(DerTree* tree)
{
    assert(tree);

    bool sth_has_changed = true;

    while (sth_has_changed)
    {
        sth_has_changed = false;
        CalculateConsts(tree, tree->root, &sth_has_changed);
        CalculateNeutralOP(tree, tree->root, &sth_has_changed);
    }
}

#define Rval  node->right->value.number
#define Lval  node->left->value.number
#define VAL   node->value.number
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
    assert(tree);
    assert(node);

    switch (node->value.op)
    {
        case OP_ADD :
        {
            VAL = Lval + Rval;
            break;
        }
        case OP_SUB :
        {
            VAL = Lval - Rval;
            break;
        }
        case OP_MUL :
        {
            VAL = Lval * Rval;
            break;
        }
        case OP_DIV :
        {
            if (Rval != 0)
            {
                VAL = Lval / Rval;
            } 
            else
            {
                node->type = NODE_ERROR;
            }
            break;
        }
        case OP_POW :
        {
            VAL = pow(Lval, Rval);
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
    assert(tree);
    assert(node);

    switch (node->value.op)
    {
        case OP_SIN :
        {
            VAL = sin(Rval);
            break;
        }
        case OP_COS :
        {
            VAL = cos(Rval);
            break;
        }
        case OP_TAN :
        {
            VAL = tan(Rval);
            break;
        }
        case OP_CTG :
        {
            if (Rval != 0)
            {
                VAL = 1 / tan(Rval);
            }
            break;
        }
        case OP_SQRT :
        {
            if (Rval >= 0)
            {
                VAL = sqrt(Rval);
            }
            else
            {
                node->type = NODE_ERROR;
            }
            break;
        }
        case OP_LN :
        {
            if (Rval >= 0)
            {
                VAL = log(Rval);
            }
            else
            {
                node->type = NODE_ERROR;
            }
            break;
        }
        case OP_EXP :
        {
            VAL = exp(Rval);
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

    if (node == tree->nil)
    {
        return;
    }

    CalculateNeutralOP(tree, node->right, sth_has_changed);
    CalculateNeutralOP(tree, node->left,  sth_has_changed);

    if (node->type == TYPE_BIN_OP)
    {
        switch (node->value.op)
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
            case OP_LN :
            {
                CalculateNeutralLn(tree, node);
                break;
            }
            case OP_EXP :
            {
                CalculateNeutralExp(tree, node);
                break;
            }
            default :
            {
                printf("Error in calculating neutrals : unknown value\nline = %d", __LINE__);
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
    assert(tree);
    assert(node);

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
    assert(tree);
    assert(node);
    
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
    assert(tree);
    assert(node);
    
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
    assert(tree);
    assert(node);
    
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

void CalculateNeutralLn(DerTree* tree, DerNode* node)
{
    assert(tree);
    assert(node);

    if (Rval == LN_NEUT)
    {
        KillYourselfAndChildren(tree, node, LN_NULL);
    }
}

void CalculateNeutralExp(DerTree* tree, DerNode* node)
{
    assert(tree);
    assert(node);

    if (Rval == EXP_NULL)
    {
        KillYourselfAndChildren(tree, node, EXP_NEUT);
    }
}

#undef Rval
#undef Lval
#undef VAL
#undef Rtype
#undef Ltype

bool IsThereVariable(DerTree* tree, DerNode* node)
{
    if (node == tree->nil) return false;

    if (node->type == TYPE_VAR) return true;

    if (IsThereVariable(tree, node->left)) return true;

    return IsThereVariable(tree, node->right);
}
