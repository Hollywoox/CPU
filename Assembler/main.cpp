#include "assembler.h"

int main(int argc, char* argv[])
{
    Text text = {};
    AsmProgArray(&text, argv[1]);

    struct Label labels[LABELS_SIZE];

    Compile(&text, labels, 1);
    Compile(&text, labels, 2);

    Clear(&text);
    ClearLabels(labels);

    return 0;
}