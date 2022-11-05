DEF_CMD(HLT, 0, 0,
{
    return;
})

DEF_CMD(PUSH, 1, 1,
{
    int cmd = *ip;
    ip     += 1;

    elem_t arg = 0;

    if(cmd & ARG_IMMED)
    {
        arg += *((elem_t*)ip);
        ip  += sizeof(elem_t);
    }
    if(cmd & ARG_REG)
    {
        arg += proc->Regs[*ip];
        ip  += 1;
    }
    if(cmd & ARG_RAM)
    {
        arg = proc->RAM[(int)arg];
    }

    StackPush(&proc->stk, arg);
})

DEF_CMD(POP, 2, 1,
{
    int cmd = *ip;
    ip     += 1;

    elem_t arg = 0;

    if(cmd & ARG_RAM)
    {
        if(cmd & ARG_IMMED)
        {
            arg += *((elem_t*)ip);
            ip  += sizeof(elem_t);
        }
        if(cmd & ARG_REG)
        {
            arg += proc->Regs[*ip];
            ip  += 1;
        }

        proc->RAM[(int)arg] = StackPop(&proc->stk);
    }
    else if(cmd & ARG_REG)
    {
        proc->Regs[*ip] = StackPop(&proc->stk);
        ip            += 1;
    }
    else
    {
        StackPop(&proc->stk);
    }
})

DEF_CMD(ADD, 3, 0, 
{
    StackPush(&proc->stk, StackPop(&proc->stk) + StackPop(&proc->stk));
    ip += 1;
})

DEF_CMD(SUB, 4, 0, 
{
    StackPush(&proc->stk, -StackPop(&proc->stk) + StackPop(&proc->stk));
    ip += 1;
})

DEF_CMD(MUL, 5, 0, 
{
    StackPush(&proc->stk, StackPop(&proc->stk) * StackPop(&proc->stk));
    ip += 1;
})

DEF_CMD(DIV, 6, 0, 
{
    elem_t last = StackPop(&proc->stk);
    StackPush(&proc->stk, StackPop(&proc->stk) / last);
    ip += 1;
})

DEF_CMD(DUP, 7, 0, 
{
    elem_t last = StackPop(&proc->stk);
    StackPush(&proc->stk, last);
    StackPush(&proc->stk, last);
    ip += 1;
})

DEF_CMD(OUT, 8, 0, 
{
    printf("out: ");
    printf(elem_fmt, StackPop(&proc->stk));
    printf("\n");
    ip += 1;
})

DEF_CMD(DUMP, 9, 0, 
{
    StackDump(&proc->stk);
    ip += 1;
})

DEF_CMD(IN, 10, 1, 
{
    int cmd = *ip;
    ip     += 1;

    elem_t arg      = 0;
    elem_t new_elem = 0;
    printf("in: ");
    scanf(elem_fmt, &new_elem);

    if(cmd & ARG_RAM)
    {
        if(cmd & ARG_IMMED)
        {
            arg += *((elem_t*)ip);
            ip  += sizeof(elem_t);
        }
        if(cmd & ARG_REG)
        {
            arg += proc->Regs[*ip];
            ip  += 1;
        }

        proc->RAM[(int)arg] = new_elem;
    }
    else if(cmd & ARG_REG)
    {
        proc->Regs[*ip] = new_elem;
        ip += 1;
    }
    else
    {
        StackPush(&proc->stk, new_elem);
    }
})

DEF_CMD(JMP, 11, 2, 
{
    ip     += 1;
    int adr = *((int*)ip);
    ip      = proc->code + adr;
})

DEF_CMD(JB, 12, 2, 
{
    ip     += 1;
    int adr = *((int*)ip);

    elem_t var1 = StackPop(&proc->stk);
    elem_t var2 = StackPop(&proc->stk);
    if(var1 < var2)
    {
        ip = proc->code + adr;
    }
    else
    {
        ip += sizeof(int);
    }

    StackPush(&proc->stk, var2);
    StackPush(&proc->stk, var1);
})

DEF_CMD(JBE, 13, 2, 
{
    ip     += 1;
    int adr = *((int*)ip);

    elem_t var1 = StackPop(&proc->stk);
    elem_t var2 = StackPop(&proc->stk);
    if(var1 <= var2)
    {
        ip = proc->code + adr;
    }
    else
    {
        ip += sizeof(int);
    }

    StackPush(&proc->stk, var2);
    StackPush(&proc->stk, var1);
})

DEF_CMD(JA, 14, 2, 
{
    ip     += 1;
    int adr = *((int*)ip);

    elem_t var1 = StackPop(&proc->stk);
    elem_t var2 = StackPop(&proc->stk);
    if(var1 > var2)
    {
        ip = proc->code + adr;
    }
    else
    {
        ip += sizeof(int);
    }

    StackPush(&proc->stk, var2);
    StackPush(&proc->stk, var1);
})

DEF_CMD(JAE, 15, 2, 
{
    ip     += 1;
    int adr = *((int*)ip);

    elem_t var1 = StackPop(&proc->stk);
    elem_t var2 = StackPop(&proc->stk);
    if(var1 >= var2)
    {
        ip = proc->code + adr;
    }
    else
    {
        ip += sizeof(int);
    }

    StackPush(&proc->stk, var2);
    StackPush(&proc->stk, var1);
})

DEF_CMD(JE, 16, 2, 
{
    ip     += 1;
    int adr = *((int*)ip);

    elem_t var1 = StackPop(&proc->stk);
    elem_t var2 = StackPop(&proc->stk);
    if(var1 == var2)
    {
        ip = proc->code + adr;
    }
    else
    {
        ip += sizeof(int);
    }

    StackPush(&proc->stk, var2);
    StackPush(&proc->stk, var1);
})

DEF_CMD(JNE, 17, 2, 
{
    ip     += 1;
    int adr = *((int*)ip);

    elem_t var1 = StackPop(&proc->stk);
    elem_t var2 = StackPop(&proc->stk);
    if(var1 != var2)
    {
        ip = proc->code + adr;
    }
    else
    {
        ip += sizeof(int);
    }

    StackPush(&proc->stk, var2);
    StackPush(&proc->stk, var1);
})

DEF_CMD(CALL, 18, 2, 
{
    ip     += 1;
    int adr = *((int*)ip);
    int ret_adr = ip + sizeof(int) - proc->code;
    StackPush(&proc->ret_stk, ret_adr);

    ip = proc->code + adr;
})

DEF_CMD(RET, 19, 0, 
{
    int ret_adr = StackPop(&proc->ret_stk);
    ip          = proc->code + ret_adr;
})