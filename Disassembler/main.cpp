#include "disassembler.h"

int main(int argc, char* argv[])
{
    char* code = DisasmCodeArray(argv[1]);
    Disassemble(code);
    free(code);
    code = NULL;
    return 0;
}