/**
    $Id: Lib.c, 0.1, 2020/05/09 16:12:00, betajaen Exp $

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
#include <proto/graphics.h>
#include <dos/dos.h>

#include <exec/types.h>
#include <exec/libraries.h>
#include <exec/resident.h>

#include <SDI_lib.h>

#define VERSION	  1
#define REVISION  0
#define DATE      "09.05.2020"
#define VERS      "Parrot.library 1.0"
#define VSTRING   "Parrot.library 1.0 (09.05.2020)\r\n"
#define VERSTAG   "\0$VER: Parrot.library 1.0 (09.05.2020)\r\n"

STATIC CONST CHAR CopyrightString[] = "Copyright(c) 2020 Robin Southern. All Rights Reserved.";
STATIC CONST CHAR UserLibName[] = "Parrot.library";
STATIC CONST CHAR UserLibID[] = "\0$VER: Parrot.library 1.0 (9.5.2020)\r\n";

struct ExecBase*      SysBase;
struct DosLibrary*    DOSBase;
struct IntuitionBase* IntuitionBase;
struct GfxBase*       GfxBase;
struct Library*       ParrotBase;

struct ParrotBase
{
  struct Library  pb_LibBase;
  ULONG           pb_SegList;
  struct Library* pb_SysBase;
  struct Library* pb_GameBase;
};

/* Stub Entry Function */
STATIC INT LibNull()
{
  return 0;
}

LIBFUNC STATIC struct ParrotBase* LibInit(REG(a0, BPTR Segment), REG(d0, struct ParrotBase* lh), REG(a6, struct ExecBase* sb));
LIBFUNC STATIC BPTR               LibExpunge(REG(a6, struct ParrotBase* base));
LIBFUNC STATIC struct ParrotBase* LibOpen(REG(a6, struct ParrotBase* base));
LIBFUNC STATIC BPTR               LibClose(REG(a6, struct ParrotBase* base));

#include "Protos.inc"

STATIC CONST APTR LibVectors[] =
{
  (APTR)LibOpen,
  (APTR)LibClose,
  (APTR)LibExpunge,
  (APTR)LibNull
  
  EXPORT_Vectors

  , (APTR)-1
};

STATIC CONST ULONG LibInitTab[] =
{
  sizeof(struct ParrotBase),
  (ULONG)LibVectors,
  (ULONG)NULL,
  (ULONG)LibInit
};

STATIC CONST USED_VAR struct Resident ROMTag =
{
  RTC_MATCHWORD,
  (struct Resident*) & ROMTag,
  (struct Resident*) & ROMTag + 1,
  RTF_AUTOINIT,
  VERSION,
  NT_LIBRARY,
  0,
  (APTR)UserLibName,
  VSTRING,
  (APTR)LibInitTab
};

#define DeleteLibrary(LIB) \
  FreeMem((STRPTR)(LIB)-(LIB)->lib_NegSize, (ULONG)((LIB)->lib_NegSize+(LIB)->lib_PosSize))

LIBFUNC STATIC struct ParrotBase* LibInit(REG(a0, BPTR librarySegment), REG(d0, struct ParrotBase* base), REG(a6, struct ExecBase* sb))
{
  SysBase = (APTR)sb;

  base->pb_LibBase.lib_Node.ln_Type = NT_LIBRARY;
  base->pb_LibBase.lib_Node.ln_Pri = 0;
  base->pb_LibBase.lib_Node.ln_Name = (char*)UserLibName;
  base->pb_LibBase.lib_Flags = LIBF_CHANGED | LIBF_SUMUSED;
  base->pb_LibBase.lib_Version = VERSION;
  base->pb_LibBase.lib_Revision = REVISION;
  base->pb_LibBase.lib_IdString = (char*)UserLibID;

  base->pb_SegList = librarySegment;
  base->pb_SysBase = (APTR)SysBase;

  DOSBase = (struct DosLibrary*) OpenLibrary("dos.library", 0);

  if (NULL == DOSBase)
  {
    return(base);
  }

  IntuitionBase = (struct IntuitionBase*) OpenLibrary("intuition.library", 0);

  if (NULL == IntuitionBase)
  {
    return(base);
  }

  GfxBase = (struct GfxBase*) OpenLibrary("graphics.library", 0);

  if (NULL == GfxBase)
  {
    return(base);
  }

  ParrotBase = (struct Library*) &base->pb_LibBase;

  return(base);
}

LIBFUNC STATIC BPTR LibExpunge(REG(a6, struct ParrotBase* base))
{
  BPTR rc;

  if (base->pb_LibBase.lib_OpenCnt > 0)
  {
    base->pb_LibBase.lib_Flags |= LIBF_DELEXP;
    return(0);
  }

  SysBase = (APTR)base->pb_SysBase;
  rc = base->pb_SegList;

  Remove((struct Node*)base);
  DeleteLibrary(&base->pb_LibBase);
  
  if (NULL != IntuitionBase)
  {
    CloseLibrary((struct Library*) IntuitionBase);
  }

  if (NULL != DOSBase)
  {
    CloseLibrary((struct Library*) DOSBase);
  }

  return(rc);
}

LIBFUNC STATIC struct ParrotBase* LibOpen(REG(a6, struct ParrotBase* base))
{
  base->pb_LibBase.lib_Flags &= ~LIBF_DELEXP;
  base->pb_LibBase.lib_OpenCnt++;

  return base;
}

LIBFUNC STATIC BPTR LibClose(REG(a6, struct ParrotBase* base))
{
  if (base->pb_LibBase.lib_OpenCnt > 0 &&
    --base->pb_LibBase.lib_OpenCnt == 0)
  {
    if (base->pb_LibBase.lib_Flags & LIBF_DELEXP)
    {
      return LibExpunge(base);
    }
  }

  return 0;
}
