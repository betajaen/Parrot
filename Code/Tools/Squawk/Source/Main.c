/**
    $Id: Main.c, 1.0, 2020/05/13 07:16:00, betajaen Exp $

    Maniac Game Converter for Parrot
    ================================

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

#include <Squawk/Squawk.h>

// struct ExecBase* SysBase;
struct DosLibrary* DOSBase;
struct Library* IFFParseBase;
struct IntuitionBase* IntuitionBase;

struct Process* process;
struct Message* wbMsg;


#define RC_OKAY RETURN_OK
#define RC_FAIL RETURN_FAIL

#ifndef PtSigned32
#define PtSigned32 int
#endif

#ifndef PtChar
#define PtChar char
#endif

PtUnsigned32 StrFormat(PtChar* pBuffer, PtSigned32 pBufferCapacity, PtChar* pFmt, ...);
PtUnsigned32 StrCopy(PtChar* pDst, PtUnsigned32 pDstCapacity, CONST PtChar* pSrc);
extern void exit();

void ConverterMain();
void StartTables();
void EndTables();

PtSigned32 main()
{
  PtSigned32 rc;

  rc = RETURN_OK;

  SysBase = *(struct ExecBase**)4L;

  process = (struct Process*)FindTask(NULL);

  if (process->pr_CLI != NULL)
  {
    DOSBase = (struct DosLibrary*)OpenLibrary("dos.library", 0);
    Write(Output(), "Maniac Mansion Converter can only be launched from Workbench\n", 44);
    goto CLEAN_EXIT;
  }

  WaitPort(&process->pr_MsgPort);
  wbMsg = GetMsg(&process->pr_MsgPort);

  DOSBase = (struct DosLibrary*)OpenLibrary("dos.library", 0);

  if (NULL == DOSBase)
  {
    goto CLEAN_EXIT;
  }

  IntuitionBase = (struct IntuitionBase*)OpenLibrary("intuition.library", 0);

  if (NULL == IntuitionBase)
  {
    goto CLEAN_EXIT;
  }

  IFFParseBase = (struct Library*)OpenLibrary("iffparse.library", 0);

  if (NULL == IFFParseBase)
  {
    goto CLEAN_EXIT;
  }

  ConverterMain();


CLEAN_EXIT:

  if (NULL != IFFParseBase)
  {
    CloseLibrary((struct Library*)IFFParseBase);
    IFFParseBase = NULL;
  }

  if (NULL != wbMsg)
  {
    Forbid();
    ReplyMsg(wbMsg);
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

  return RETURN_OK;
}


void ExitNow()
{
  if (NULL != wbMsg)
  {
    Forbid();
    ReplyMsg(wbMsg);
  }

  if (NULL != IFFParseBase)
  {
    CloseLibrary((struct Library*)IFFParseBase);
    IFFParseBase = NULL;
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

  exit();
}


