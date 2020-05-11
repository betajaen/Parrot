/**
    $Id: Lib.c, 0.1, 2020/05/10 09:16:00, betajaen Exp $

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

#include <SDI_lib.h>
#include <SDI_stdarg.h>

#include "Version.h"
#include "Game.h"

struct ParrotBase;

#define __BASE_OR_IFACE_TYPE	struct GameBase *
#define __BASE_OR_IFACE_VAR		GameBase
#define __BASE_OR_IFACE			__BASE_OR_IFACE_TYPE __BASE_OR_IFACE_VAR

VOID EXPORT_GameInitialise(
  REG(a6, UNUSED __BASE_OR_IFACE),
  REG(a0, struct ParrotBase* parrot)
);

VOID EXPORT_GameShutdown(
  REG(a6, UNUSED __BASE_OR_IFACE)
);

BOOL EXPORT_OnGameEvent(
  REG(a6, UNUSED __BASE_OR_IFACE),
  REG(d0, ULONG event),
  REG(d1, ULONG data)
);

LIBFUNC STATIC struct GameBase*       LibInit(REG(a0, BPTR Segment), REG(d0, struct GameBase* lh), REG(a6, struct ExecBase* sb));
LIBFUNC STATIC BPTR                   LibExpunge(REG(a6, struct GameBase* base));
LIBFUNC STATIC struct GameBase*       LibOpen(REG(a6, struct GameBase* base));
LIBFUNC STATIC BPTR                   LibClose(REG(a6, struct GameBase* base));
LIBFUNC STATIC LONG                   LibNull(void);

LIBFUNC STATIC LONG LibNull(VOID)
{
  return(0);
}

STATIC CONST APTR LibVectors[] =
{
  (APTR)LibOpen,
  (APTR)LibClose,
  (APTR)LibExpunge,
  (APTR)LibNull,
  (APTR)EXPORT_GameInitialise,
  (APTR)EXPORT_GameShutdown,
  (APTR)EXPORT_OnGameEvent,
  (APTR)-1
};

STATIC CONST ULONG LibInitTab[] =
{
  sizeof(struct GameBase),
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

struct GameBase*    GameBase;
struct ExecBase*    SysBase;
struct DosLibrary*  DOSBase;
struct ParrotBase*  ParrotBase;

#define DeleteLibrary(LIB) \
  FreeMem((STRPTR)(LIB)-(LIB)->lib_NegSize, (ULONG)((LIB)->lib_NegSize+(LIB)->lib_PosSize))

LIBFUNC STATIC struct GameBase* LibInit(REG(a0, BPTR librarySegment), REG(d0, struct GameBase* base), REG(a6, struct ExecBase* sb))
{
  SysBase = (APTR)sb;

  base->gb_LibBase.lib_Node.ln_Type = NT_LIBRARY;
  base->gb_LibBase.lib_Node.ln_Pri = 0;
  base->gb_LibBase.lib_Node.ln_Name = (char*)UserLibName;
  base->gb_LibBase.lib_Flags = LIBF_CHANGED | LIBF_SUMUSED;
  base->gb_LibBase.lib_Version = VERSION;
  base->gb_LibBase.lib_Revision = REVISION;
  base->gb_LibBase.lib_IdString = (char*)UserLibID;

  base->gb_SegList = librarySegment;
  base->gb_SysBase = (APTR)SysBase;

  DOSBase = (struct DosLibrary*) OpenLibrary("dos.library", 0);

  base->gb_DOSBase = DOSBase;

  GameBase = base;

  return(base);
}

LIBFUNC STATIC BPTR LibExpunge(REG(a6, struct GameBase* base))
{
  BPTR rc;

  if (base->gb_LibBase.lib_OpenCnt > 0)
  {
    base->gb_LibBase.lib_Flags |= LIBF_DELEXP;
    return(0);
  }

  SysBase = (APTR)base->gb_SysBase;
  rc = base->gb_SegList;

  Remove((struct Node*)base);

  CloseLibrary((struct Library*) DOSBase);

  DeleteLibrary(&base->gb_LibBase);

  return(rc);
}

LIBFUNC STATIC struct GameBase* LibOpen(REG(a6, struct GameBase* base))
{
  base->gb_LibBase.lib_Flags &= ~LIBF_DELEXP;
  base->gb_LibBase.lib_OpenCnt++;

  return base;
}

LIBFUNC STATIC BPTR LibClose(REG(a6, struct GameBase* base))
{
  if (base->gb_LibBase.lib_OpenCnt > 0 &&
    --base->gb_LibBase.lib_OpenCnt == 0)
  {
    if (base->gb_LibBase.lib_Flags & LIBF_DELEXP)
    {
      return LibExpunge(base);
    }
  }

  return 0;
}

VOID GameInitialise();

VOID EXPORT_GameInitialise(
  REG(a6, __BASE_OR_IFACE),
  REG(a0, struct ParrotBase* parrot)
)
{
  ParrotBase = parrot;
  GameBase->gb_ParrotBase = parrot;

  GameInitialise();
}

VOID GameShutdown();

VOID EXPORT_GameShutdown(
  REG(a6, UNUSED __BASE_OR_IFACE)
)
{
  GameShutdown();
}

BOOL OnGameEvent(ULONG event, ULONG data);

BOOL EXPORT_OnGameEvent(
  REG(a6, UNUSED __BASE_OR_IFACE),
  REG(d0, ULONG event),
  REG(d1, ULONG data)
)
{
  return OnGameEvent(event, data);
}
