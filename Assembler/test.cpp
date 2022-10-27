#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/onegin.h"
#include "../include/stack.h"

struct Test
{
    int field = 123;
};


int main(int argc, char* argv[])
{
    const char* str = "in rttctctc";
    char cmd[10];

    sscanf(str, "%s", cmd);
    printf("%s\n", cmd);
    int result = sscanf(str + strlen(cmd), "%s", cmd);
    printf("%d\n", result);

    
    //strcpy(s, cmd);
    
    //printf("%d\n", sum);
    return 0;
}