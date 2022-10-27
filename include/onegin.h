#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>
#include <locale.h>
#include <ctype.h>

typedef struct{
    char *str;
    int size;
} string;

typedef struct{
    string* str_array;
    char* buf;

    int num_of_str;
    int num_of_bytes;
} Text;

/* counts the number of symbols(bytes) int the file */
void CountBytes(Text* txt, char* adr);

/* makes a buffer for original text */
void MakeBuf(Text* txt, FILE* fp);

/* counts the number of strings in the text */
void CountStr(Text* txt);

/* makes an array of strings */
void MakeStrArray(Text* txt);

/* compares two strings, returns 1 if the first string is bigger,
0 if they are equal, -1 if the second is bigger */
int Comp(const void* aPtr, const void* bPtr);

/* compares two strings in reverse way, returns 1 if the first string is bigger,
0 if they are equal, -1 if the second is bigger */
int CompReverse(const void* aPtr, const void* bPtr);

/* swaps two strings */
void Swap(string* str1, string* str2);

/* algorithm of sorting in o(n^2) */
void BubbleSort(Text* txt, int (*comp)(const void*, const void*));

/* algorithm of sorting in o(n*logn) */
void QuickSort(string* text, int first, int last, int (*comp)(const void*, const void*));

/* prints buffer */
void PrintBuf(Text* txt, FILE* fp);

/* prints the array of strings */
void PrintStrArray(Text* txt, FILE* fp);

/* clears the memory */
void Clear(Text* txt);