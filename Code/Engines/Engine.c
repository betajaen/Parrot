/**
    $Id: Engine.c, 0.1, 2020/05/10 09:16:00, betajaen Exp $

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

#include "Parrot.h"

#include <exec/types.h>
#include <exec/libraries.h>
#include <exec/resident.h>
#include <dos/dos.h>

#include <proto/exec.h>

struct ExecBase* SysBase;

#include <SDI_lib.h>
#include <SDI_stdarg.h>

#include "Info.inc"

struct LibraryHeader
{
  struct Library	libBase;
  struct Library* sysBase;
  ULONG           segList;
};

#define __BASE_OR_IFACE_TYPE	struct LibraryHeader *
#define __BASE_OR_IFACE_VAR		EngineBase
#define __BASE_OR_IFACE			__BASE_OR_IFACE_TYPE __BASE_OR_IFACE_VAR

VOID Lib_Parrot_Initialise(
  REG(a6, UNUSED __BASE_OR_IFACE),
  REG(a0, struct PARROT_CONTEXT* parrot)
);

VOID Lib_Parrot_Shutdown(
  REG(a6, UNUSED __BASE_OR_IFACE),
  REG(a0, struct PARROT_CONTEXT* parrot)
);

BOOL Lib_Parrot_Event(
  REG(a6, UNUSED __BASE_OR_IFACE),
  REG(a0, struct PARROT_CONTEXT* parrot),
  REG(d0, ULONG event),
  REG(d1, ULONG data)
);

LIBFUNC static struct LibraryHeader*  LibInit(REG(a0, BPTR Segment), REG(d0, struct LibraryHeader* lh), REG(a6, struct ExecBase* sb));
LIBFUNC static BPTR                   LibExpunge(REG(a6, struct LibraryHeader* base));
LIBFUNC static struct LibraryHeader*  LibOpen(REG(a6, struct LibraryHeader* base));
LIBFUNC static BPTR                   LibClose(REG(a6, struct LibraryHeader* base));
LIBFUNC static LONG                   LibNull(void);

LIBFUNC static LONG LibNull(VOID)
{
  return(0);
}

STATIC CONST APTR LibVectors[] =
{
  (APTR)LibOpen,
  (APTR)LibClose,
  (APTR)LibExpunge,
  (APTR)LibNull,
  (APTR)Lib_Parrot_Initialise,
  (APTR)Lib_Parrot_Shutdown,
  (APTR)Lib_Parrot_Event,
  (APTR)-1
};

STATIC CONST ULONG LibInitTab[] =
{
  sizeof(struct LibraryHeader),
  (ULONG)LibVectors,
  (ULONG)NULL,
  (ULONG)LibInit
};

static const USED_VAR struct Resident ROMTag =
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

LIBFUNC static struct LibraryHeader* LibInit(REG(a0, BPTR librarySegment), REG(d0, struct LibraryHeader* base), REG(a6, struct ExecBase* sb))
{
  SysBase = (APTR)sb;

  base->libBase.lib_Node.ln_Type = NT_LIBRARY;
  base->libBase.lib_Node.ln_Pri = 0;
  base->libBase.lib_Node.ln_Name = (char*)UserLibName;
  base->libBase.lib_Flags = LIBF_CHANGED | LIBF_SUMUSED;
  base->libBase.lib_Version = VERSION;
  base->libBase.lib_Revision = REVISION;
  base->libBase.lib_IdString = (char*)UserLibID;

  base->segList = librarySegment;
  base->sysBase = (APTR)SysBase;

  return(base);
}

LIBFUNC static BPTR LibExpunge(REG(a6, struct LibraryHeader* base))
{
  BPTR rc;

  if (base->libBase.lib_OpenCnt > 0)
  {
    base->libBase.lib_Flags |= LIBF_DELEXP;
    return(0);
  }

  SysBase = (APTR)base->sysBase;
  rc = base->segList;

  Remove((struct Node*)base);
  DeleteLibrary(&base->libBase);

  return(rc);
}

LIBFUNC static struct LibraryHeader* LibOpen(REG(a6, struct LibraryHeader* base))
{
  base->libBase.lib_Flags &= ~LIBF_DELEXP;
  base->libBase.lib_OpenCnt++;

  return base;
}

LIBFUNC static BPTR LibClose(REG(a6, struct LibraryHeader* base))
{
  if (base->libBase.lib_OpenCnt > 0 &&
    --base->libBase.lib_OpenCnt == 0)
  {
    if (base->libBase.lib_Flags & LIBF_DELEXP)
    {
      return LibExpunge(base);
    }
  }

  return 0;
}
