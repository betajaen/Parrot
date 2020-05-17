/**
    $Id: Game.c, 0.1, 2020/05/07 08:54:00, betajaen Exp $

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

#include <proto/exec.h>
#include <proto/dos.h>

extern struct ParrotBase* ParrotBase;

LONG RequesterF(CONST_STRPTR pOptions, CONST_STRPTR pFmt, ...);
ULONG StrFormat(CHAR* pBuffer, LONG pBufferCapacity, CHAR* pFmt, ...);
APTR OpenArchive(UBYTE id);
EXPORT VOID SetArchivesPath(CONST CHAR* path);
EXPORT APTR ArenaNew(ULONG size, ULONG requirements);

EXPORT APTR ArenaGame;

STATIC APTR ArenaChapter, ArenaRoom;
STATIC APTR GameArchive;

EXPORT VOID GameStart(CONST_STRPTR path)
{
  SetArchivesPath(path);
  ArenaGame = ArenaNew(16384, 0ul);
  ArenaChapter = NULL;
  ArenaRoom = NULL;

  GameArchive = OpenArchive(0);
}

EXPORT VOID GameDelayTicks(UWORD ticks)
{
  Delay(ticks);
}

EXPORT VOID GameDelaySeconds(UWORD seconds)
{
  Delay(seconds * 50);
}
