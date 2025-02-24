#include "Macro.h"

int main()
{
    int A = CLN2D_GEN_REPLACE_INT(replaceName_A);
    float B = CLN2D_GEN_REPLACE_FLOAT(replaceName_B);
    bool C = CLN2D_GEN_REPLACE_BOOL(replaceName_C);
    char D = CLN2D_GEN_REPLACE_CHAR(replaceName_D);
    const char* E = CLN2D_GEN_REPLACE_STRING(replaceName_E);
    return 0;
}