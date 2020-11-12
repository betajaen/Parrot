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

STATIC LONG Globals[MAX_SCRIPT_GLOBALS];
STATIC struct VIRTUAL_MACHINE VirtualMachine[MAX_VIRTUAL_MACHINES];

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

/* stop
   Stop Script
   2 => 0, 0
*/
STATIC INLINE VOID OpStop(struct VIRTUAL_MACHINE* vm)
{
  vm->vm_State = VM_STATE_STOP;
}

/* rem
   Remark
   2 => 1, user
*/
STATIC INLINE VOID OpRem(struct VIRTUAL_MACHINE* vm)
{
  vm->vm_PC += 2;
}

/* pushb
   Push Byte onto Stack
   2 => 2, value
*/
STATIC INLINE VOID OpPushByte(struct VIRTUAL_MACHINE* vm)
{
  BYTE value;

  if (vm->vm_StackHead == MAX_VM_STACK_SIZE)
  {
    PARROT_ERR(
      "VM Is out of Stack Space!\n"
      "Reason: Tried to push a byte on stack, and ran out of space"
      PARROT_ERR_STR("Opcode")
      PARROT_ERR_INT("Asset Id")
      PARROT_ERR_INT("PC"),
      "pushb",
      (ULONG) GET_ASSET_ID(vm->vm_Script),
      (ULONG) vm->vm_PC
    );
  }

  value = VM_PARAM_B(VM, 1);

  vm->vm_Stack[vm->vm_StackHead] = value;
  ++vm->vm_StackHead;
  vm->vm_PC += 2;
}

/* pushw 
   Push Word onto Stack
   4 => 3, 0, value.w[1], value.w[0]
*/
STATIC INLINE VOID OpPushWord(struct VIRTUAL_MACHINE* vm)
{
  WORD value;

  if (vm->vm_StackHead == MAX_VM_STACK_SIZE)
  {
    PARROT_ERR(
      "VM Is out of Stack Space!\n"
      "Reason: Tried to push a word on stack, and ran out of space"
      PARROT_ERR_STR("Opcode")
      PARROT_ERR_INT("Asset Id")
      PARROT_ERR_INT("PC"),
      "pushw",
      (ULONG)GET_ASSET_ID(vm->vm_Script),
      (ULONG)vm->vm_PC
    );
  }

  value = VM_PARAM_W(2);

  vm->vm_Stack[vm->vm_StackHead] = value;
  ++vm->vm_StackHead;
  vm->vm_PC += 4;
}

/* pushl
   Push Long onto Stack
   6 => 4, 0, value.l[3], value.[2], value.w[2], value.w[0]
*/
STATIC INLINE VOID OpPushWord(struct VIRTUAL_MACHINE* vm)
{
  LONG value;

  if (vm->vm_StackHead == MAX_VM_STACK_SIZE)
  {
    PARROT_ERR(
      "VM Is out of Stack Space!\n"
      "Reason: Tried to push a long on stack, and ran out of space"
      PARROT_ERR_STR("Opcode")
      PARROT_ERR_INT("Asset Id")
      PARROT_ERR_INT("PC"),
      "pushl",
      (ULONG)GET_ASSET_ID(vm->vm_Script),
      (ULONG)vm->vm_PC
    );
  }

  value = VM_PARAM_W(2);

  vm->vm_Stack[vm->vm_StackHead] = value;
  ++vm->vm_StackHead;
  vm->vm_PC += 6;
}

/* pushs
   Push Stack value onto Stack
   2 => 5, where
*/
STATIC INLINE VOID OpPushStack(struct VIRTUAL_MACHINE* vm)
{
  BYTE where;
  WORD loc;

  where = VM_PARAM_B(1);

  loc = vm->vm_StackHead - (WORD) where;

  if (loc < 0)
  {
    PARROT_ERR(
      "Stack Outof bounds!\n"
      "Reason: Tried to push a stack value from the stack on an out of bounds stack area"
      PARROT_ERR_STR("Opcode")
      PARROT_ERR_INT("Asset Id")
      PARROT_ERR_INT("PC"),
      "pushs",
      (ULONG)GET_ASSET_ID(vm->vm_Script),
      (ULONG)vm->vm_PC
    );
  }

  vm->vm_Stack[vm->vm_StackHead] = vm->vm_Stack[loc];
  ++vm->vm_StackHead;
  vm->vm_PC += 2;
}

/* pop
   Pop Stack value from the stack and discard value.
   2 => 6, 0
*/
STATIC INLINE VOID OpPopStack(struct VIRTUAL_MACHINE* vm)
{
  if (vm->vm_StackHead == 0)
  {
    PARROT_ERR(
      "VM stack underflow!\n"
      "Reason: Tried to pop from the stack when it is empty!"
      PARROT_ERR_STR("Opcode")
      PARROT_ERR_INT("Asset Id")
      PARROT_ERR_INT("PC"),
      "pop",
      (ULONG)GET_ASSET_ID(vm->vm_Script),
      (ULONG)vm->vm_PC
    );
  }

  --vm->vm_StackHead;
  vm->vm_PC += 2;
}


/* j
   Jump to relative address
   2 => 7, address
*/
STATIC INLINE VOID OpJump(struct VIRTUAL_MACHINE* vm)
{
  LONG newPc;
  newPc = VM_PARAM_B(VM, 1);
  newPc *= 2;
  newPc += vm->vm_PC;

  if (newPc < 0)
  {
    PARROT_ERR(
      "Program Counter underflow!\n"
      "Reason: Tried to jump to an address outside script space"
      PARROT_ERR_STR("Opcode")
      PARROT_ERR_INT("Asset Id")
      PARROT_ERR_INT("PC"),
      "j",
      (ULONG)GET_ASSET_ID(vm->vm_Script),
      (ULONG)vm->vm_PC
    );
  }

  if (newPc >= vm->vm_Script->sc_Length)
  {
    PARROT_ERR(
      "Program Counter overflow!\n"
      "Reason: Tried to jump to an address outside script space"
      PARROT_ERR_STR("Opcode")
      PARROT_ERR_INT("Asset Id")
      PARROT_ERR_INT("PC"),
      "j",
      (ULONG)GET_ASSET_ID(vm->vm_Script),
      (ULONG)vm->vm_PC
    );
  }

  vm->vm_PC = newPc;
}

