#include "cpu.h"

int main(int argc, char* argv[])
{
    CPU proc = {};
    Ctor(&proc, 100, argv[1]);
    Run(&proc);
    Dtor(&proc);
    return 0;
}