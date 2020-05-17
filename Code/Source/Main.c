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

#include <Parrot/Parrot.h>
#include <Parrot/Requester.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/iffparse.h>
#include <dos/dos.h>
#include <workbench/startup.h>

BYTE VersionString[]   = "$VER: Parrot 0.1 (6.5.2020)\r\n";
BYTE CopyrightString[] = "Copyright(c) 2020 Robin Southern. All Rights Reserved.";

struct ExecBase*      SysBase;
struct DosLibrary*    DOSBase;
struct IntuitionBase* IntuitionBase;
struct GfxBase*       GfxBase;
struct Library*       IFFParseBase;

EXPORT VOID GameStart(STRPTR name);

INT main()
{
  struct Process* process;
  struct Message* wbMsg;
  INT             rc;
  struct SCREEN_INFO screenInfo;
  ULONG  screen;

  rc = RETURN_OK;
  SysBase = NULL;
  DOSBase = NULL;
  IntuitionBase = NULL;
  GfxBase = NULL;
  IFFParseBase = NULL;

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

  GfxBase = (struct GfxBase*) OpenLibrary("graphics.library", 0);

  if (NULL == GfxBase)
  {
    rc = RETURN_FAIL;
    goto CLEAN_EXIT;
  }

  IFFParseBase = OpenLibrary("iffparse.library", 0);

  if (NULL == IFFParseBase)
  {
    rc = RETURN_FAIL;
    goto CLEAN_EXIT;
  }

  GameStart("PROGDIR:Tools");
  
  CLEAN_EXIT:

  if (NULL != wbMsg)
  {
    Forbid();
    ReplyMsg(wbMsg);
  }

  if (NULL != IFFParseBase)
  {
    CloseLibrary((struct Library*) IFFParseBase);
    IFFParseBase = NULL;
  }

  if (NULL != GfxBase)
  {
    CloseLibrary((struct Library*) GfxBase);
    GfxBase = NULL;
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
