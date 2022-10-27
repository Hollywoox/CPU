#include "cpu.h"

int main(int argc, char* argv[])
{
    CPU cpu = {};
    Ctor(&cpu, 100, argv[1]);
    for(int i = 0; i < 100; ++i)
    {
        cpu.RAM[i] = i * i;
    }
    Run(&cpu);
    Dtor(&cpu);
    return 0;
}