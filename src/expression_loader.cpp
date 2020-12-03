#include "derivative.h"
#include "expression_loader.h"

void IgnoreSpaces(Buffer* buffer)
{
    assert(buffer);
    
    while (*buffer->str == ' ') 
    {
        buffer->str++;
    }
}

DerNode* GetAnswer(char* str)
{
    assert(str);
 
    Buffer buffer       = {};
    buffer.str          = str;
    buffer.original_str = str;
    buffer.status       = BUFFER_IS_OK;

    DerNode* root = GetExpression(&buffer);
    IgnoreSpaces(&buffer);

    if (*buffer.str != '\0' && *buffer.str != '\n') 
    {
        buffer.status = ENDING_ERR;
        SyntaxError(&buffer);
        return nullptr;
    }

    return root;
}

DerNode* GetExpression(Buffer* buffer)
{
    assert(buffer);
 
    DerNode* result = GetTerm(buffer);

    while (*buffer->str == '+' || *buffer->str == '-')
    {
        bool is_plus = (*buffer->str == '+');
        buffer->str++;

        DerNode* value = GetTerm(buffer);

        if (is_plus) result = ConstructNode(TYPE_BIN_OP, { .op = OP_ADD }, result, value);
        else         result = ConstructNode(TYPE_BIN_OP, { .op = OP_SUB }, result, value);

        IgnoreSpaces(buffer);
    }

    return result;
}

DerNode* GetTerm(Buffer* buffer)
{
    assert(buffer);
 
    DerNode* result = GetPower(buffer);
    IgnoreSpaces(buffer);
    
    while (*buffer->str == '*' || *buffer->str == '/')
    {
        bool is_mul = (*buffer->str == '*');
        buffer->str++;

        DerNode* value = GetPower(buffer);
        if (is_mul) result = ConstructNode(TYPE_BIN_OP, { .op = OP_MUL }, result, value);
        else        result = ConstructNode(TYPE_BIN_OP, { .op = OP_DIV }, result, value);
        IgnoreSpaces(buffer);
    }

    return result;
}

DerNode* GetPower(Buffer* buffer)
{
    assert(buffer);
 
    DerNode* result = GetUnaryFunction(buffer);
    IgnoreSpaces(buffer);
    
    while (*buffer->str == '^')
    {
        buffer->str++;

        result = ConstructNode(TYPE_BIN_OP, { .op = OP_POW }, result, GetUnaryFunction(buffer));
        IgnoreSpaces(buffer);
    }

    return result;
}

DerNode* GetUnaryFunction(Buffer* buffer)
{
    assert(buffer);

    IgnoreSpaces(buffer);

    if (*buffer->str != '(' && isalpha(*buffer->str) && !strspn(buffer->str, VARIABLES))
    {
        for (int i = 0; i < NUM_UNARY_OP; ++i)
        {
            if (strncmp(buffer->str, UNARY_OP[i], strlen(UNARY_OP[i])) == 0)
            {
                buffer->str += strlen(UNARY_OP[i]);
                return ConstructNode(TYPE_UN_OP, { .op = i}, nullptr, GetPrimaryExression(buffer));    
            }
        }
    }
    else
    {
        return GetPrimaryExression(buffer);
    }
}

DerNode* GetPrimaryExression(Buffer* buffer)
{
    assert(buffer);

    IgnoreSpaces(buffer);
    if (*buffer->str == '(')
    {
        buffer->str++;
        DerNode* result = GetExpression(buffer);
        IgnoreSpaces(buffer);

        if (*buffer->str != ')')
        {
            buffer->status = BRACKET_ERR;
            SyntaxError(buffer);
            return nullptr;
        }

        buffer->str++;
        IgnoreSpaces(buffer);

        return result;
    }
    else
    {
        return GetNumberOrVar(buffer);
    }
}

DerNode* GetNumberOrVar(Buffer* buffer)
{
    assert(buffer);
    
    double value = 0;
    size_t start = 0;
    size_t end   = 0;

    if (!sscanf(buffer->str, "%n%lf%n", &start, &value, &end))
    {
        if (strspn(buffer->str, VARIABLES))
        {
            char variable = *buffer->str;
            buffer->str++;
            return ConstructNode(TYPE_VAR, { .var = variable }, nullptr, nullptr);
        }
        buffer->status = GET_NUMBER_ERR;
        SyntaxError(buffer);
        return nullptr;
    }

    buffer->str += end - start;
    return ConstructNode(TYPE_CONST, { .number = value }, nullptr, nullptr);
}

void PrintError(Buffer* buffer)
{
    assert(buffer);
    
    switch (buffer->status)
    {
        case BUFFER_IS_OK :
        {
            printf("Error was called, but not detected\n");
            break;
        }
        case GET_NUMBER_ERR :
        {
            printf("Empty number\n");
            break;
        }
        case ENDING_ERR :
        {
            printf("Unknown symbol in the end\n");
            break;
        }
        case BRACKET_ERR :
        {
            printf("missed ')'\n");
            break;
        }
        default :
        {
            printf("Unknown error\n");
            break;
        }
    }
}

void SyntaxError(Buffer* buffer)
{
    assert(buffer);
    
    printf("Syntax error : ");
    PrintError(buffer);
    printf("%s\n", buffer->original_str);

    for (int i = 0; i < buffer->str - buffer->original_str; ++i)
    {
        printf(" ");
    }
    printf("^\n");
}
