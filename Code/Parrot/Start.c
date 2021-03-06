/**
    $Id: Start.c 1.5 2021/02/21 10:22:00, betajaen Exp $

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
#include <Parrot/Log.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <dos/dos.h>
#include <workbench/startup.h>

struct DosLibrary* DOSBase;
struct IntuitionBase* IntuitionBase;
struct GfxBase* GfxBase;
struct Message* sWbMsg;

void parrot_entry(); /* User Defined in Application */

int main()
{
  struct Process* process;
  int      rc;

  rc = RETURN_OK;
  DOSBase = NULL;
  IntuitionBase = NULL;
  GfxBase = NULL;

  process = (struct Process*)FindTask(NULL);

  if (process->pr_CLI != NULL)
  {
    DOSBase = (struct DosLibrary*)OpenLibrary("dos.library", 33L);
    Write(Output(), "Parrot can only be launched from Workbench\n", 44);
    rc = RETURN_FAIL;
    goto CleanAndLeave;
  }

  WaitPort(&process->pr_MsgPort);
  sWbMsg = GetMsg(&process->pr_MsgPort);

  DOSBase = (struct DosLibrary*)OpenLibrary("dos.library", 33L);

  if (NULL == DOSBase)
  {
    rc = RETURN_FAIL;
    goto CleanAndLeave;
  }

  IntuitionBase = (struct IntuitionBase*)OpenLibrary("intuition.library", 33L);

  if (NULL == IntuitionBase)
  {
    rc = RETURN_FAIL;
    goto CleanAndLeave;
  }

  GfxBase = (struct GfxBase*)OpenLibrary("graphics.library", 33L);

  if (NULL == GfxBase)
  {
    rc = RETURN_FAIL;
    goto CleanAndLeave;
  }

  parrot_entry();

CleanAndLeave:

  if (NULL != sWbMsg)
  {
    Forbid();
    ReplyMsg(sWbMsg);
  }

  if (NULL != GfxBase)
  {
    CloseLibrary((struct Library*)GfxBase);
    GfxBase = NULL;
  }

  if (NULL != IntuitionBase)
  {
    CloseLibrary((struct Library*)IntuitionBase);
    IntuitionBase = NULL;
  }

  if (NULL != DOSBase)
  {
    CloseLibrary((struct Library*)DOSBase);
    DOSBase = NULL;
  }

  return rc;
}
