#include "onegin.h"


void CountBytes(Text* txt, const char* adr)
{
    struct stat data = {};
    stat(adr, &data);
    txt->num_of_bytes = (int)data.st_size;
}

void MakeBuf(Text* txt, FILE* fp)
{
    txt->buf = (char*)calloc(txt->num_of_bytes + 1, sizeof(char));
    assert(txt->buf != NULL);

    int read = fread(txt->buf, sizeof(char), txt->num_of_bytes, fp);
    if(read != txt->num_of_bytes) 
    {
        printf("%s() at %s(%d): error:\n", __FUNCTION__, __FILE__, __LINE__);
        printf("can't read from the file\n");
        abort();
    }

    *(txt->buf + read) = '\0';
}

void CountStr(Text* txt)
{
    int count = 1;
    for(int i = 0; i < txt->num_of_bytes; ++i){
        if(txt->buf[i] == '\n')
        {
            ++count;
            txt->buf[i] = '\0';
        }
    }

    txt->num_of_str = count;
}

void MakeStrArray(Text* txt)
{
    txt->str_array = (string*)calloc(txt->num_of_str, sizeof(string)); 
    assert(txt->str_array != NULL);

    char *begin_of_str = txt->buf;
    char *end_of_str = txt->buf;
    int  str_number = 0;

    for(int i = 0; i <= txt->num_of_bytes; ++i)
    {
        if(i == txt->num_of_bytes)
        {
           txt->str_array[str_number].str = begin_of_str;
           txt->str_array[str_number].size = end_of_str - begin_of_str; 
           break;
        }

        if((*end_of_str) == '\0')
        {
            txt->str_array[str_number].str = begin_of_str;
            txt->str_array[str_number].size = end_of_str - begin_of_str;
            
            ++str_number;
            begin_of_str = end_of_str + 1;
        }    

        end_of_str = end_of_str + 1;
    }
}

int Comp(const void* aPtr, const void* bPtr)
{
    int str1_size = ((string*)aPtr)->size; 
    if(str1_size == 0) return -1;

    int str2_size = ((string*)bPtr)->size;
    if(str2_size == 0) return 1;

    char* str1 = ((string*)aPtr)->str;
    char* str2 = ((string*)bPtr)->str;

    int i = 0;
    int j = 0;
    while(i < str1_size && j < str2_size)
    {
        while(!isalpha(*(str1 + i)) && i < str1_size - 1) ++i;
        while(!isalpha(*(str2 + j)) && j < str2_size - 1) ++j;

        if(i == str1_size - 1 && !isalpha(*(str1 + i))) return -1;
        if(j == str2_size - 1 && !isalpha(*(str2 + j))) return 1;

        if((int)(*(str1 + i)) > (int)(*(str2 + j))) return 1;
        if((int)(*(str1 + i)) < (int)(*(str2 + j))) return -1; 

        ++i;
        ++j;

        if(i == str1_size && j != str2_size) return -1;
        if(i != str1_size && j == str2_size) return 1;
    }

    return 0;
}

int CompReverse(const void* aPtr, const void* bPtr)
{
    int str1_size = ((string*)aPtr)->size; 
    if(str1_size == 0) return -1;

    int str2_size = ((string*)bPtr)->size;
    if(str2_size == 0) return 1;

    char* str1 = ((string*)aPtr)->str + str1_size - 1;
    char* str2 = ((string*)bPtr)->str + str2_size - 1;

    int i = 0;
    int  j = 0;
    while(i < str1_size && j < str2_size)
    {
        while(!isalpha(*(str1 - i)) && i < str1_size - 1) ++i;
        while(!isalpha(*(str2 - j)) && j < str2_size - 1) ++j;

        if(i == str1_size - 1 && !isalpha(*(str1 - i))) return -1;
        if(j == str2_size - 1 && !isalpha(*(str2 - j))) return 1;

        if((int)(*(str1 - i)) > (int)(*(str2 - j))) return 1;
        if((int)(*(str1 - i)) < (int)(*(str2 - j))) return -1; 

        ++i;
        ++j;

        if(i == str1_size && j != str2_size) return -1;
        if(i != str1_size && j == str2_size) return 1;
    }

    return 0;
}

void Swap(string* str1, string* str2)
{
    string exchange = *str1;

    *str1 = *str2;
    *str2 = exchange;
}

void BubbleSort(Text* txt, int (*comp)(const void*, const void*))
{
    for(int i = 0; i < txt->num_of_str; ++i)
    {
        for(int j = 1; j < txt->num_of_str - i; ++j)
        {
            if(comp(&txt->str_array[j - 1], &txt->str_array[j]) > 0) Swap(&txt->str_array[j - 1], &txt->str_array[j]);
        }
    }
}

void QuickSort(string* text, int first, int last, int (*comp)(const void*, const void*))
{
    int left, right, pivot;
    
    if(first < last)
    {
        pivot = first;
        left = first;
        right = last;

        while(left < right)
        {
            while(comp(&text[left], &text[pivot]) <= 0 && left < last) left++;
            while(comp(&text[right], &text[pivot]) > 0) right--;
            if(left < right)
            {
                Swap(&text[left], &text[right]);
            }
        }

        Swap(&text[pivot], &text[right]);
        QuickSort(text, first, right - 1, comp);
        QuickSort(text, right + 1, last, comp);
    }
}

void PrintBuf(Text* txt, FILE* fp)
{
    char *begin_of_str = txt->buf;
    char *end_of_str = txt->buf;

    for(int i = 0; i <= txt->num_of_bytes; ++i)
    {
        if(i == txt->num_of_bytes)
        {
            fprintf(fp, "%s", begin_of_str);
            fprintf(fp, "\n");
            break;
        }

        if((*end_of_str) == '\0')
        {
            fprintf(fp, "%s", begin_of_str);
            fprintf(fp, "\n");

            begin_of_str = end_of_str + 1;
        }

        end_of_str = end_of_str + 1;
    }
}

void PrintStrArray(Text* txt, FILE* fp)
{
    for(int i = 0; i < txt->num_of_str; ++i)
    {
        if(txt->str_array[i].size != 0)
        {
            fprintf(fp, "%s", txt->str_array[i].str);
            fprintf(fp, "\n");
        }
    }
}

void Clear(Text* txt)
{
    free(txt->str_array);
    txt->str_array = NULL;

    free(txt->buf);
    txt->buf = NULL;
}