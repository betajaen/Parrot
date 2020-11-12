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

STATIC LONG Globals[MAX_SCRIPT_GLOBALS];
STATIC struct VIRTUAL_MACHINE VirtualMachine[MAX_VIRTUAL_MACHINES];


VOID ScriptInitialise()
{
  UWORD ii;
  struct VIRTUAL_MACHINE* vm;

  for (ii = 0; ii < MAX_VIRTUAL_MACHINES; ii++)
  {
    vm = &VirtualMachine[ii];
    vm->vm_State = VM_STATE_FREE;
    vm->vm_PC = 0;
    vm->vm_Script = NULL;
    vm->vm_Timer = 0;
  }

  for (ii = 0; ii < MAX_SCRIPT_GLOBALS; ii++)
  {
    Globals[ii] = 0;
  }

}

VOID ScriptShutdown()
{

}
