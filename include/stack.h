#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#define INT
#define CANARY_PROT

#ifdef CANARY_PROT
#define ON_CANARY_PROT(...) __VA_ARGS__
#else
#define ON_CANARY_PROT(...) 0
#endif

#ifdef INT
    typedef int elem_t;
    static const char* elem_fmt = "%d";
    const int POISON = 0xDEADF00D;
    const int SHIFT = 2;
#endif

#ifdef DOUBLE
    typedef double elem_t;
    static const char* elem_fmt = "%f";
    const double POISON = -6789456123.435612;
    const int SHIFT = 1;
#endif

#ifdef CHAR
    typedef char elem_t;
    static const char* elem_fmt = "%c";
    const char POISON = '\0';
    const int SHIFT = 8;
#endif


#define StackCtor(stk, size) StackCtor_((stk), (size), #stk, __FUNCTION__, __FILE__, __LINE__)

#define StackDump(stk) StackDump_((stk), __FUNCTION__, __FILE__, __LINE__)

#define ASSERT_OK(stk) if(StackVerify(stk) != 0)                                    \
                       {                                                            \
                           StackDump_(stk, __FUNCTION__, __FILE__, __LINE__);       \
                           PrintError(stk);                                         \
                           abort();                                                 \
                       }


typedef unsigned long long canary;
#define CANARY 0xBADDED32DEADF00D

typedef struct 
{
    const char* name;
    const char* func;
    const char* file;
    int line;
} varinfo;

typedef struct
{
    canary left_protector;

    size_t size;
    size_t capacity;
    elem_t* data;
    int error;

    varinfo st_info;

    unsigned long st_hash;
    unsigned long data_hash;

    canary right_protector;
} Stack;


enum errors
{
    NULL_ST_POINTER = 1,
    NULL_DATA_POINTER = 2,
    BAD_CAPACITY = 4,
    BAD_SIZE = 8,
    BIG_SIZE = 16,
    RIGHT_ST_CANARY_DEAD = 32,
    LEFT_ST_CANARY_DEAD = 64,
    RIGHT_DATA_CANARY_DEAD = 128,
    LEFT_DATA_CANARY_DEAD = 256,
    //ELEM_POISONED = 512,
    WRONG_ST_HASH = 1024,
    WRONG_DATA_HASH = 2048
};


/* creates a stack with the necessary capacity */
void StackCtor_(Stack* stk, size_t capacity, const char* name, const char* func, const char* file, int line);

/* pushs a new element in the end of the stack */
void StackPush(Stack* stk, elem_t value);

/* deletes and returns the last element of the stack */
elem_t StackPop(Stack* stk);

/* increases the capacity of the stack */
void StackResizeUp(Stack* stk);

/* reduces the capacity of the stack */
void StackResizeDown(Stack* stk);

/* fill stack with poison */
void StackFill(Stack* stk);

/* looks for errors in the stack and returns the sum */
int StackVerify(Stack* stk);

/* prints the elements of the stack */
void StackPrint(Stack* stk, FILE* log);

/* prints the information about the stack */
void StackDump_(Stack* stk, const char* function, const char* file, int line);

/* prints the information about errors */
void PrintError(Stack* stk);

/* counts hash for the stack structure */
unsigned long HashFuncStack(Stack* stk);

/* counts hash for the data array of the stack */
unsigned long HashFuncData(Stack* stk);

/* deletes the whole stack and clears the memory */
void StackDtor(Stack* stk);