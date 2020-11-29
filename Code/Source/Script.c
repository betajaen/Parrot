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

STATIC LONG Globals[MAX_SCRIPT_GLOBALS];
STATIC struct VIRTUAL_MACHINE VirtualMachine[MAX_VIRTUAL_MACHINES];

STATIC VOID TickVirtualMachine(struct VIRTUAL_MACHINE* vm);

VOID GameLoadPalette(UWORD asset);

#define VM_PARAM_B(VM, WHERE) 0
#define VM_PARAM_W(VM, WHERE) 0
#define VM_PARAM_L(VM, WHERE) 0

VOID ScriptInitialise()
{
  UWORD ii,jj;
  struct VIRTUAL_MACHINE* vm;

  for (ii = 0; ii < MAX_VIRTUAL_MACHINES; ii++)
  {
    vm = &VirtualMachine[ii];
    vm->vm_State = VM_STATE_FREE;
    vm->vm_PC = 0;
    vm->vm_Script = NULL;
    vm->vm_Timer = 0;
    vm->vm_StackHead = 0;
    
    for (jj = 0; jj < MAX_VM_STACK_SIZE; jj++)
    {
      vm->vm_Stack[jj] = 0;
    }
  }

  for (ii = 0; ii < MAX_SCRIPT_GLOBALS; ii++)
  {
    Globals[ii] = 0;
  }

}

VOID ScriptShutdown()
{
}

STATIC VOID PrepareVm(struct VIRTUAL_MACHINE* vm, struct SCRIPT* script)
{
  vm->vm_PC = 0;
  vm->vm_Script = script;
  vm->vm_StackHead = 0;
  vm->vm_Stack[0] = 0;
  vm->vm_State = VM_STATE_RUN;
  vm->vm_Timer = 0;
  vm->vm_Constants = (ULONG*)&script->sc_Constants;
  vm->vm_Instructions = ((UBYTE*)&script->sc_Opcodes);
}

STATIC VOID StartScript(struct SCRIPT* script)
{
  struct VIRTUAL_MACHINE* vm;
  UWORD ii;

  for (ii = 0; ii < MAX_VIRTUAL_MACHINES; ii++)
  {
    vm = &VirtualMachine[ii];

    if (vm->vm_State == VM_STATE_FREE)
    {
      vm->vm_PC = 0;
      vm->vm_Script = script;
      vm->vm_StackHead = 0;
      vm->vm_State = VM_STATE_RUN;
      vm->vm_Timer = 0;
      vm->vm_Instructions = ((UBYTE*) (script)) + sizeof(struct SCRIPT);
    }
  }
}

VOID RunScript(UWORD id)
{
  struct SCRIPT* script;
  UWORD archive;

#if 0
  archive = FindAssetArchive(id, CT_SCRIPT, CHUNK_FLAG_ARCH_ANY);
  script = LoadAssetT(struct SCRIPT, ArenaChapter, archive, CT_SCRIPT, id, CHUNK_FLAG_ARCH_ANY);
#endif
  StartScript(script);
}

VOID RunScriptNow(UWORD id, UWORD chapter, struct ARENA* arena)
{
  struct SCRIPT* script;
  struct VIRTUAL_MACHINE vm;

  InitStackVar(vm);

  script = (struct SCRIPT*) GetAsset(id, chapter, CT_SCRIPT, arena);

  PrepareVm(&vm, script);

  while (TRUE)
  {
    if (vm.vm_State == VM_STATE_FREE)
      break;

    TickVirtualMachine(&vm);
  }

}

VOID TickVirtualMachines()
{
  struct VIRTUAL_MACHINE* vm;
  UWORD ii;

  for (ii = 0; ii < MAX_VIRTUAL_MACHINES; ii++)
  {
    vm = &VirtualMachine[ii];

    if (vm->vm_State != VM_STATE_FREE)
    {
      TickVirtualMachine(vm);
    }
    else
    {
      /*
        Free the script after it has ran.
       */
      if (vm->vm_Script != NULL)
      {
#if 0
        UnloadAsset(ArenaChapter, vm->vm_Script);
#endif
        vm->vm_Script = NULL;
      }
    }
  }
}

/*

    Opcodes

*/
#define DECL_OPCODE(NUM) \
  STATIC VOID Op_##NUM(struct VIRTUAL_MACHINE* vm, UWORD opcode)

#define VM_CONSTANT_MASK (MAX_CONSTANTS_PER_SCRIPT - 1);
#define VM_GLOBALS_MASK (MAX_SCRIPT_GLOBALS-1)

#define VM_ARG_S     (0) /* FUTURE */
#define VM_ARG_U     (0) /* FUTURE */
#define VM_CONST(X)  (0) /* FUTURE */

#define VM_PUSH(X)      /* FUTURE */
#define VM_POP       (0) /* FUTURE */
#define VM_TOP       (0) /* FUTURE */

#define VM_CMP_EQ     1
#define VM_CMP_LESS   2
#define VM_CMP_MORE   4

/* stop */
DECL_OPCODE(00)
{
  vm->vm_State = VM_STATE_STOP;
}

/* rem */
DECL_OPCODE(01)
{
  /* No Operation */
  vm->vm_PC++;
}

/* push */
DECL_OPCODE(02)
{
  LONG v;

  v = VM_ARG_S;
  VM_PUSH(v);
  vm->vm_PC++;
}

/* pushu */
DECL_OPCODE(03)
{
  LONG v;

  v = VM_ARG_U;
  VM_PUSH(v);
  vm->vm_PC++;
}

/* pushc */
DECL_OPCODE(04)
{
  LONG vi;

  vi = VM_ARG_U & VM_CONSTANT_MASK;
  vi = VM_CONST(vi);

  VM_PUSH(vi);
  vm->vm_PC++;
}

/* dup */
DECL_OPCODE(05)
{
  LONG v;

  v = VM_TOP;
  VM_PUSH(v);
  vm->vm_PC++;
}

/* dis */
DECL_OPCODE(06)
{
  LONG x;
  x = VM_POP;
  vm->vm_PC++;
}

/* RESERVED */
DECL_OPCODE(07)
{
  vm->vm_PC++;
}

/* FUTURE load */
DECL_OPCODE(08)
{
  vm->vm_PC++;
}

/* FUTURE save */
DECL_OPCODE(09)
{
  vm->vm_PC++;
}

/* gload */
DECL_OPCODE(0A)
{
  LONG vi;
  vi = VM_ARG_U & VM_GLOBALS_MASK;
  vi = Globals[vi];

  VM_PUSH(vi);
  vm->vm_PC++;
}

/* gsave */
DECL_OPCODE(0B)
{
  LONG v, i;
  v = VM_POP;
  i = VM_ARG_U & VM_GLOBALS_MASK;

  Globals[i] = v;
  vm->vm_PC++;
}

/* cmp */
DECL_OPCODE(0C)
{
  LONG r, l, c;

  r = VM_POP;
  l = VM_POP;

  c = 0;

  if (l == r)
    c |= VM_CMP_EQ;
  
  if (l < r)
    c |= VM_CMP_LESS;

  if (l > r)
    c |= VM_CMP_MORE;

  vm->vm_Cmp = c;
  vm->vm_PC++;
}

/* cmpc */
DECL_OPCODE(0D)
{
  LONG r, l, c;

  r = VM_ARG_S;
  l = VM_POP;

  c = 0;

  if (l == r)
    c |= VM_CMP_EQ;

  if (l < r)
    c |= VM_CMP_LESS;

  if (l > r)
    c |= VM_CMP_MORE;

  vm->vm_Cmp = c;
  vm->vm_PC++;
}

/* j */
DECL_OPCODE(0E)
{
  LONG a;

  a = VM_ARG_U;
  a <<= 2;

  vm->vm_PC = a;
}

/* je */
DECL_OPCODE(0F)
{
  LONG a;
  
  if (vm->vm_Cmp == VM_CMP_EQ)
  {
    a = VM_ARG_U;
    a <<= 1;
  }
  else
  {
    a = vm->vm_PC + 1;
  }
  
  vm->vm_PC = a;
}

/* jne */
DECL_OPCODE(10)
{
  LONG a;

  if (vm->vm_Cmp == 0)
  {
    a = VM_ARG_U;
    a <<= 1;
  }
  else
  {
    a = vm->vm_PC + 1;
  }

  vm->vm_PC = a;
}

/* jg */
DECL_OPCODE(11)
{
  LONG a;

  if (vm->vm_Cmp == VM_CMP_MORE)
  {
    a = VM_ARG_U;
    a <<= 1;
  }
  else
  {
    a = vm->vm_PC + 1;
  }

  vm->vm_PC = a;
}

/* jge */
DECL_OPCODE(12)
{
  LONG a;

  if ((vm->vm_Cmp & (VM_CMP_MORE | VM_CMP_EQ)) != 0)
  {
    a = VM_ARG_U;
    a <<= 1;
  }
  else
  {
    a = vm->vm_PC + 1;
  }

  vm->vm_PC = a;
}

/* jl */
DECL_OPCODE(13)
{
  LONG a;

  if (vm->vm_Cmp == VM_CMP_LESS)
  {
    a = VM_ARG_U;
    a <<= 1;
  }
  else
  {
    a = vm->vm_PC + 1;
  }

  vm->vm_PC = a;
}

/* jle */
DECL_OPCODE(14)
{
  LONG a;

  if ((vm->vm_Cmp & (VM_CMP_LESS | VM_CMP_EQ)) != 0)
  {
    a = VM_ARG_U;
    a <<= 1;
  }
  else
  {
    a = vm->vm_PC + 1;
  }

  vm->vm_PC = a;
}

/* add */
DECL_OPCODE(15)
{
  LONG lv, r;

  r = VM_POP;
  lv = VM_POP;
  
  lv += r;
  VM_PUSH(lv);
  vm->vm_PC++;
}

/* addq */
DECL_OPCODE(16)
{
  LONG lv, r;

  r = VM_ARG_S;
  lv = VM_POP;

  lv += r;
  VM_PUSH(lv);
  vm->vm_PC++;
}

/* sub */
DECL_OPCODE(17)
{
  LONG lv, r;

  r = VM_POP;
  lv = VM_POP;

  lv -= r;
  VM_PUSH(lv);
  vm->vm_PC++;
}

/* subq */
DECL_OPCODE(18)
{
  LONG lv, r;

  r = VM_ARG_S;
  lv = VM_POP;

  lv -= r;
  VM_PUSH(lv);
  vm->vm_PC++;
}

/* mul */
DECL_OPCODE(19)
{
  LONG lv, r;

  r = VM_POP;
  lv = VM_POP;

  lv *= r;
  VM_PUSH(lv);
  vm->vm_PC++;
}

/* RESERVED */
DECL_OPCODE(1A)
{
  vm->vm_PC++;
}

/* shl */
DECL_OPCODE(1B)
{
  LONG lv, r;

  r = VM_ARG_S;
  lv = VM_POP;

  lv <<= r;
  VM_PUSH(lv);
  vm->vm_PC++;
}

/* shr */
DECL_OPCODE(1C)
{
  LONG lv, r;

  r = VM_ARG_S;
  lv = VM_POP;

  lv >>= r;
  VM_PUSH(lv);
  vm->vm_PC++;
}

/* RESERVED */
DECL_OPCODE(1D)
{
  vm->vm_PC++;
}

/* RESERVED */
DECL_OPCODE(1E)
{
  vm->vm_PC++;
}

/* RESERVED */
DECL_OPCODE(1F)
{
  vm->vm_PC++;
}

/* RESERVED */
DECL_OPCODE(20)
{
  vm->vm_PC++;
}

/* RESERVED */
DECL_OPCODE(21)
{
  vm->vm_PC++;
}

/* RESERVED */
DECL_OPCODE(22)
{
  vm->vm_PC++;
}

/* RESERVED */
DECL_OPCODE(23)
{
  vm->vm_PC++;
}

/* RESERVED */
DECL_OPCODE(24)
{
  vm->vm_PC++;
}

/* RESERVED */
DECL_OPCODE(25)
{
  vm->vm_PC++;
}

/* RESERVED */
DECL_OPCODE(26)
{
  vm->vm_PC++;
}

/* RESERVED */
DECL_OPCODE(27)
{
  vm->vm_PC++;
}

/* RESERVED */
DECL_OPCODE(28)
{
  vm->vm_PC++;
}

/* RESERVED */
DECL_OPCODE(29)
{
  vm->vm_PC++;
}

/* RESERVED */
DECL_OPCODE(2A)
{
  vm->vm_PC++;
}

/* RESERVED */
DECL_OPCODE(2B)
{
  vm->vm_PC++;
}

/* RESERVED */
DECL_OPCODE(2C)
{
  vm->vm_PC++;
}

/* RESERVED */
DECL_OPCODE(2D)
{
  vm->vm_PC++;
}

/* RESERVED */
DECL_OPCODE(2E)
{
  vm->vm_PC++;
}

/* RESERVED */
DECL_OPCODE(2F)
{
  vm->vm_PC++;
}

/* RESERVED */
DECL_OPCODE(30)
{
  vm->vm_PC++;
}

/* RESERVED */
DECL_OPCODE(31)
{
  vm->vm_PC++;
}

/* RESERVED */
DECL_OPCODE(32)
{
  vm->vm_PC++;
}

/* RESERVED */
DECL_OPCODE(33)
{
  vm->vm_PC++;
}

/* RESERVED */
DECL_OPCODE(34)
{
  vm->vm_PC++;
}

/* RESERVED */
DECL_OPCODE(35)
{
  vm->vm_PC++;
}

/* RESERVED */
DECL_OPCODE(36)
{
  vm->vm_PC++;
}

/* RESERVED */
DECL_OPCODE(37)
{
  vm->vm_PC++;
}

/* RESERVED */
DECL_OPCODE(38)
{
  vm->vm_PC++;
}

/* RESERVED */
DECL_OPCODE(39)
{
  vm->vm_PC++;
}

/* RESERVED */
DECL_OPCODE(3A)
{
  vm->vm_PC++;
}

/* RESERVED */
DECL_OPCODE(3B)
{
  vm->vm_PC++;
}

/* RESERVED */
DECL_OPCODE(3C)
{
  vm->vm_PC++;
}

/* RESERVED */
DECL_OPCODE(3D)
{
  vm->vm_PC++;
}

/* RESERVED */
DECL_OPCODE(3E)
{
  vm->vm_PC++;
}

/* RESERVED */
DECL_OPCODE(3F)
{
  vm->vm_PC++;
}

#undef DECL_OPCODE

#define CASE_OPCODE(NUM) case 0x##NUM: Op_##NUM(vm, opcode); return

STATIC VOID TickVirtualMachine(struct VIRTUAL_MACHINE* vm)
{
  UWORD opcode;
  
  opcode = vm->vm_Instructions[vm->vm_PC];
  
  switch (opcode & 0x3F)
  {
    CASE_OPCODE(00);
    CASE_OPCODE(01);
    CASE_OPCODE(02);
    CASE_OPCODE(03);
    CASE_OPCODE(04);
    CASE_OPCODE(05);
    CASE_OPCODE(06);
    CASE_OPCODE(07);
    CASE_OPCODE(08);
    CASE_OPCODE(09);
    CASE_OPCODE(0A);
    CASE_OPCODE(0B);
    CASE_OPCODE(0C);
    CASE_OPCODE(0D);
    CASE_OPCODE(0E);
    CASE_OPCODE(0F);
    CASE_OPCODE(20);
    CASE_OPCODE(21);
    CASE_OPCODE(22);
    CASE_OPCODE(23);
    CASE_OPCODE(24);
    CASE_OPCODE(25);
    CASE_OPCODE(26);
    CASE_OPCODE(27);
    CASE_OPCODE(28);
    CASE_OPCODE(29);
    CASE_OPCODE(2A);
    CASE_OPCODE(2B);
    CASE_OPCODE(2C);
    CASE_OPCODE(2D);
    CASE_OPCODE(2E);
    CASE_OPCODE(2F);
    CASE_OPCODE(30);
    CASE_OPCODE(31);
    CASE_OPCODE(32);
    CASE_OPCODE(33);
    CASE_OPCODE(34);
    CASE_OPCODE(35);
    CASE_OPCODE(36);
    CASE_OPCODE(37);
    CASE_OPCODE(38);
    CASE_OPCODE(39);
    CASE_OPCODE(3A);
    CASE_OPCODE(3B);
    CASE_OPCODE(3C);
    CASE_OPCODE(3D);
    CASE_OPCODE(3E);
    CASE_OPCODE(3F);
  }
}
