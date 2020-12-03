#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

enum BUF_STATUS
{
    BUFFER_IS_OK   = 0,
    GET_NUMBER_ERR = 1,
    ENDING_ERR     = 2,
    BRACKET_ERR    = 3,
};

struct Buffer
{
    char* str = nullptr;
    char* original_str = nullptr;

    BUF_STATUS status = BUFFER_IS_OK;
};

DerNode* GetAnswer          (char* str);
DerNode* GetNumberOrVar     (Buffer* buffer);
DerNode* GetExpression      (Buffer* buffer);
DerNode* GetPrimaryExression(Buffer* buffer);
DerNode* GetTerm            (Buffer* buffer);
DerNode* GetUnaryFunction   (Buffer* buffer);
DerNode* GetPower           (Buffer* buffer);
void     PrintError         (Buffer* buffer);
void     SyntaxError        (Buffer* buffer);
