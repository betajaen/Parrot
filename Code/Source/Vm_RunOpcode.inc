/* This is an automatically generated file */

WORD Vm_RunOpcode(OPCODE opcode)
{
  switch(opcode & 0x3F)
  {
    case VM_OP_ADD:
    {
      LONG arg_V = Vm_u10tou16(opcode);
      LONG l, r;
      r = Vm_Pop();
      l = Vm_Pop();
      Vm_SetVar(arg_V, l + r);
    }
    return 1;
    case VM_OP_ADDS:
    {
      LONG l, r;
      r = Vm_Pop();
      l = Vm_Pop();
      Vm_Push(l + r);
    }
    return 1;
    case VM_OP_BEQ:
    {
      ULONG arg_A = Vm_s10tos16(opcode);
      if ((Vm_LastCmp & VM_CMP_EQUALS) == VM_CMP_EQUALS)
      {
        return arg_A;
      }
    }
    return 1;
    case VM_OP_BGE:
    {
      ULONG arg_A = Vm_s10tos16(opcode);
      if ((Vm_LastCmp & (VM_CMP_GT | VM_CMP_EQUALS)) != 0)
      {
        return arg_A;
      }
    }
    return 1;
    case VM_OP_BGT:
    {
      ULONG arg_A = Vm_s10tos16(opcode);
      if ((Vm_LastCmp & VM_CMP_GT) == VM_CMP_GT)
      {
        return arg_A;
      }
    }
    return 1;
    case VM_OP_BLE:
    {
      ULONG arg_A = Vm_s10tos16(opcode);
      if ((Vm_LastCmp & (VM_CMP_LT | VM_CMP_EQUALS)) != 0)
      {
        return arg_A;
      }
    }
    return 1;
    case VM_OP_BLT:
    {
      ULONG arg_A = Vm_s10tos16(opcode);
      if ((Vm_LastCmp & VM_CMP_LT) == VM_CMP_LT)
      {
        return arg_A;
      }
    }
    return 1;
    case VM_OP_BNE:
    {
      ULONG arg_A = Vm_s10tos16(opcode);
      if ((Vm_LastCmp & VM_CMP_EQUALS) == 0)
      {
        return arg_A;
      }
    }
    return 1;
    case VM_OP_BRA:
    {
      ULONG arg_A = Vm_s10tos16(opcode);
      return arg_A;
    }
    return 1;
    case VM_OP_CMP:
    {
      LONG arg_L = Vm_u10tou16(opcode);
      LONG l, r;
      r = Vm_Pop();
      l = Vm_GetVar(arg_L);
      Vm_Compare(l, r);
    }
    return 1;
    case VM_OP_CMPI:
    {
      LONG arg_L = Vm_u10tou16(opcode);
      LONG l, r;
      r = Vm_Pop();
      l = arg_L;
      Vm_Compare(l, r);
    }
    return 1;
    case VM_OP_CMPS:
    {
      LONG l, r;
      r = Vm_Pop();
      l = Vm_Pop();
      Vm_Compare(l, r);
    }
    return 1;
    case VM_OP_DEC:
    {
      LONG arg_V = Vm_u10tou16(opcode);
      Vm_DecVar(arg_V);
    }
    return 1;
    case VM_OP_DUP:
    {
      Vm_Push(Vm_Peek());
    }
    return 1;
    case VM_OP_INC:
    {
      LONG arg_V = Vm_u10tou16(opcode);
      Vm_IncVar(arg_V);
    }
    return 1;
    case VM_OP_LOADI:
    {
      LONG arg_V = Vm_u10tou16(opcode);
      Vm_Push(arg_V);
    }
    return 1;
    case VM_OP_LOADK:
    {
      LONG arg_C = Vm_u10tou16(opcode);
      LONG v;
      v = Vm_GetConstant(arg_C);
      Vm_Push(v);
    }
    return 1;
    case VM_OP_MUL:
    {
      LONG arg_V = Vm_u10tou16(opcode);
      LONG l, r;
      r = Vm_Pop();
      l = Vm_Pop();
      Vm_SetVar(arg_V, l * r);
    }
    return 1;
    case VM_OP_POP:
    {
      Vm_Pop();
    }
    return 1;
    case VM_OP_PUSHI:
    {
      LONG arg_V = Vm_u10tou16(opcode);
      Vm_Push(arg_V);
    }
    return 1;
    case VM_OP_SHL:
    {
      LONG arg_V = Vm_u10tou16(opcode);
      LONG l;
      l = Vm_Pop();
      Vm_Push(l << arg_V);
    }
    return 1;
    case VM_OP_SHR:
    {
      LONG arg_V = Vm_u10tou16(opcode);
      LONG l;
      l = Vm_Pop();
      Vm_Push(l >> arg_V);
    }
    return 1;
    case VM_OP_SUB:
    {
      LONG arg_V = Vm_u10tou16(opcode);
      LONG l, r;
      r = Vm_Pop();
      l = Vm_Pop();
      Vm_SetVar(arg_V, l - r);
    }
    return 1;
    case VM_OP_SUBS:
    {
      LONG l, r;
      r = Vm_Pop();
      l = Vm_Pop();
      Vm_Push(l - r);
    }
    return 1;
    case VM_OP_SYS:
    {
      LONG arg_F = Vm_u10tou16(opcode);
      LONG arg;
      arg = Vm_Pop();
      Parrot_SysCall(arg_F, arg);
    }
    return 1;
  }
}
