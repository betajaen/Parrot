/**
    $Id: Main.c, 1.2 2020/05/06 20:56:00, betajaen Exp $

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
#include <Parrot/Graphics.h>
#include <Parrot/Log.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/iffparse.h>
#include <dos/dos.h>
#include <workbench/startup.h>

#define PARROT_VERSION "Parrot 1.3 (11.02.2021)"

PtByte VersionString[]   = "$VER: " PARROT_VERSION "\r\n";
PtByte CopyrightString[] = "Copyright(c) 2021 Robin Southern. All Rights Reserved.";

//struct ExecBase*      SysBase;
struct DosLibrary*    DOSBase;
struct IntuitionBase* IntuitionBase;
struct GfxBase*       GfxBase;
struct Library*       IFFParseBase;
struct Message*       wbMsg;

PtPublic void GameStart(STRPTR name);

PtUnsigned16 GcCounter;

PtSigned32 main()
{
  struct Process* process;
  PtSigned32      rc;

  rc = RETURN_OK;
  DOSBase = NULL;
  IntuitionBase = NULL;
  GfxBase = NULL;
  IFFParseBase = NULL;

  process = (struct Process*) FindTask(NULL);

  SetRequesterWindow(NULL);

  if (process->pr_CLI != NULL)
  {
    DOSBase = (struct DosLibrary*) OpenLibrary("dos.library", 33L);
    Write(Output(), "Parrot can only be launched from Workbench\n", 44);
    rc = RETURN_FAIL;
    goto CLEAN_EXIT;
  }

  WaitPort(&process->pr_MsgPort);
  wbMsg = GetMsg(&process->pr_MsgPort);

  DOSBase = (struct DosLibrary*) OpenLibrary("dos.library", 33L);

  if (NULL == DOSBase)
  {
    rc = RETURN_FAIL;
    goto CLEAN_EXIT;
  }

  Log_Initialise();
  INFO("Parrot " PARROT_VERSION);

  IntuitionBase = (struct IntuitionBase*) OpenLibrary("intuition.library", 33L);

  if (NULL == IntuitionBase)
  {
    rc = RETURN_FAIL;
    goto CLEAN_EXIT;
  }

  GfxBase = (struct GfxBase*) OpenLibrary("graphics.library", 33L);

  if (NULL == GfxBase)
  {
    rc = RETURN_FAIL;
    goto CLEAN_EXIT;
  }

  IFFParseBase = OpenLibrary("iffparse.library", 33L);

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
    INFO("Log Shutdown");
    Log_Shutdown();

    CloseLibrary((struct Library*) DOSBase);
    DOSBase = NULL;
  }


  return rc;
}

void ExitArenaNow();
void ExitScreenNow();
extern void exit();

void ExitNow()
{

  ExitArenaNow();
  GfxHide();
  GfxClose();

  //ExitScreenNow();


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

  exit();
}
