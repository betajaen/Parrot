/**
    $Id: Main.c, 0.1, 2020/05/06 20:56:00, betajaen Exp $

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

#include "Common.h"

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <dos/dos.h>
#include <workbench/startup.h>

#include <inline/parrot_protos.h>


BYTE VersionString[] = "$VER: Parrot 0.1 (6.5.2020)\r\n";
BYTE CopyrightString[] = "Copyright(c) 2020 Robin Southern. All Rights Reserved.";

struct ExecBase*      SysBase;
struct DosLibrary*    DOSBase;
struct IntuitionBase* IntuitionBase;
struct Library*       ParrotBase;

LONG Requester(CONST_STRPTR options, CONST_STRPTR text);
LONG RequesterF(CONST_STRPTR options, CONST_STRPTR text, ...);

ULONG ScreenNew(struct SCREEN_INFO*);
VOID ScreenDelete(ULONG);


APTR MemNew(ULONG size, ULONG requirements);
VOID MemDelete(APTR mem);
VOID MakeParrotContext(struct PARROT_CONTEXT* ctx);

INT main()
{
  struct Process* process;
  struct Message* wbMsg;
  INT             rc;
  struct SCREEN_INFO screenInfo;
  ULONG  screen;
  struct PARROT_CONTEXT ctx;

  rc = RETURN_OK;
  SysBase = NULL;
  DOSBase = NULL;
  IntuitionBase = NULL;

  SysBase = *(struct ExecBase**) 4L;

  process = (struct Process*) FindTask(NULL);

  if (process->pr_CLI != NULL)
  {
    DOSBase = (struct DosLibrary*) OpenLibrary("dos.library", 0);
    Write(Output(), "Parrot can only be launched from Workbench\n", 44);
    rc = RETURN_FAIL;
    goto CLEAN_EXIT;
  }

  WaitPort(&process->pr_MsgPort);
  wbMsg = GetMsg(&process->pr_MsgPort);

  DOSBase = (struct DosLibrary*) OpenLibrary("dos.library", 0);

  if (NULL == DOSBase)
  {
    rc = RETURN_FAIL;
    goto CLEAN_EXIT;
  }

  IntuitionBase = (struct IntuitionBase*) OpenLibrary("intuition.library", 0);

  if (NULL == IntuitionBase)
  {
    rc = RETURN_FAIL;
    goto CLEAN_EXIT;
  }

  ParrotBase = OpenLibrary("PROGDIR:parrot-test.library", 0);
  
  if (NULL == ParrotBase)
  {
    Requester("Ok", "Did not open");
    rc = RETURN_FAIL;
    goto CLEAN_EXIT;
  }
  MakeParrotContext(&ctx);

  RequesterF("Cool", "Opened %lx", ParrotBase);

  RequesterF("Cool", "State Before=%lx", ctx.State);


  Lib_Parrot_Initialise(&ctx);
  
  
  RequesterF("Cool", "State After = %lx", ctx.State);
  
  Lib_Parrot_Shutdown(&ctx);

  RequesterF("Cool", "State After.2 = %lx", ctx.State);

  CloseLibrary(ParrotBase);
  
  
  RequesterF("Cool", "Closed %lx", ParrotBase);

  screenInfo.si_Left = 0;
  screenInfo.si_Top  = 0;
  screenInfo.si_Width = 320;
  screenInfo.si_Height = 240;
  screenInfo.si_Depth = 3;
  screenInfo.si_Title = "Parrot";
  screenInfo.si_Flags = SIF_IS_PUBLIC;
  
  screen = ScreenNew(&screenInfo);
  
  Delay(50 * 2);
  
  ScreenDelete(screen);

  CLEAN_EXIT:

  if (NULL != wbMsg)
  {
    Forbid();
    ReplyMsg(wbMsg);
  }

  if (NULL != IntuitionBase)
  {
    CloseLibrary((struct Library*) IntuitionBase);
    IntuitionBase = NULL;
  }

  if (NULL != DOSBase)
  {
    CloseLibrary((struct Library*) DOSBase);
    DOSBase = NULL;
  }


  return rc;
}

VOID MakeParrotContext(struct PARROT_CONTEXT* ctx)
{
  ctx->State = 0;
  ctx->pi_ScreenOpen = ScreenNew;
  ctx->pi_ScreenClose = ScreenDelete;
  ctx->pi_MemNew = MemNew;
  ctx->pi_MemDelete = MemDelete;
}
