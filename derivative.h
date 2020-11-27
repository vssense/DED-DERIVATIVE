#pragma once 

// #include "C:\Users\vssen\Desktop\TX\TXLib.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

typedef int ElemT;

enum NodeType
{
	TYPE_NIL    = 0,
	TYPE_CONST  = 1,
	TYPE_VAR    = 2,
	TYPE_BIN_OP = 3,
	TYPE_UN_OP  = 4,
	NODE_ERROR  = 5
};

enum BinaryOperators
{
	OP_ADD = 0,
	OP_SUB = 1,
	OP_MUL = 2,
	OP_DIV = 3,
	OP_POW = 4,
};

static const char* BINARY_OP[] = {"+", "-", "*", "/", "^"};
static const size_t NUM_BINARY_OP = 5;

enum UnaryOperators
{
	OP_SIN  = 0,
	OP_COS  = 1,
	OP_TG   = 2,
	OP_CTG  = 3,
	OP_SQRT = 4,
    OP_LN   = 5,
    OP_EXP  = 6
};

static const char* UNARY_OP[] = {"sin", "cos", "tg", "ctg", "sqrt", "ln", "exp"};
static const size_t NUM_UNARY_OP = 7;

static const size_t NO_MATCHES = 3232;

enum Variables
{
	VAR_X = 'x',
	VAR_Y = 'y'
};

static const char* VARIABLES = "xy";

enum NeutralElems
{
	ADD_NEUT = 0,
	SUB_NEUT = 0,
	MUL_NULL = 0,
	MUL_NEUT = 1,
	DIV_NULL = 0,
	DIV_NEUT = 1,
	POW_NULL = 0,
	POW_NEUT = 1,
    LN_NEUT  = 1,
    LN_NULL  = 0,
    EXP_NULL = 0,
    EXP_NEUT = 1
};

enum BreakElems
{
	MUL_BREAK = 0,
	DIV_BREAK = 0
};

struct DerNode
{
	ElemT value = 0;

	NodeType type = TYPE_CONST;

	DerNode* left  = nullptr; 
	DerNode* right = nullptr;

	DerNode* parent = nullptr; 
};

struct DerTree
{
	DerNode* root = nullptr;

	DerNode* nil = nullptr;
};


DerTree* GetTree                (const int argc, char* argv[]);
DerNode* ConstructNode          (NodeType type, ElemT value, DerNode* left, DerNode* right);
void     TreeDump               (DerTree* tree);
void     PrintExpression        (DerTree* tree);
void     Destruct               (DerTree* tree);
void     DestructNode           (DerTree* tree, DerNode* node);
void     DestructNodes          (DerTree* tree, DerNode* node);
void     KillChildren           (DerTree* tree, DerNode* node);
void     KillYourselfAndChildren(DerTree* tree, DerNode* node, ElemT new_value);
void     KillFatherAndBrother   (DerTree* tree, DerNode* node);
void     Delete                 (DerTree* tree);
