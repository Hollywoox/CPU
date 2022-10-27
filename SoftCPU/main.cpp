#include "cpu.h"

int main(int argc, char* argv[])
{
    CPU cpu = {};
    Ctor(&cpu, 100, argv[1]);
    Run(&cpu);
    Dtor(&cpu);
    return 0;
}