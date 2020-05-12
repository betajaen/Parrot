/**
    $Id: Protos.inc, 1.0, 2020/05/10 13:30:00, betajaen Exp $

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

/***START***/

/* GameStart */
VOID GameStart(CONST_STRPTR name);
VOID EXPORT_GameStart(__reg("a6") struct ParrotBase*, __reg("a0") CONST_STRPTR name)
{
  GameStart(name);
}

/* TestRequester */
LONG TestRequester(LONG x);
LONG EXPORT_TestRequester(__reg("a6") struct ParrotBase*, __reg("d0") LONG x)
{
  return TestRequester(x);
}

/* MemNew */
APTR MemNew(ULONG size, ULONG requirements);
APTR EXPORT_MemNew(__reg("a6") struct ParrotBase*, __reg("d0") ULONG size, __reg("d1") ULONG requirements)
{
  return MemNew(size, requirements);
}

/* MemDelete */
BOOL MemDelete(APTR pMem);
BOOL EXPORT_MemDelete(__reg("a6") struct ParrotBase*, __reg("a0") APTR pMem)
{
  return MemDelete(pMem);
}

/* MemSize */
ULONG MemSize(APTR pMem);
ULONG EXPORT_MemSize(__reg("a6") struct ParrotBase*, __reg("a0") APTR pMem)
{
  return MemSize(pMem);
}

/* ScreenNew */
APTR ScreenNew(APTR arena, struct SCREEN_INFO* info);
APTR EXPORT_ScreenNew(__reg("a6") struct ParrotBase*, __reg("a0") APTR arena, __reg("a1") struct SCREEN_INFO* info)
{
  return ScreenNew(arena, info);
}

/* ScreenDelete */
VOID ScreenDelete(APTR obj);
VOID EXPORT_ScreenDelete(__reg("a6") struct ParrotBase*, __reg("a0") APTR obj)
{
  ScreenDelete(obj);
}

/* ScreenSetCursor */
VOID ScreenSetCursor(APTR screen, UBYTE cursor);
VOID EXPORT_ScreenSetCursor(__reg("a6") struct ParrotBase*, __reg("a0") APTR screen, __reg("d0") UBYTE cursor)
{
  ScreenSetCursor(screen, cursor);
}

/* ScreenGetCursor */
UBYTE ScreenGetCursor(APTR screen);
UBYTE EXPORT_ScreenGetCursor(__reg("a6") struct ParrotBase*, __reg("a0") APTR screen)
{
  return ScreenGetCursor(screen);
}

/* ScreenSetColour */
VOID ScreenSetColour(APTR screen, UWORD index, UBYTE r, UBYTE g, UBYTE b);
VOID EXPORT_ScreenSetColour(__reg("a6") struct ParrotBase*, __reg("a0") APTR screen, __reg("d0") UWORD index, __reg("d1") UBYTE r, __reg("d2") UBYTE g, __reg("d3") UBYTE b)
{
  ScreenSetColour(screen, index, r, g, b);
}

/* GameDelaySeconds */
VOID GameDelaySeconds(UWORD seconds);
VOID EXPORT_GameDelaySeconds(__reg("a6") struct ParrotBase*, __reg("d0") UWORD seconds)
{
  GameDelaySeconds(seconds);
}

/* GameDelayTicks */
VOID GameDelayTicks(UWORD ticks);
VOID EXPORT_GameDelayTicks(__reg("a6") struct ParrotBase*, __reg("d0") UWORD ticks)
{
  GameDelayTicks(ticks);
}

/* ArenaNew */
APTR ArenaNew(ULONG size, ULONG requirements);
APTR EXPORT_ArenaNew(__reg("a6") struct ParrotBase*, __reg("d0") ULONG size, __reg("d1") ULONG requirements)
{
  return ArenaNew(size, requirements);
}

/* ArenaDelete */
BOOL ArenaDelete(APTR arena);
BOOL EXPORT_ArenaDelete(__reg("a6") struct ParrotBase*, __reg("a0") APTR arena)
{
  return ArenaDelete(arena);
}

/* ArenaSize */
ULONG ArenaSize(APTR arena);
ULONG EXPORT_ArenaSize(__reg("a6") struct ParrotBase*, __reg("a0") APTR arena)
{
  return ArenaSize(arena);
}

/* ObjAlloc */
APTR ObjAlloc(APTR arena, ULONG size, ULONG class);
APTR EXPORT_ObjAlloc(__reg("a6") struct ParrotBase*, __reg("a0") APTR arena, __reg("d0") ULONG size, __reg("d1") ULONG class)
{
  return ObjAlloc(arena, size, class);
}

/* ObjGetClass */
ULONG ObjGetClass(APTR alloc);
ULONG EXPORT_ObjGetClass(__reg("a6") struct ParrotBase*, __reg("a0") APTR alloc)
{
  return ObjGetClass(alloc);
}

/* ObjGetSize */
ULONG ObjGetSize(APTR alloc);
ULONG EXPORT_ObjGetSize(__reg("a6") struct ParrotBase*, __reg("a0") APTR alloc)
{
  return ObjGetSize(alloc);
}

/* ArenaRollback */
BOOL ArenaRollback(APTR arena);
BOOL EXPORT_ArenaRollback(__reg("a6") struct ParrotBase*, __reg("a0") APTR arena)
{
  return ArenaRollback(arena);
}

/* ArenaSpace */
ULONG ArenaSpace(APTR arena);
ULONG EXPORT_ArenaSpace(__reg("a6") struct ParrotBase*, __reg("a0") APTR arena)
{
  return ArenaSpace(arena);
}

/* ScreenClear */
VOID ScreenClear(APTR obj);
VOID EXPORT_ScreenClear(__reg("a6") struct ParrotBase*, __reg("a0") APTR obj)
{
  ScreenClear(obj);
}

/* ScreenSwapBuffers */
VOID ScreenSwapBuffers(APTR obj);
VOID EXPORT_ScreenSwapBuffers(__reg("a6") struct ParrotBase*, __reg("a0") APTR obj)
{
  ScreenSwapBuffers(obj);
}

/* RoomNew */
struct ROOM* RoomNew(APTR arena);
struct ROOM* EXPORT_RoomNew(__reg("a6") struct ParrotBase*, __reg("a0") APTR arena)
{
  return RoomNew(arena);
}


#define EXPORT_Vectors \
  , (APTR) EXPORT_GameStart\
  , (APTR) EXPORT_TestRequester\
  , (APTR) EXPORT_MemNew\
  , (APTR) EXPORT_MemDelete\
  , (APTR) EXPORT_MemSize\
  , (APTR) EXPORT_ScreenNew\
  , (APTR) EXPORT_ScreenDelete\
  , (APTR) EXPORT_ScreenSetCursor\
  , (APTR) EXPORT_ScreenGetCursor\
  , (APTR) EXPORT_ScreenSetColour\
  , (APTR) EXPORT_GameDelaySeconds\
  , (APTR) EXPORT_GameDelayTicks\
  , (APTR) EXPORT_ArenaNew\
  , (APTR) EXPORT_ArenaDelete\
  , (APTR) EXPORT_ArenaSize\
  , (APTR) EXPORT_ObjAlloc\
  , (APTR) EXPORT_ObjGetClass\
  , (APTR) EXPORT_ObjGetSize\
  , (APTR) EXPORT_ArenaRollback\
  , (APTR) EXPORT_ArenaSpace\
  , (APTR) EXPORT_ScreenClear\
  , (APTR) EXPORT_ScreenSwapBuffers\
  , (APTR) EXPORT_RoomNew

/***END***/
