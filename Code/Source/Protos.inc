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
ULONG ScreenNew(struct SCREEN_INFO* info);
ULONG EXPORT_ScreenNew(__reg("a6") struct ParrotBase*, __reg("a0") struct SCREEN_INFO* info)
{
  return ScreenNew(info);
}

/* ScreenDelete */
VOID ScreenDelete(ULONG screen);
VOID EXPORT_ScreenDelete(__reg("a6") struct ParrotBase*, __reg("d0") ULONG screen)
{
  ScreenDelete(screen);
}

/* ScreenSetCursor */
VOID ScreenSetCursor(ULONG screen, UBYTE cursor);
VOID EXPORT_ScreenSetCursor(__reg("a6") struct ParrotBase*, __reg("d0") ULONG screen, __reg("d1") UBYTE cursor)
{
  ScreenSetCursor(screen, cursor);
}

/* ScreenGetCursor */
UBYTE ScreenGetCursor(ULONG screen);
UBYTE EXPORT_ScreenGetCursor(__reg("a6") struct ParrotBase*, __reg("d0") ULONG screen)
{
  return ScreenGetCursor(screen);
}

/* ScreenSetColour */
VOID ScreenSetColour(ULONG screen, UWORD index, UBYTE r, UBYTE g, UBYTE b);
VOID EXPORT_ScreenSetColour(__reg("a6") struct ParrotBase*, __reg("d0") ULONG screen, __reg("d1") UWORD index, __reg("d2") UBYTE r, __reg("d3") UBYTE g, __reg("d4") UBYTE b)
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
  , (APTR) EXPORT_GameDelayTicks

/***END***/
