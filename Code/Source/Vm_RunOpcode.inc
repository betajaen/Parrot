/* This is an automatically generated file */

PtSigned16 Vm_RunOpcode(OPCODE opcode)
{
  switch(opcode & 0x3F)
  {
    case VM_OP_ADD:
    {
      PtSigned32 arg_V = Vm_u10tou16(opcode);
      PtSigned32 l, r;
      r = Vm_Pop();
      l = Vm_Pop();
      Vm_SetVar(arg_V, l + r);
    }
    return 1;
    case VM_OP_ADDS:
    {
      PtSigned32 l, r;
      r = Vm_Pop();
      l = Vm_Pop();
      Vm_Push(l + r);
    }
    return 1;
    case VM_OP_BEQ:
    {
      PtUnsigned32 arg_A = Vm_s10tos16(opcode);
      if ((Vm_LastCmp & VM_CMP_EQUALS) == VM_CMP_EQUALS)
      {
        return arg_A;
      }
    }
    return 1;
    case VM_OP_BGE:
    {
      PtUnsigned32 arg_A = Vm_s10tos16(opcode);
      if ((Vm_LastCmp & (VM_CMP_GT | VM_CMP_EQUALS)) != 0)
      {
        return arg_A;
      }
    }
    return 1;
    case VM_OP_BGT:
    {
      PtUnsigned32 arg_A = Vm_s10tos16(opcode);
      if ((Vm_LastCmp & VM_CMP_GT) == VM_CMP_GT)
      {
        return arg_A;
      }
    }
    return 1;
    case VM_OP_BLE:
    {
      PtUnsigned32 arg_A = Vm_s10tos16(opcode);
      if ((Vm_LastCmp & (VM_CMP_LT | VM_CMP_EQUALS)) != 0)
      {
        return arg_A;
      }
    }
    return 1;
    case VM_OP_BLT:
    {
      PtUnsigned32 arg_A = Vm_s10tos16(opcode);
      if ((Vm_LastCmp & VM_CMP_LT) == VM_CMP_LT)
      {
        return arg_A;
      }
    }
    return 1;
    case VM_OP_BNE:
    {
      PtUnsigned32 arg_A = Vm_s10tos16(opcode);
      if ((Vm_LastCmp & VM_CMP_EQUALS) == 0)
      {
        return arg_A;
      }
    }
    return 1;
    case VM_OP_BRA:
    {
      PtUnsigned32 arg_A = Vm_s10tos16(opcode);
      return arg_A;
    }
    return 1;
    case VM_OP_CMP:
    {
      PtSigned32 arg_L = Vm_u10tou16(opcode);
      PtSigned32 l, r;
      r = Vm_Pop();
      l = Vm_GetVar(arg_L);
      Vm_Compare(l, r);
    }
    return 1;
    case VM_OP_CMPI:
    {
      PtSigned32 arg_L = Vm_u10tou16(opcode);
      PtSigned32 l, r;
      r = Vm_Pop();
      l = arg_L;
      Vm_Compare(l, r);
    }
    return 1;
    case VM_OP_CMPS:
    {
      PtSigned32 l, r;
      r = Vm_Pop();
      l = Vm_Pop();
      Vm_Compare(l, r);
    }
    return 1;
    case VM_OP_DEC:
    {
      PtSigned32 arg_V = Vm_u10tou16(opcode);
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
      PtSigned32 arg_V = Vm_u10tou16(opcode);
      Vm_IncVar(arg_V);
    }
    return 1;
    case VM_OP_LOADI:
    {
      PtSigned32 arg_V = Vm_u10tou16(opcode);
      Vm_Push(arg_V);
    }
    return 1;
    case VM_OP_LOADK:
    {
      PtSigned32 arg_C = Vm_u10tou16(opcode);
      PtSigned32 v;
      v = Vm_GetConstant(arg_C);
      Vm_Push(v);
    }
    return 1;
    case VM_OP_LOADPAL:
    {
      PtSigned32 arg_P = Vm_u10tou16(opcode);
      Api_LoadPalette(arg_P);
    }
    return 1;
    case VM_OP_LOADROOM:
    {
      PtSigned32 arg_R = Vm_u10tou16(opcode);
      Api_LoadRoom(arg_R);
    }
    return 1;
    case VM_OP_MUL:
    {
      PtSigned32 arg_V = Vm_u10tou16(opcode);
      PtSigned32 l, r;
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
      PtSigned32 arg_V = Vm_u10tou16(opcode);
      Vm_Push(arg_V);
    }
    return 1;
    case VM_OP_SHL:
    {
      PtSigned32 arg_V = Vm_u10tou16(opcode);
      PtSigned32 l;
      l = Vm_Pop();
      Vm_Push(l << arg_V);
    }
    return 1;
    case VM_OP_SHR:
    {
      PtSigned32 arg_V = Vm_u10tou16(opcode);
      PtSigned32 l;
      l = Vm_Pop();
      Vm_Push(l >> arg_V);
    }
    return 1;
    case VM_OP_SUB:
    {
      PtSigned32 arg_V = Vm_u10tou16(opcode);
      PtSigned32 l, r;
      r = Vm_Pop();
      l = Vm_Pop();
      Vm_SetVar(arg_V, l - r);
    }
    return 1;
    case VM_OP_SUBS:
    {
      PtSigned32 l, r;
      r = Vm_Pop();
      l = Vm_Pop();
      Vm_Push(l - r);
    }
    return 1;
    case VM_OP_SYS:
    {
      PtSigned32 arg_F = Vm_u10tou16(opcode);
      PtSigned32 arg;
      arg = Vm_Pop();
      Parrot_SysCall(arg_F, arg);
    }
    return 1;
  }
}
