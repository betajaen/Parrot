/**
    $Id: Script.c, 1.2 2020/11/12 16:35:00, betajaen Exp $

    Parrot - Point and Click Adventure Game Player
    ==============================================

    Copyright 2020 Robin Southern http://github.com/betajaen/parrot

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

#include <Parrot/Parrot.h>
#include <Parrot/Requester.h>
#include <Parrot/String.h>
#include <Parrot/Asset.h>
#include <Parrot/Arena.h>
#include <Parrot/Log.h>

void Parrot_SysCall(UWORD function, LONG argument);
STATIC struct VIRTUAL_MACHINE VirtualMachine[MAX_VIRTUAL_MACHINES];
struct VIRTUAL_MACHINE* Vm_Current;
LONG Vm_Globals[MAX_SCRIPT_GLOBALS];

STATIC VOID Vm_TickAll();
STATIC VOID Vm_Tick();

VOID Vm_Initialise()
{
  UWORD ii, jj;
  struct VIRTUAL_MACHINE* vm;

  for (ii = 0; ii < MAX_VIRTUAL_MACHINES; ii++)
  {
    vm = &VirtualMachine[ii];
    vm->vm_State = VM_STATE_END;
    vm->vm_PC = 0;
    vm->vm_Script = NULL;
    vm->vm_Timer = 0;
    vm->vm_StackHead = 0;
    vm->vm_Script = NULL;
    vm->vm_Opcodes = NULL;
    vm->vm_OpcodesLength = 0;

    for (jj = 0; jj < MAX_VM_STACK_SIZE; jj++)
    {
      vm->vm_Stack[jj] = 0;
    }

    for (jj = 0; jj < MAX_VM_VARIABLES; jj++)
    {
      vm->vm_Vars[jj] = 0;
    }
  }

  for (ii = 0; ii < MAX_SCRIPT_GLOBALS; ii++)
  {
    Vm_Globals[ii] = 0;
  }

}

VOID Vm_Shutdown()
{
}

STATIC VOID Vm_PrepareVm(struct VIRTUAL_MACHINE* vm, struct SCRIPT* script)
{
  UWORD ii;

  vm->vm_PC = 0;
  vm->vm_StackHead = 0;
  vm->vm_Stack[0] = 0;
  vm->vm_State = VM_STATE_RUN;
  vm->vm_Timer = 0;
  vm->vm_Constants = (ULONG*)&script->sc_Constants;
  vm->vm_Opcodes = ((OPCODE*) &script->sc_Opcodes);
  vm->vm_OpcodesLength = script->sc_NumOpcodes;
  vm->vm_Script = script;

  for (ii = 0; ii < MAX_VM_STACK_SIZE; ii++)
  {
    vm->vm_Stack[ii] = 0;
  }

  for (ii = 0; ii < MAX_VM_VARIABLES; ii++)
  {
    vm->vm_Vars[ii] = 0;
  }
}

STATIC VOID Vm_Recycle(struct VIRTUAL_MACHINE* vm)
{
  /*
    Free the script after it has ran.
  */
  if (Vm_Current->vm_Script != NULL)
  {
#if 0
    UnloadAsset(ArenaChapter, vm->vm_Script);
#endif
    Vm_Current->vm_Script = NULL;
  }

  vm->vm_State = VM_STATE_END;
}

STATIC VOID Vm_StartScript(struct SCRIPT* script)
{
  struct VIRTUAL_MACHINE* vm;
  UWORD ii;

  for (ii = 0; ii < MAX_VIRTUAL_MACHINES; ii++)
  {
    vm = &VirtualMachine[ii];

    if (vm->vm_State == VM_STATE_END)
    {
      Vm_PrepareVm(vm, script);
      return;
    }
  }
}

VOID Vm_RunScript(UWORD id)
{
  struct SCRIPT* script;
  UWORD archive;

#if 0
  archive = FindAssetArchive(id, CT_SCRIPT, CHUNK_FLAG_ARCH_ANY);
  script = LoadAssetT(struct SCRIPT, ArenaChapter, archive, CT_SCRIPT, id, CHUNK_FLAG_ARCH_ANY);
#endif
  Vm_StartScript(script);
}

VOID Vm_RunScriptNow(UWORD id, UWORD chapter, struct ARENA* arena)
{
  struct SCRIPT* script;
  struct VIRTUAL_MACHINE vm;

  InitStackVar(vm);

  script = (struct SCRIPT*)GetAsset(id, chapter, CT_SCRIPT, arena);

  Vm_PrepareVm(&vm, script);

  TRACEF("VM Script::RunNow. Script = %ld, NumOpcodes = %ld", script->as_Id, script->sc_NumOpcodes);

  if (vm.vm_OpcodesLength > 0)
  {
    while (TRUE)
    {
      if (vm.vm_State == VM_STATE_END)
        break;

      Vm_Current = &vm;
      Vm_Tick();
    }
  }

  Vm_Recycle(&vm);
  Vm_Current = NULL;
}


#define Vm_Branch(A) return A;

INLINE VOID Vm_Compare(LONG l, LONG r)
{
  UWORD cmp = VM_CMP_NONE;
  if (l == r)
    cmp |= VM_CMP_EQUALS;
  if (l < r)
    cmp |= VM_CMP_LT;
  else if (l > r)
    cmp |= VM_CMP_GT;

  Vm_Current->vm_Cmp = cmp;
}

INLINE VOID Vm_Push(LONG value)
{
  if (Vm_Current->vm_StackHead < MAX_VM_STACK_SIZE)
  {
    Vm_Current->vm_Stack[Vm_Current->vm_StackHead] = value;
    Vm_Current->vm_StackHead++;

    TRACEF("VM Stack::Push. Size=%ld, Top=%ld", Vm_Current->vm_StackHead, value);
  }
  else
  {
    /* Out of Stack space */
    PARROT_ERR(
      "Out of VM Stack Space!\n"
      "Reason: Stack space exceeded"
      PARROT_ERR_INT("Script Id"),
      Vm_Current->vm_Script->as_Id
    );
  }
}

INLINE LONG Vm_Pop()
{
  if (Vm_Current->vm_StackHead > 0)
  {
    Vm_Current->vm_StackHead--;
  }

  LONG value = Vm_Current->vm_Stack[Vm_Current->vm_StackHead];

  TRACEF("Vm Stack::Pop. Size=%ld, Top=%ld", Vm_Current->vm_StackHead, value);

  return value;
}

INLINE LONG Vm_Peek()
{
  LONG value = Vm_Current->vm_Stack[Vm_Current->vm_StackHead];

  TRACEF("Vm Stack::Peek. Size=%ld, Top=%ld", Vm_Current->vm_StackHead, value);

  return value;
}

INLINE VOID Vm_SetVar(UWORD index, LONG v)
{
  Vm_Current->vm_Vars[index] = v;
}

INLINE LONG Vm_GetVar(UWORD index)
{
  return Vm_Current->vm_Vars[index];
}

INLINE VOID Vm_IncVar(UWORD index)
{
  Vm_Current->vm_Vars[index]++;
}

INLINE VOID Vm_DecVar(UWORD index)
{
  Vm_Current->vm_Vars[index]--;
}

INLINE LONG Vm_GetConstant(UWORD index)
{
  return Vm_Current->vm_Constants[index];
}

INLINE LONG Vm_s10tos16(UWORD value)
{
  return value >> 6;
}

INLINE LONG Vm_u10tou16(UWORD value)
{
  return value >> 6;
}

#define Vm_LastCmp (Vm_Current->vm_Cmp)

#include <Parrot/Vm_Opcodes.h>
#include "Vm_RunOpcode.inc"

STATIC VOID Vm_TickAll()
{
  UWORD ii;

  for (ii = 0; ii < MAX_VIRTUAL_MACHINES; ii++)
  {
    Vm_Current = &VirtualMachine[ii];
    
    if (Vm_Current->vm_OpcodesLength == 0)
    {
      Vm_Current->vm_State = VM_STATE_STOPPING;
    }
    Vm_Tick();
    
  }
}

STATIC VOID Vm_Tick()
{
  switch (Vm_Current->vm_State)
  {
    case VM_STATE_STOPPING:
    {
      Vm_Recycle(Vm_Current);
    }
    break;
    case VM_STATE_RUN:
    {
      WORD pc, nextPc;
      OPCODE opcode;

      pc = Vm_Current->vm_PC;
      opcode = Vm_Current->vm_Opcodes[pc];

      TRACEF("VM Tick. Pc = %ld, Opcode = %s, Arg = %ld", pc, OpcodesStr[opcode & 0x3F], opcode >> 6);

      nextPc = pc + Vm_RunOpcode(opcode);

      if (nextPc >= 0 && nextPc < Vm_Current->vm_OpcodesLength)
      {
        Vm_Current->vm_PC = nextPc;
      }
      else
      {
        Vm_Current->vm_State = VM_STATE_END;
      }
    }
    break;
    case VM_STATE_TIMER:
    {
      if (Vm_Current->vm_Timer == 0)
      {
        Vm_Current->vm_State = VM_STATE_RUN;
      }
    }
    break;
  }
}
