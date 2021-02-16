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
inline void Op_add(PtUnsigned16 arg_V)
{
  ScriptEnforceSpace();
  ScriptData[ScriptSize++] =  VM_OP_ADD | ScriptU16tou10(arg_V);
}


/*
  adds
  Pop last two values from stack and add them together. Result is in stack
  Stack:
    0 = LHS
    -1 = RHS
*/
inline void Op_adds()
{
  ScriptEnforceSpace();
  ScriptData[ScriptSize++] =  VM_OP_ADDS;
}


/*
  beq
  Branch to address if equals
  Args:
    arg_A = Address
*/
inline void Op_beq(PtSigned16 arg_A)
{
  ScriptEnforceSpace();
  ScriptData[ScriptSize++] =  VM_OP_BEQ | ScriptS16tos10(arg_A);
}


/*
  bge
  Branch to address if greater than or equals
  Args:
    arg_A = Address
*/
inline void Op_bge(PtSigned16 arg_A)
{
  ScriptEnforceSpace();
  ScriptData[ScriptSize++] =  VM_OP_BGE | ScriptS16tos10(arg_A);
}


/*
  bgt
  Branch to address if greater than
  Args:
    arg_A = Address
*/
inline void Op_bgt(PtSigned16 arg_A)
{
  ScriptEnforceSpace();
  ScriptData[ScriptSize++] =  VM_OP_BGT | ScriptS16tos10(arg_A);
}


/*
  ble
  Branch to address if less than or equals
  Args:
    arg_A = Address
*/
inline void Op_ble(PtSigned16 arg_A)
{
  ScriptEnforceSpace();
  ScriptData[ScriptSize++] =  VM_OP_BLE | ScriptS16tos10(arg_A);
}


/*
  blt
  Branch to address if less than
  Args:
    arg_A = Address
*/
inline void Op_blt(PtSigned16 arg_A)
{
  ScriptEnforceSpace();
  ScriptData[ScriptSize++] =  VM_OP_BLT | ScriptS16tos10(arg_A);
}


/*
  bne
  Branch to address not equals
  Args:
    arg_A = Address
*/
inline void Op_bne(PtSigned16 arg_A)
{
  ScriptEnforceSpace();
  ScriptData[ScriptSize++] =  VM_OP_BNE | ScriptS16tos10(arg_A);
}


/*
  bra
  Branch to address always
  Args:
    arg_A = Address
*/
inline void Op_bra(PtSigned16 arg_A)
{
  ScriptEnforceSpace();
  ScriptData[ScriptSize++] =  VM_OP_BRA | ScriptS16tos10(arg_A);
}


/*
  cmp
  Compare variable to last item on stack
  Args:
    arg_L = LHS
  Stack:
    0 = RHS
*/
inline void Op_cmp(PtUnsigned16 arg_L)
{
  ScriptEnforceSpace();
  ScriptData[ScriptSize++] =  VM_OP_CMP | ScriptU16tou10(arg_L);
}


/*
  cmpi
  Compare variable to last item to an immediate on stack
  Args:
    arg_L = LHS
  Stack:
    0 = RHS
*/
inline void Op_cmpi(PtUnsigned16 arg_L)
{
  ScriptEnforceSpace();
  ScriptData[ScriptSize++] =  VM_OP_CMPI | ScriptU16tou10(arg_L);
}


/*
  cmps
  Compare two stack values together
  Stack:
    0 = LHS
    -1 = RHS
*/
inline void Op_cmps()
{
  ScriptEnforceSpace();
  ScriptData[ScriptSize++] =  VM_OP_CMPS;
}


/*
  dec
  Decrease variable by 1
  Args:
    arg_V = Var
*/
inline void Op_dec(PtUnsigned16 arg_V)
{
  ScriptEnforceSpace();
  ScriptData[ScriptSize++] =  VM_OP_DEC | ScriptU16tou10(arg_V);
}


/*
  dup
  Duplicate the stack value
*/
inline void Op_dup()
{
  ScriptEnforceSpace();
  ScriptData[ScriptSize++] =  VM_OP_DUP;
}


/*
  inc
  Increase variable by 1
  Args:
    arg_V = Var
*/
inline void Op_inc(PtUnsigned16 arg_V)
{
  ScriptEnforceSpace();
  ScriptData[ScriptSize++] =  VM_OP_INC | ScriptU16tou10(arg_V);
}


/*
  loadi
  Load a 10-bit signed value and place it onto the stack
  Args:
    arg_V = Value
*/
inline void Op_loadi(PtUnsigned16 arg_V)
{
  ScriptEnforceSpace();
  ScriptData[ScriptSize++] =  VM_OP_LOADI | ScriptU16tou10(arg_V);
}


/*
  loadk
  Load a constant and place it onto the stack
  Args:
    arg_C = Constant
*/
inline void Op_loadk(PtUnsigned16 arg_C)
{
  ScriptEnforceSpace();
  ScriptData[ScriptSize++] =  VM_OP_LOADK | ScriptU16tou10(arg_C);
}


/*
  loadpal
  Load a palette
  Args:
    arg_P = PaletteNum
*/
inline void Op_loadpal(PtUnsigned16 arg_P)
{
  ScriptEnforceSpace();
  ScriptData[ScriptSize++] =  VM_OP_LOADPAL | ScriptU16tou10(arg_P);
}


/*
  loadroom
  Load a room
  Args:
    arg_R = RoomNum
*/
inline void Op_loadroom(PtUnsigned16 arg_R)
{
  ScriptEnforceSpace();
  ScriptData[ScriptSize++] =  VM_OP_LOADROOM | ScriptU16tou10(arg_R);
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
inline void Op_mul(PtUnsigned16 arg_V)
{
  ScriptEnforceSpace();
  ScriptData[ScriptSize++] =  VM_OP_MUL | ScriptU16tou10(arg_V);
}


/*
  pop
  Pop stack value and discard
*/
inline void Op_pop()
{
  ScriptEnforceSpace();
  ScriptData[ScriptSize++] =  VM_OP_POP;
}


/*
  pushi
  Push 10-bit signed value on the stack
  Args:
    arg_V = Variable
*/
inline void Op_pushi(PtUnsigned16 arg_V)
{
  ScriptEnforceSpace();
  ScriptData[ScriptSize++] =  VM_OP_PUSHI | ScriptU16tou10(arg_V);
}


/*
  shl
  Pop a value. Left Shift it and push the result on the stack
  Args:
    arg_V = Var
  Stack:
    0 = LHS
*/
inline void Op_shl(PtUnsigned16 arg_V)
{
  ScriptEnforceSpace();
  ScriptData[ScriptSize++] =  VM_OP_SHL | ScriptU16tou10(arg_V);
}


/*
  shr
  Pop a value. Right Shift it and push the result on the stack
  Args:
    arg_V = Var
  Stack:
    0 = LHS
*/
inline void Op_shr(PtUnsigned16 arg_V)
{
  ScriptEnforceSpace();
  ScriptData[ScriptSize++] =  VM_OP_SHR | ScriptU16tou10(arg_V);
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
inline void Op_sub(PtUnsigned16 arg_V)
{
  ScriptEnforceSpace();
  ScriptData[ScriptSize++] =  VM_OP_SUB | ScriptU16tou10(arg_V);
}


/*
  subs
  Pop last two values from stack and subtract them together. Result is in stack.
  Stack:
    0 = LHS
    -1 = RHS
*/
inline void Op_subs()
{
  ScriptEnforceSpace();
  ScriptData[ScriptSize++] =  VM_OP_SUBS;
}


/*
  sys
  Perform a System Call, where the top of the stack is the argument.
  Args:
    arg_F = Function
  Stack:
    0 = Argument
*/
inline void Op_sys(PtUnsigned16 arg_F)
{
  ScriptEnforceSpace();
  ScriptData[ScriptSize++] =  VM_OP_SYS | ScriptU16tou10(arg_F);
}


