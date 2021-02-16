/**
    $Id: Room.c, 1.2 2020/05/11 15:49:00, betajaen Exp $

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
#include <Parrot/Log.h>
#include <Parrot/Api.h>

extern BOOL InEvtForceQuit;

void Parrot_SysCall(PtUnsigned16 function, PtSigned32 argument)
{
  switch (function)
  {
    default:
    {
      ERRORF("SysCall. Unknown Function = %ld, Argument = 0x%lx", function, argument);
    }
    break;
    case SYSCALL_EXIT:
    {
      InEvtForceQuit = TRUE;
      TRACEF("SysCall. Exit. Argument = 0x%lx", argument);
    }
    break;
    case SYSCALL_TRACE:
    {
      INFOF("SysCall. Trace. Argument = 0x%lx", argument);
    }
    break;
    case SYSCALL_STOP_SCRIPT:
    {
      if (Vm_Current->vm_State == VM_STATE_RUN)
      {
        Vm_Current->vm_State = VM_STATE_STOPPING;
      }
      TRACEF("SysCall. Stop. Argument = 0x%lx", argument);
    }
    break;
    case SYSCALL_LOAD_GAME:
    {
      /* argument is slot number */
      TRACEF("SysCall. LoadGame. Argument = 0x%lx", argument);
    }
    break;
    case SYSCALL_SAVE_GAME:
    {
      /* argument is slot number */
      TRACEF("SysCall. SaveGame. Argument = 0x%lx", argument);
    }
    break;
    case SYSCALL_DELAY_TICKS:
    {
      /* argument is slot number */
      TRACEF("SysCall. DelayTicks. Argument = 0x%lx", argument);
      Api_DelayTicks(argument);
    }
    break;
    case SYSCALL_DELAY_SECONDS:
    {
      /* argument is slot number */
      TRACEF("SysCall. DelaySeconds. Argument = 0x%lx", argument);
      Api_DelaySeconds(argument);
    }
    break;
  }
}

