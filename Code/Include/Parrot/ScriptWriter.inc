/* This is an automatically generated file */

/*
  add
  Pop last two values from stack and add them together. Result is in vars
  Args:
    arg_V = Var
  Stack:
    0 = LHS
    -1 = RHS
*/
VOID Vm_Write_add(OPCODE* script, UWORD arg_V)
{
  *script++ =  VM_OP_ADD | Vm_u16tou10(arg_V);
}


/*
  adds
  Pop last two values from stack and add them together. Result is in stack
  Stack:
    0 = LHS
    -1 = RHS
*/
VOID Vm_Write_adds(OPCODE* script)
{
  *script++ =  VM_OP_ADDS;
}


/*
  beq
  Branch to address if equals
  Args:
    arg_A = Address
*/
VOID Vm_Write_beq(OPCODE* script, WORD arg_A)
{
  *script++ =  VM_OP_BEQ | Vm_s16tos10(arg_A);
}


/*
  bge
  Branch to address if greater than or equals
  Args:
    arg_A = Address
*/
VOID Vm_Write_bge(OPCODE* script, WORD arg_A)
{
  *script++ =  VM_OP_BGE | Vm_s16tos10(arg_A);
}


/*
  bgt
  Branch to address if greater than
  Args:
    arg_A = Address
*/
VOID Vm_Write_bgt(OPCODE* script, WORD arg_A)
{
  *script++ =  VM_OP_BGT | Vm_s16tos10(arg_A);
}


/*
  ble
  Branch to address if less than or equals
  Args:
    arg_A = Address
*/
VOID Vm_Write_ble(OPCODE* script, WORD arg_A)
{
  *script++ =  VM_OP_BLE | Vm_s16tos10(arg_A);
}


/*
  blt
  Branch to address if less than
  Args:
    arg_A = Address
*/
VOID Vm_Write_blt(OPCODE* script, WORD arg_A)
{
  *script++ =  VM_OP_BLT | Vm_s16tos10(arg_A);
}


/*
  bne
  Branch to address not equals
  Args:
    arg_A = Address
*/
VOID Vm_Write_bne(OPCODE* script, WORD arg_A)
{
  *script++ =  VM_OP_BNE | Vm_s16tos10(arg_A);
}


/*
  bra
  Branch to address always
  Args:
    arg_A = Address
*/
VOID Vm_Write_bra(OPCODE* script, WORD arg_A)
{
  *script++ =  VM_OP_BRA | Vm_s16tos10(arg_A);
}


/*
  cmp
  Compare variable to last item on stack
  Args:
    arg_L = LHS
  Stack:
    0 = RHS
*/
VOID Vm_Write_cmp(OPCODE* script, UWORD arg_L)
{
  *script++ =  VM_OP_CMP | Vm_u16tou10(arg_L);
}


/*
  cmpi
  Compare variable to last item to an immediate on stack
  Args:
    arg_L = LHS
  Stack:
    0 = RHS
*/
VOID Vm_Write_cmpi(OPCODE* script, UWORD arg_L)
{
  *script++ =  VM_OP_CMPI | Vm_u16tou10(arg_L);
}


/*
  cmps
  Compare two stack values together
  Stack:
    0 = LHS
    -1 = RHS
*/
VOID Vm_Write_cmps(OPCODE* script)
{
  *script++ =  VM_OP_CMPS;
}


/*
  dec
  Decrease variable by 1
  Args:
    arg_V = Var
*/
VOID Vm_Write_dec(OPCODE* script, UWORD arg_V)
{
  *script++ =  VM_OP_DEC | Vm_u16tou10(arg_V);
}


/*
  dup
  Duplicate the stack value
*/
VOID Vm_Write_dup(OPCODE* script)
{
  *script++ =  VM_OP_DUP;
}


/*
  inc
  Increase variable by 1
  Args:
    arg_V = Var
*/
VOID Vm_Write_inc(OPCODE* script, UWORD arg_V)
{
  *script++ =  VM_OP_INC | Vm_u16tou10(arg_V);
}


/*
  loadi
  Load a 10-bit signed value and place it onto the stack
  Args:
    arg_V = Value
*/
VOID Vm_Write_loadi(OPCODE* script, UWORD arg_V)
{
  *script++ =  VM_OP_LOADI | Vm_u16tou10(arg_V);
}


/*
  loadk
  Load a constant and place it onto the stack
  Args:
    arg_C = Constant
*/
VOID Vm_Write_loadk(OPCODE* script, UWORD arg_C)
{
  *script++ =  VM_OP_LOADK | Vm_u16tou10(arg_C);
}


/*
  mul
  Pop last two values from stack and add them together. Result is in vars
  Args:
    arg_V = Var
  Stack:
    0 = LHS
    -1 = RHS
*/
VOID Vm_Write_mul(OPCODE* script, UWORD arg_V)
{
  *script++ =  VM_OP_MUL | Vm_u16tou10(arg_V);
}


/*
  pop
  Pop stack value and discard
*/
VOID Vm_Write_pop(OPCODE* script)
{
  *script++ =  VM_OP_POP;
}


/*
  pushi
  Push 10-bit signed value on the stack
  Args:
    arg_V = Variable
*/
VOID Vm_Write_pushi(OPCODE* script, UWORD arg_V)
{
  *script++ =  VM_OP_PUSHI | Vm_u16tou10(arg_V);
}


/*
  shl
  Pop a value. Left Shift it and push the result on the stack
  Args:
    arg_V = Var
  Stack:
    0 = LHS
*/
VOID Vm_Write_shl(OPCODE* script, UWORD arg_V)
{
  *script++ =  VM_OP_SHL | Vm_u16tou10(arg_V);
}


/*
  shr
  Pop a value. Right Shift it and push the result on the stack
  Args:
    arg_V = Var
  Stack:
    0 = LHS
*/
VOID Vm_Write_shr(OPCODE* script, UWORD arg_V)
{
  *script++ =  VM_OP_SHR | Vm_u16tou10(arg_V);
}


/*
  sub
  Pop last two values from stack and subtract them together. Result is in vars
  Args:
    arg_V = Var
  Stack:
    0 = LHS
    -1 = RHS
*/
VOID Vm_Write_sub(OPCODE* script, UWORD arg_V)
{
  *script++ =  VM_OP_SUB | Vm_u16tou10(arg_V);
}


/*
  subs
  Pop last two values from stack and subtract them together. Result is in stack.
  Stack:
    0 = LHS
    -1 = RHS
*/
VOID Vm_Write_subs(OPCODE* script)
{
  *script++ =  VM_OP_SUBS;
}


/*
  sys
  Perform a System Call, where the top of the stack is the argument.
  Args:
    arg_F = Function
  Stack:
    0 = Argument
*/
VOID Vm_Write_sys(OPCODE* script, UWORD arg_F)
{
  *script++ =  VM_OP_SYS | Vm_u16tou10(arg_F);
}


